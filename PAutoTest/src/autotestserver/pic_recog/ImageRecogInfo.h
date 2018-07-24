#ifndef IMAGERECOGINFO_H
#define IMAGERECOGINFO_H

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
#include "ExtractPointsFromImage.h"
#include <core/types_c.h>
#include <core/types.hpp>
#define NUM_FRAME 10 //只处理前10帧，根据视频帧数可修改
#define MAX_ALPHA 10.0
#define MAX_TRACKBAR 5


using namespace cv;

class ImageRecogInfo
{
public:

    ~ImageRecogInfo();
    static ImageRecogInfo* GetInstance();

    ExtractPointsFromImage& point();
    void setPoint(const ExtractPointsFromImage &point);

    bool videoflag() const;
    void setVideoflag(bool videoflag);

private:
    ImageRecogInfo();
    ImageRecogInfo(const ImageRecogInfo&);
    static ImageRecogInfo* instance;
    ExtractPointsFromImage m_point;
    bool m_videoflag;

};

#endif // IMAGERECOGINFO_H
