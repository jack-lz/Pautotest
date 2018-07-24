#ifndef ACTIONPLAYING_H
#define ACTIONPLAYING_H

#include <string>
#include <vector>
#include <memory>

#include "Action.h"

class ActionPlaying : public Action
{
private:
    const std::string _filePath;

    std::shared_ptr<std::vector<unsigned char>> readPcmData(std::string pcmFileName);
    void play(std::shared_ptr<std::vector<unsigned char>> data);

public:
    ActionPlaying(int timeout, std::string filePath)
        :Action(timeout)
        ,_filePath(filePath)
    {}

    virtual ~ActionPlaying()
    {}

    virtual bool run(std::string casename, int seq) override;
};

#endif /* ACTIONPLAYING_H */
