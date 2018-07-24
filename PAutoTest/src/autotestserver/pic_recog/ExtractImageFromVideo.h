#ifndef EXTRACTIMAGEFROMVIDEO_H
#define EXTRACTIMAGEFROMVIDEO_H

#include "ImageRecogInfo.h"
#include "ExtractPointsFromImage.h"



class ExtractImageFromVideo
{
public:
    ExtractImageFromVideo();
    ExtractImageFromVideo(const ExtractPointsFromImage& point);
    ~ExtractImageFromVideo();

    bool MakeDir(std::string& dirpath);
    bool VideoToImageRecog(std::string stdimagePath, int timeout);
    bool CheckCharWithStd(std::string OcrResult, std::string stdResult);
    std::vector<std::string> getSubChar(std::string str);
    int min(int a, int b, int c);
    int compute_distance(const char *strA, int len_a, const char *strB, int len_b, int **temp);
    void ShowCameraVideo();

    ExtractPointsFromImage& getPoint();
    void setPoint(const ExtractPointsFromImage &point);

    Mat& getFrame();
    void setFrame(const Mat &frame);

private:
    ExtractPointsFromImage m_point;
    Mat m_frame;
};

#endif // EXTRACTIMAGEFROMVIDEO_H
