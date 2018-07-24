#include "ImageRecogInfo.h"
#include <mutex>
#include <iostream>

std::mutex    g_mutex;


ImageRecogInfo* ImageRecogInfo::instance = NULL;

ImageRecogInfo::~ImageRecogInfo()
{
    delete instance;
    instance = NULL;
}

ImageRecogInfo *ImageRecogInfo::GetInstance()
{
    if (instance == NULL) {
        instance = new ImageRecogInfo();
    }
    return instance;
}

ImageRecogInfo::ImageRecogInfo()
    :m_videoflag(true)
{

}
bool ImageRecogInfo::videoflag() const
{
    std::unique_lock<std::mutex>  lock(g_mutex);
    return m_videoflag;
}

void ImageRecogInfo::setVideoflag(bool videoflag)
{
    std::unique_lock<std::mutex>  lock(g_mutex);
    m_videoflag = videoflag;
}

ExtractPointsFromImage& ImageRecogInfo::point()
{
    std::unique_lock<std::mutex>  lock(g_mutex);
    return m_point;
}

void ImageRecogInfo::setPoint(const ExtractPointsFromImage &point)
{
    std::unique_lock<std::mutex>  lock(g_mutex);
    m_point = point;
}



