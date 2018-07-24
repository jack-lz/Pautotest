#ifndef RECTIFYIMAGES_H
#define RECTIFYIMAGES_H


#include "ImageRecogInfo.h"

class RectifyImages
{
public:
    RectifyImages();
    ~RectifyImages() {};

    Mat ClipImages(Mat src, std::vector<cv::Point2f> corners, cv::Point2f center);
    void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f& center);
    void CalcDstSize(const std::vector<cv::Point2f>& corners);

private:
    int g_dst_hight;
    int g_dst_width;
};

#endif // RECTIFYIMAGES_H
