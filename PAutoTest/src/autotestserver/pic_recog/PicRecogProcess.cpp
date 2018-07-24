#include "PicRecogProcess.h"
#include "ExtractImageFromVideo.h"
#include "LOG.h"

#include <iostream>
using namespace std;

PicRecogProcess::PicRecogProcess()
{

}

PicRecogProcess::~PicRecogProcess()
{

}

bool PicRecogProcess::Run(std::string stdimagePath, int timeout)
{
    ExtractImageFromVideo videoParser(ImageRecogInfo::GetInstance()->point());
    LOG("PicRecogProcess:stdimagePath: %s\n", stdimagePath.c_str());
    bool recog_result = videoParser.VideoToImageRecog(stdimagePath, timeout);
    LOG("pic_recog_result : %d\n", recog_result);
    return recog_result;

//    if (m_point.ExtractElements(src)) {

//        RectifyImages imageClip;
//        string quadPath = imageClip.ClipImages(src);

//        RecogCharWithOCR recog;
//        string OcrResult = recog.RecogImage("/home/liuzhen/test/test_pic/template_pic/voice.bmp", "eng");
//        LOG("OcrResult : %s\n", OcrResult.c_str());
//    }
}

