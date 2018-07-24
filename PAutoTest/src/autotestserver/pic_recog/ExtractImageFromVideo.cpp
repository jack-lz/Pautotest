#include "ExtractImageFromVideo.h"
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <core/core_c.h>
#include <videoio/videoio_c.h>
#include <highgui/highgui_c.h>
#include "RectifyImages.h"
#include "RecogCharWithOCR.h"
#include "LOG.h"

using namespace cv;
using namespace std;

#define MAX(a,b)   ((a>b) ? a:b)


ExtractImageFromVideo::ExtractImageFromVideo()
{

}

ExtractImageFromVideo::ExtractImageFromVideo(const ExtractPointsFromImage &point)
    :m_point(point)
{

}

ExtractImageFromVideo::~ExtractImageFromVideo()
{

}



bool ExtractImageFromVideo::VideoToImageRecog(std::string stdimagePath, int timeout)
{
    struct timeval start, end;
    int duration;
    gettimeofday(&start, NULL);

    string dirpath;
    bool dirflag = MakeDir(dirpath);
    if (!dirflag) {
        LOG("make direct fail!\n");
        return false;
    }

    RectifyImages imageClip;
    RecogCharWithOCR recog;
    LOG("stdimagePath is = %s\n", stdimagePath.c_str());
    Mat std_image = imread(stdimagePath);

    string stdResult = recog.RecogImage(std_image, "eng");
    LOG("stdImage recognition is = %s\n", stdResult.c_str());


    IplImage* pFrame = NULL;
    CvCapture* pCapture = cvCreateCameraCapture(-1);
    cvSetCaptureProperty(pCapture,CV_CAP_PROP_FRAME_WIDTH,800);
    cvSetCaptureProperty(pCapture,CV_CAP_PROP_FRAME_HEIGHT,480);
    int j = 0;
    while(true){
        pFrame = cvQueryFrame( pCapture );
        if(!pFrame) {
            LOG("读取视频失败\n");
            cvReleaseCapture(&pCapture);
            return false;
        }


        char buffer[100] = { 0 };
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        snprintf(buffer, sizeof(buffer), "%spic_%04d%02d%03d.jpg", dirpath.c_str(),
                 (int)(ts.tv_sec/60), (int)(ts.tv_sec%60), (int)(ts.tv_nsec/1000000));
        Mat frame = cvarrToMat(pFrame);
        bool bRet = imwrite(buffer, frame); // 写入  前面是  path+name不要忘了后缀 后面是 帧
//        cvNamedWindow("result", 1);
//        cvShowImage("result", pFrame);

//        waitKey(0);

        if (m_point.getCorners().size() != 4) {
            if (!m_point.ExtractElements(frame)) {
                LOG("ExtractElements fail!\n");
                cvReleaseCapture(&pCapture);
                return false;
            }
        }

        Mat quad_image = imageClip.ClipImages(frame, m_point.getCorners(), m_point.getCenter());
        string OcrResult = recog.RecogImage(quad_image, "eng");

        LOG("OcrResult : %s\n", OcrResult.c_str());

        int len_ocr = strlen(OcrResult.c_str());
        int len_std = strlen(stdResult.c_str());

        int **temp = (int**)malloc(sizeof(int*) * (len_ocr + 1));
        for(int i = 0; i < len_ocr + 1; i++) {
            temp[i] = (int*)malloc(sizeof(int) * (len_std + 1));
            memset(temp[i], 0, sizeof(int) * (len_std + 1));
        }
        int distance = compute_distance(OcrResult.c_str(), len_ocr, stdResult.c_str(), len_std, temp);
        LOG("distance : %d\n", distance);

        float similar = 1 - float(distance) / float(MAX(len_ocr,len_std));
        LOG("similar : %f\n", similar);

//        bool check_result = CheckCharWithStd(OcrResult, stdResult);
//        LOG("check_result = %d\n", check_result);

        gettimeofday(&end, NULL);
        duration = (1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)) / 1000;

        if (similar >= 0.60) {
            cvReleaseCapture(&pCapture);
            LOG("pc_recog right!\n");
            return true;
        }
        else if(duration > timeout) {
            cvReleaseCapture(&pCapture);
            LOG("duration = %d\n", duration);
            return false;
        }

//      capture.set(CAP_PROP_POS_FRAMES, i*frame_rate); // 从此时的帧数开始获取帧
//      LOG("CAP_PROP_POS_FRAMES= %s\n", capture.get(CAP_PROP_POS_FRAMES));
//      capture.set(CAP_PROP_POS_MSEC, i * 1000); // 从此时的帧数开始获取帧
//      CmpPictures("template.jpg", image_name);
    }
}

