#include <fstream>
#include <iostream>
#include <thread>
#include <GetBeepLen.h>

float GetBeepLen::getBeepLength(int type)
{
    switch (type)
    {
        case 1:
            return g_configInfo.conf17.beep1.beep1_voice_len;
        case 2:
            return g_configInfo.conf17.beep2.beep2_voice_len;
        case 3:
            return g_configInfo.conf17.beep3.beep3_voice_len;
        default:
            return -1;
    }
}

BeepInfo::BeepInfo(int type)
{
    this->type = type;
}

int BeepInfo::getBeepType()
{
    return this->type;
}

float BeepInfo::getBeepLength()
{
    switch (type)
    {
        case 1:
            return g_configInfo.conf17.beep1.beep1_voice_len;
        case 2:
            return g_configInfo.conf17.beep2.beep2_voice_len;
        case 3:
            return g_configInfo.conf17.beep3.beep3_voice_len;
        default:
            return -1;
    }
}
