#ifndef BEEPLEN_H
#define BEEPLEN_H

#include <string>
#include <vector>
#include <memory>
#include "ConfigAna.h"
#include "Action.h"

class GetBeepLen
{
public:
    float getBeepLength(int type);
    ~GetBeepLen(){}
};


class BeepInfo
{
public:
    BeepInfo(int type);
    int getBeepType();
    float getBeepLength();
    ~BeepInfo(){

    }
private:
    int type;
};

#endif /* BEEPLEN_H */
