#ifndef ACTIONJUDGEPROMPTBEEP_H
#define ACTIONJUDGEPROMPTBEEP_H

#include "Action.h"
#include <GetBeepLen.h>
#include <iostream>
#include "LOG.h"

class ActionJudgePromptBeep : public Action
{
private:
    const std::string _filePath;
    BeepInfo* type;
    
public:
    ActionJudgePromptBeep(int timeout, std::string filePath,  int type)
        : Action(timeout)
        ,_filePath(filePath)
        , type(new BeepInfo(type))
    {
        LOG("%s type = %d \n", __FUNCTION__, type);
    }

    virtual ~ActionJudgePromptBeep()
    {
        LOG("%s \n", __FUNCTION__);
        if (this->type != NULL) {
            delete this->type;
            this->type = NULL;
        }
    }

    virtual bool run(std::string casename, int seq) override;
};

#endif /* ACTIONJUDGEPROMPTBEEP_H */

