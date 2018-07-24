#ifndef ACTIONJUDGEBEEPPROMPT_H
#define ACTIONJUDGEBEEPPROMPT_H

#include "Action.h"
#include <GetBeepLen.h>

class ActionJudgeBeepPrompt : public Action, public GetBeepLen
{
private:
    const std::string _filePath;
    BeepInfo* type;

public:
    ActionJudgeBeepPrompt(int timeout, std::string filePath,  int type)
        : Action(timeout)
        ,_filePath(filePath)
        , type(new BeepInfo(type))
    {

    }

    virtual ~ActionJudgeBeepPrompt()
    {
        if (this->type != NULL) {
            delete this->type;
            this->type = NULL;
        }
    }

    virtual bool run(std::string casename, int seq) override;
};

#endif /* ACTIONJUDGEBEEPPROMPT_H */
