#ifndef ACTIONDELAYED_H
#define ACTIONDELAYED_H

#include <string>

#include "Action.h"

class ActionDelayed : public Action
{
private:
    int m_second;
public:
    ActionDelayed(int timeout, int second)
        :Action(timeout)
        ,m_second(second)
    {}

    virtual ~ActionDelayed()
    {}

    virtual bool run(std::string casename, int seq) override;
};

#endif /* ACTIONDELAYED_H */
