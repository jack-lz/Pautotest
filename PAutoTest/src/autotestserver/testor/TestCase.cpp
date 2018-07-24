#include "TestCase.h"
#include <iostream>
#include "LOG.h"

void TestCase::setFinal_action(spAction final_action)
{
    _final_action = final_action;
}

bool TestCase::Run()
{
    bool flag = true;
    int seq = 0;
    for (auto action : _actions) {
        if (! action->run(_name, seq) ) {
            flag = false;
            LOG("################################# Result failed,flag = %d \n", flag);
            break;
        }
        ++seq;
    }
    _final_action->run(_name, seq);
    return flag;
}
