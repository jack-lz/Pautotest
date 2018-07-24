#include "ExtractPointsFromImage.h"
#include <iostream>
#include <imgproc/types_c.h>
#include "LOG.h"

using namespace cv;
using namespace std;

ExtractPointsFromImage::ExtractPointsFromImage()
{

}

bool ExtractPointsFromImage::ExtractElements(Mat srcImage)
{
//    resize(src, src, Size(800, 590), 0, 0, INTER_NEAREST);
//    cv::namedWindow("src img", WINDOW_NORMAL);
//    imshow("src img", srcImage);


    Mat img = srcImage.clone();
    cvtColor(img, img, CV_RGB2GRAY);   //二值化
//    cv::namedWindow("gray", WINDOW_NORMAL);
//    imshow("gray", img);
    //equalizeHist(img, img);
    //imshow("equal", img);
    GaussianBlur(img, img, Size(5, 5), 0, 0);  //高斯滤波

    //获取自定义核
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5)); //第一个参数MORPH_RECT表示矩形的卷积核，当然还可以选择椭圆形的、交叉型的
    //膨胀操作
    dilate(img, img, element);  //实现过程中发现，适当的膨胀很重要
//    namedWindow("dilate", WINDOW_NORMAL);
//    imshow("dilate", img);
    Canny(img, img, 50, 120, 3);   //边缘提取
//    namedWindow("get contour", WINDOW_NORMAL);
//    imshow("get contour", img);

    dilate(img, img, element); //实现过程中发现，适当的膨胀很重要

    vector<vector<Point> > contours;
    vector<vector<Point> > f_contours;
    vector<cv::Point> approx2;
    //注意第5个参数为CV_RETR_EXTERNAL，只检索外框
    findContours(img, f_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //找轮廓

    //求出面积最大的轮廓
    int max_area = 0;
    int index;
    for (int i = 0; i < f_contours.size(); i++)
    {
        double tmparea = fabs(contourArea(f_contours[i]));
        if (tmparea > max_area)
        {
            index = i;
            max_area = tmparea;
        }

    }
    contours.push_back(f_contours[index]);

    LOG("%d\n",contours.size());  //因为我写的是找出最外层轮廓，所以理论上只有一个轮廓

    vector<Point> tmp = contours[0];
    bool flag = false;

    for (int line_type = 1; line_type <= 3; line_type++)
    {
        LOG("line_type: %d\n", line_type);
        Mat black = img.clone();
        black.setTo(0);
        drawContours(black, contours, 0, Scalar(255), line_type);  //注意线的厚度，不要选择太细的
//        cv::namedWindow("show contour", WINDOW_NORMAL);
//        imshow("show contour", black);


        std::vector<Vec4i> lines;
        std::vector<cv::Point2f> approx;

        int para = 10;
        int round = 0;
        for (; para < 300; para++)
        {
//          LOG("round: %d\n", ++round);
            lines.clear();
            corners.clear();
            approx.clear();
            center = Point2f(0, 0);

            cv::HoughLinesP(black, lines, 1, CV_PI / 180, para, 150, 15);

            //过滤距离太近的直线
            std::set<int> ErasePt;
            for (int i = 0; i < lines.size(); i++)
            {
                for (int j = i + 1; j < lines.size(); j++)
                {
                    if (IsBadLine(abs(lines[i][0] - lines[j][0]), abs(lines[i][1] - lines[j][1])) && (IsBadLine(abs(lines[i][2] - lines[j][2]), abs(lines[i][3] - lines[j][3]))))
                    {
                        ErasePt.insert(j);//将该坏线加入集合
                    }
                }
            }

            int Num = lines.size();
            while (Num != 0)
            {
                std::set<int>::iterator j = ErasePt.find(Num);
                if (j != ErasePt.end())
                {
                    lines.erase(lines.begin() + Num - 1);
                }
                Num--;
            }
            if (lines.size() != 4)
            {
                continue;
            }

            //计算直线的交点，保存在图像范围内的部分
            for (int i = 0; i < lines.size(); i++)
            {
                for (int j = i + 1; j < lines.size(); j++)
                {
                    cv::Point2f pt = ComputeIntersect(lines[i], lines[j]);
                    if (pt.x >= 0 && pt.y >= 0 && pt.x <= srcImage.cols && pt.y <= srcImage.rows)             //保证交点在图像的范围之内
                        corners.push_back(pt);
                }
            }
            if (corners.size() != 4)
            {
//                LOG("corners.size isn't four!\n");
                continue;
            }
#if 1
            bool IsGoodPoints = true;

            //保证点与点的距离足够大以排除错误点
            for (int i = 0; i < corners.size(); i++)
            {
                for (int j = i + 1; j < corners.size(); j++)
                {
                    int distance = sqrt((corners[i].x - corners[j].x)*(corners[i].x - corners[j].x) + (corners[i].y - corners[j].y)*(corners[i].y - corners[j].y));
                    if (distance < 5)
                    {
                        IsGoodPoints = false;
                    }
                }
            }

            if (!IsGoodPoints) continue;
#endif
            cv::approxPolyDP(cv::Mat(corners), approx, cv::arcLength(cv::Mat(corners), true) * 0.02, true);

            if (lines.size() == 4 && corners.size() == 4 && approx.size() == 4)
            {
                // Get mass center
                for (int i = 0; i < corners.size(); i++)
                    center += corners[i];
                center *= (1. / corners.size());
                LOG("ExtractElements is succeed!\n");
                flag = true;
                return flag;
            }
        }
    }

    return flag;
//    waitKey();
}

bool ExtractPointsFromImage::IsBadLine(int a, int b)
{
    if (a * a + b * b < 100)
    {
        return true;
    }
    else
    {
        return false;
    }
}

cv::Point2f ExtractPointsFromImage::ComputeIntersect(cv::Vec4i a, cv::Vec4i b)
{
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
    int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

    if (float d = ((float)(x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4)))
    {
        cv::Point2f pt;
        pt.x = ((x1*y2 - y1*x2) * (x3 - x4) - (x1 - x2) * (x3*y4 - y3*x4)) / d;
        pt.y = ((x1*y2 - y1*x2) * (y3 - y4) - (y1 - y2) * (x3*y4 - y3*x4)) / d;
        return pt;
    }
    else
        return cv::Point2f(-1, -1);
}
std::vector<cv::Point2f> ExtractPointsFromImage::getCorners() const
{
    return corners;
}

void ExtractPointsFromImage::setCorners(const std::vector<cv::Point2f> &value)
{
    corners = value;
}
cv::Point2f ExtractPointsFromImage::getCenter() const
{
    return center;
}

void ExtractPointsFromImage::setCenter(const cv::Point2f &value)
{
    center = value;
}
