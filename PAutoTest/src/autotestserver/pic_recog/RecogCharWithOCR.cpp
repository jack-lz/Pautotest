#include "RecogCharWithOCR.h"
#include <iostream>
#include <core/core_c.h>
#include "LOG.h"

using namespace std;

RecogCharWithOCR::RecogCharWithOCR()
{

}

string RecogCharWithOCR::RecogImage(Mat quad_image, string lang)
{
    char *outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, lang.c_str(), tesseract::OEM_TESSERACT_ONLY)) {  // eng   chi_sim
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    Mat imageROI;
    int log_width = quad_image.rows*15/100;
    imageROI = quad_image(Rect(0, log_width, quad_image.cols, quad_image.rows-log_width));


//    cv::namedWindow("imageROI", WINDOW_NORMAL);
//    cv::imshow("imageROI", imageROI);
//    waitKey(0);


    IplImage imgTmp = imageROI;
    IplImage *iplimg = cvCloneImage(&imgTmp);
    api->SetImage((unsigned char*)(iplimg->imageData), iplimg->width, iplimg->height, iplimg->nChannels, iplimg->widthStep);

    // Get OCR result
    outText = api->GetUTF8Text();
    LOG("outText = %s\n", outText);

    // Destroy used object and release memory
    api->End();
    string OcrResult = string(outText);
    delete [] outText;
    return OcrResult;
}