bool ExtractImageFromVideo::MakeDir(string& dirpath)
{
    char *path = getenv("HOME");
    LOG("path_buff = %s\n", path);
    dirpath= string(path) + "/ImageFromVideo/";
    if (access(dirpath.c_str(), 0) == -1)
    {
        LOG("%s is not existing,now make it!\n", dirpath.c_str());
        int flag=mkdir(dirpath.c_str(), 0777);
        if (flag == 0)
        {
            return true;
        } else {
            return false;
        }
    }
    else {
        LOG("%s direct is existing!\n", dirpath.c_str());
        return true;
    }
}

bool ExtractImageFromVideo::CheckCharWithStd(string OcrResult, string stdResult)
{
    bool check_result = true;
    vector<string> vt_substr = getSubChar(stdResult);
    for (vector<string>::iterator iter = vt_substr.begin(); iter != vt_substr.end(); iter++) {
        int find_result = OcrResult.find(*iter,0);
        LOG("find %s result is %d\n", (*iter).c_str(), find_result);
        if (find_result < 0) {
            check_result = false;
            break;
        }
    }
    return check_result;
}

vector<string> ExtractImageFromVideo::getSubChar(string str)
{
    int bj=0;
    int ej1=0;
    int ej2=0;
    std::vector<string> vt_substr;

    while(ej1 = str.find("\"",bj))
    {
        if (ej1<1)
        {
            break;
        }
        bj = ej1+1;
        ej2 = str.find("\"",bj);

        string tempCCre = str.substr(ej1,ej2-ej1+1);
        vt_substr.push_back(tempCCre);

        bj = ej2+1;
        ej1 = 0;
    }
    return vt_substr;
}

int ExtractImageFromVideo::min(int a, int b, int c) {
    if(a < b) {
        if(a < c)
            return a;
        else
            return c;
    } else {
        if(b < c)
            return b;
        else
            return c;
    }
}

int ExtractImageFromVideo::compute_distance(const char *strA, int len_a, const char *strB, int len_b, int **temp) {
    int i, j;

    for(i = 1; i <= len_a; i++) {
        temp[i][0] = i;
    }

    for(j = 1; j <= len_b; j++) {
        temp[0][j] = j;
    }

    temp[0][0] = 0;

    for(i = 1; i <= len_a; i++) {
        for(j = 1; j <= len_b; j++) {
            if(strA[i -1] == strB[j - 1]) {
                temp[i][j] = temp[i - 1][j - 1];
            } else {
                temp[i][j] = min(temp[i - 1][j], temp[i][j - 1], temp[i - 1][j - 1]) + 1;
            }
        }
    }
    return temp[len_a][len_b];
}

void ExtractImageFromVideo::ShowCameraVideo()
{
    LOG("start ShowCameraVideo\n");

    IplImage* pFrame = NULL;
    cvNamedWindow("result", 1);
    CvCapture* pCapture = cvCreateCameraCapture(-1);
    cvSetCaptureProperty(pCapture,CV_CAP_PROP_FRAME_WIDTH,800);
    cvSetCaptureProperty(pCapture,CV_CAP_PROP_FRAME_HEIGHT,480);
    int j = 0;
    while(ImageRecogInfo::GetInstance()->videoflag()){
        pFrame = cvQueryFrame( pCapture );
        if(!pFrame) {
            LOG("open video fail!\n");
            break;
        }

        char c = cvWaitKey(33);
        if(c == 32) {
            break;
        }
        cvShowImage("result", pFrame);
        Mat test_frame = cvarrToMat(pFrame);
        test_frame.copyTo(m_frame);
    }
    cvReleaseCapture(&pCapture);
    cvDestroyWindow("result");
}

ExtractPointsFromImage& ExtractImageFromVideo::getPoint()
{
    return m_point;
}

void ExtractImageFromVideo::setPoint(const ExtractPointsFromImage &point)
{
    m_point = point;
}

Mat& ExtractImageFromVideo::getFrame()
{
    return m_frame;
}

void ExtractImageFromVideo::setFrame(const Mat &frame)
{
    frame.copyTo(m_frame);
}



