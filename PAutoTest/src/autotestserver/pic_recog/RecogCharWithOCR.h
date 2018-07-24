#ifndef RECOGCHARWITHOCR_H
#define RECOGCHARWITHOCR_H

#include <string>
#include "ImageRecogInfo.h"

class RecogCharWithOCR
{
public:
    RecogCharWithOCR();
    ~RecogCharWithOCR() {};


    std::string RecogImage(Mat quad_image, std::string lang);

private:


};

#endif // RECOGCHARWITHOCR_H
