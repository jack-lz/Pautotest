#ifndef ACTIONCOMPAREPROMPT_H
#define ACTIONCOMPAREPROMPT_H

#include "Action.h"

class ActionComparePrompt : public Action
{
private:
    const std::string _filePath;

public:
    ActionComparePrompt(int timeout, std::string filePath)
        :Action(timeout)
        ,_filePath(filePath)
    {}

    virtual ~ActionComparePrompt()
    {}

    virtual bool run(std::string casename, int seq) override;
};

#endif /* ACTIONCOMPAREPROMPT_H */
