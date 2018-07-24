#ifndef ACTIONJUDGEBEEP_H
#define ACTIONJUDGEBEEP_H

#include "Action.h"
#include "GetBeepLen.h"

class ActionJudgeBeep : public Action, public GetBeepLen
{
private:
    const std::string _filePath;
    BeepInfo* type;

public:

    ActionJudgeBeep(int timeout, std::string filePath,  int type)
        : Action(timeout)
        ,_filePath(filePath)
        , type(new BeepInfo(type))
    {
    }

    virtual ~ActionJudgeBeep()
    {
        if (this->type != NULL) {
            delete this->type;
            this->type = NULL;
        }
    }

    virtual bool run(std::string casename, int seq) override;
};

#endif /* ACTIONJUDGEBEEP_H */
