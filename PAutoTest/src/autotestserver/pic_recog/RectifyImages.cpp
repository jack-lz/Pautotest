#include "RectifyImages.h"
#include <iostream>
#include <sys/time.h>
#include "LOG.h"

using namespace std;

RectifyImages::RectifyImages()
    :g_dst_width(0)
    ,g_dst_hight(0)
{

}

Mat RectifyImages::ClipImages(Mat src, std::vector<cv::Point2f> corners, cv::Point2f center)
{
    Mat source = src.clone();
    Mat bkup = src.clone();
    LOG("ClipImages process!\n");
//    cv::circle(bkup, corners[0], 3, CV_RGB(255, 0, 0), -1);
//    cv::circle(bkup, corners[1], 3, CV_RGB(0, 255, 0), -1);
//    cv::circle(bkup, corners[2], 3, CV_RGB(0, 0, 255), -1);
//    cv::circle(bkup, corners[3], 3, CV_RGB(255, 255, 255), -1);
//    cv::circle(bkup, center, 3, CV_RGB(255, 0, 255), -1);


    sortCorners(corners, center);
    LOG("corners size = %f\n", corners.size());
    LOG("tl: %f\n", corners[0]);
    LOG("tr: %f\n", corners[1]);
    LOG("br: %f\n", corners[2]);
    LOG("bl: %f\n", corners[3]);
    CalcDstSize(corners);

    cv::Mat quad = cv::Mat::zeros(g_dst_hight, g_dst_width, CV_8UC3);
    std::vector<cv::Point2f> quad_pts;
    quad_pts.push_back(cv::Point2f(0, 0));
    quad_pts.push_back(cv::Point2f(quad.cols, 0));
    quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));

    quad_pts.push_back(cv::Point2f(0, quad.rows));

    cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);
    cv::warpPerspective(source, quad, transmtx, quad.size());

//    cv::namedWindow("find", WINDOW_NORMAL);
//    imshow("find", bkup);

//    cv::namedWindow("quadrilateral", WINDOW_NORMAL);
//    cv::imshow("quadrilateral", quad);
//    waitKey(0);

    // 保存
    char buffer[100] = { 0 };
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    snprintf(buffer, sizeof(buffer), "pic_%04d%02d%03d.jpg",
             (int)(ts.tv_sec/60), (int)(ts.tv_sec%60), (int)(ts.tv_nsec/1000000));
    string quadPath = string("/home/liuzhen/test/test_pic/quadrilateral_pic/") + string(buffer);

    imwrite(quadPath, quad);


    /*如果需要二值化就解掉注释把*/
    /*
     Mat local,gray;
     cvtColor(quad, gray, CV_RGB2GRAY);
     int blockSize = 25;
     int constValue = 10;
     adaptiveThreshold(gray, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, constValue);
     imshow("二值化", local);
     */
    return quad;
}

bool x_sort(const Point2f & m1, const Point2f & m2)
{
    return m1.x < m2.x;
}

//确定四个点的中心线
void RectifyImages::sortCorners(std::vector<cv::Point2f>& corners,
    cv::Point2f& center)
{
    std::vector<cv::Point2f> top, bot;
    vector<Point2f> backup = corners;

    sort(corners.begin(),corners.end(), x_sort);  //注意先按x的大小给4个点排序

    for (int i = 0; i < corners.size(); i++)
    {
        if (corners[i].y < center.y && top.size() < 2)    //这里的小于2是为了避免三个顶点都在top的情况
            top.push_back(corners[i]);
        else
            bot.push_back(corners[i]);
    }
    corners.clear();

    if (top.size() == 2 && bot.size() == 2)
    {
        //LOG("log\n");
        cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
        cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
        cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
        cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];


        corners.push_back(tl);
        corners.push_back(tr);
        corners.push_back(br);
        corners.push_back(bl);
    }
    else
    {
        corners = backup;
    }
}

void RectifyImages::CalcDstSize(const vector<cv::Point2f>& corners)
{
    int h1 = sqrt((corners[0].x - corners[3].x)*(corners[0].x - corners[3].x) + (corners[0].y - corners[3].y)*(corners[0].y - corners[3].y));
    int h2 = sqrt((corners[1].x - corners[2].x)*(corners[1].x - corners[2].x) + (corners[1].y - corners[2].y)*(corners[1].y - corners[2].y));
    g_dst_hight = MAX(h1, h2);

    int w1 = sqrt((corners[0].x - corners[1].x)*(corners[0].x - corners[1].x) + (corners[0].y - corners[1].y)*(corners[0].y - corners[1].y));
    int w2 = sqrt((corners[2].x - corners[3].x)*(corners[2].x - corners[3].x) + (corners[2].y - corners[3].y)*(corners[2].y - corners[3].y));
    g_dst_width = MAX(w1, w2);
}


