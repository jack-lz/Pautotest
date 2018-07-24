#ifndef EXTRACTPOINTSFROMIMAGE_H
#define EXTRACTPOINTSFROMIMAGE_H

#include <string>
#include <vector>
#include <set>
#include <imgproc/imgproc.hpp>
#include <calib3d.hpp>
#include <calib3d/calib3d.hpp>
#include <features2d.hpp>
#include <xfeatures2d.hpp>
#include <xfeatures2d/nonfree.hpp>
#include <xfeatures2d/cuda.hpp>
#include <highgui.hpp>
#include <core/cuda.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


class ExtractPointsFromImage
{
public:
    ExtractPointsFromImage();
    ~ExtractPointsFromImage() {};

    bool ExtractElements(cv::Mat srcImage);
    bool IsBadLine(int a, int b);
    cv::Point2f ComputeIntersect(cv::Vec4i a, cv::Vec4i b);


    std::vector<cv::Point2f> getCorners() const;
    void setCorners(const std::vector<cv::Point2f> &value);

    cv::Point2f getCenter() const;
    void setCenter(const cv::Point2f &value);

private:
    std::vector<cv::Point2f> corners;
    cv::Point2f center;

};

#endif // EXTRACTPOINTSFROMIMAGE_H
