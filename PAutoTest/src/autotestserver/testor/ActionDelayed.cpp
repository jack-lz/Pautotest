#include "ActionDelayed.h"
#include <unistd.h>


bool ActionDelayed::run(std::string casename, int seq)
{
    sleep(m_second);

    return true;
}
