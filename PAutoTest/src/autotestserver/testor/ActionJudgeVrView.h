#ifndef ACTIONJUDGEVRVIEW_H
#define ACTIONJUDGEVRVIEW_H

#include <string>
#include <vector>
#include <memory>

#include "Action.h"

class ActionJudgeVrView : public Action
{
private:
    const std::string _filePath;

public:
    ActionJudgeVrView(int timeout, std::string filePath)
        :Action(timeout)
        ,_filePath(filePath)
    {}

    virtual ~ActionJudgeVrView()
    {}

    virtual bool run(std::string casename, int seq) override;
};

#endif /* ACTIONPLAYING_H */
