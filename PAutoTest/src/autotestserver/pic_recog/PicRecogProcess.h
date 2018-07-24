#ifndef PICRECOGPROCESS_H
#define PICRECOGPROCESS_H

#include <string>

class PicRecogProcess
{
public:
    PicRecogProcess();
    ~PicRecogProcess();

    bool Run(std::string stdimagePath, int timeout);


private:




};

#endif // PICRECOGPROCESS_H
