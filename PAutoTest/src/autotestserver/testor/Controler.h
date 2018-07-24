
#ifndef CONTROLER_H
#define CONTROLER_H

#include<map>
#include<string>
#include <thread>
#include "ExtractImageFromVideo.h"
// #include "RecordButton_Define.h"

class Controler
{

public:
    Controler() {}

    void Run(const char* workSpacePath, const char* reportFile);
    void ShowCameraVideo();

private:
    ExtractImageFromVideo m_videoshower;
};

#endif /* CONTROLER_H */
