#include <string.h>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <thread>
#include "LOG.h"

#include "ActionVrControl.h"
#include<string.h>
#include<unistd.h>


int ActionVrControl::getHardKeyValue() const
{
    return m_hardKeyValue;
}

SocketServer &ActionVrControl::getSs() const
{
    return _ss;
}

uint ActionVrControl::getTotalUsefulLen() const
{
    return m_totalUsefulLen;
}

char *ActionVrControl::getTotalMsg()
{
    return m_totalMsg;
}

void ActionVrControl::init(ActionVrControl::HardKeyValue key)
{
    m_checkSum = 0;
    memset(m_remoteMsgHead, 0, sizeof(m_remoteMsgHead));
    memset(m_msgBody, 0, sizeof(m_msgBody));
    memset(m_msgTail, 0, sizeof(m_msgTail));
    std::string msgHead;
    std::string msgTail;
    uint msgBodySize = 0;
    if (VoiceKey == key) {
        msgHead = "9f0201200120252300092000000096";
        msgTail = "9f03";
        msgBodySize = 4;
    }
    else if (HomeKey == key) {
        msgHead = "9f02012001202523000d2000000094";
        msgTail = "9f03";
        msgBodySize = 8;
    }
    else if (HookOnKey == key) {
        msgHead = "9f0201200120252300092000000096";
        msgTail = "9f03";
        msgBodySize = 4;
    }
    else if (VolumeUp == key) {
        msgHead = "9f0201200120252300092000000096";
        msgTail = "9f03";
        msgBodySize = 4;
    }

    m_headUsefulLen = msgHead.length()/2;
    m_bodyUsefulLen = msgBodySize;
    m_tailUsefulLen = msgTail.length()/2;
    m_totalUsefulLen = m_headUsefulLen + m_bodyUsefulLen + 1 + m_tailUsefulLen;
    for(int i = 0, j = 0; i < msgHead.length()-1; j++)
    {
        char high = msgHead.at(i);
        char low = msgHead.at(i+1);
        char result = packageChar(charToUint(high), charToUint(low));
        memcpy((void *)(m_remoteMsgHead + j), (void *)(&result), 1);
        i += 2;
    }
    for(int i = 0, j = 0; i < msgTail.length()-1; j++)
    {
        char high = msgTail.at(i);
        char low = msgTail.at(i+1);
        char result = packageChar(charToUint(high), charToUint(low));
        memcpy((void *)(m_msgTail + j), (void *)(&result), 1);
        i += 2;
    }
}

char ActionVrControl::packageChar(char highFourBitValue, char lowFourBitValue)
{
    return highFourBitValue*16 + lowFourBitValue;
}

char ActionVrControl::charToUint(char value)
{
    char result;
    switch(value)
    {
        case '0':
            result = 0;
            break;
        case '1':
            result = 1;
            break;
        case '2':
            result = 2;
            break;
        case '3':
            result = 3;
            break;
        case '4':
            result = 4;
            break;
        case '5':
            result = 5;
            break;
        case '6':
            result = 6;
            break;
        case '7':
            result = 7;
            break;
        case '8':
            result = 8;
            break;
        case '9':
            result = 9;
            break;
        case 'a':
            result = 10;
            break;
        case 'b':
            result = 11;
            break;
        case 'c':
            result = 12;
            break;
        case 'd':
            result = 13;
            break;
        case 'e':
            result = 14;
            break;
        case 'f':
            result = 15;
            break;
        default:
            result = 0;
            break;
    }
    return result;
}

void ActionVrControl::creatMsg(int keyValue, ActionVrControl::MouseEventType type)
{
    memset(m_msgBody, 0, sizeof(m_msgBody));
    if(PressKey == type)
    {
        int index = (keyValue -1)/8;
        int bit = (8 - keyValue%8)%8;
        char mod = 1;
        mod <<= bit;
        m_msgBody[index] |= mod;
    }
    m_checkSum = 0;
    for(int i = 2; i < m_headUsefulLen; i++)
    {
        m_checkSum ^=  m_remoteMsgHead[i];
    }
    for(int i = 0; i < m_bodyUsefulLen; i++)
    {
        m_checkSum ^=  m_msgBody[i];
    }
    memset(m_totalMsg, 0, sizeof(m_totalMsg));
    memcpy((void*)m_totalMsg, (void*)m_remoteMsgHead, m_headUsefulLen);
    memcpy((void*)(m_totalMsg + m_headUsefulLen), (void*)m_msgBody, m_bodyUsefulLen);
    memcpy((void*)(m_totalMsg + m_headUsefulLen + m_bodyUsefulLen), (void*)(&m_checkSum), sizeof(m_checkSum));
    memcpy((void*)(m_totalMsg + m_headUsefulLen + m_bodyUsefulLen + sizeof(m_checkSum)), (void*)m_msgTail, m_tailUsefulLen);

    return;
}

bool ActionVrControl::run(std::string casename, int seq)
{
    creatMsg(m_hardKeyValue, PressKey);
    _ss.SendMsg(m_totalMsg, m_totalUsefulLen);
    creatMsg(m_hardKeyValue, ReleaseKey);
    _ss.SendMsg(m_totalMsg, m_totalUsefulLen);
    return true;
}

std::string ActionVrControl::get_final() const
{
    return _final;
}


bool ActionLongPressPTT::run(std::__cxx11::string casename, int seq)
{
    std::thread taskLongPressPTT(&ActionLongPressPTT::runInThread,this);
    taskLongPressPTT.detach();
    return true;
}

bool ActionLongPressPTT::runInThread()
{
    LOG("ActionLongPressPTT %d\n", getHardKeyValue());
    creatMsg((HardKeyValue)getHardKeyValue(), PressKey);
    getSs().SendMsg(getTotalMsg(), getTotalUsefulLen());
    sleep(1);
    creatMsg((HardKeyValue)getHardKeyValue(), ReleaseKey);
    getSs().SendMsg(getTotalMsg(), getTotalUsefulLen());
    LOG("ActionLongPressPTT end\n");
    return true;
}


bool ActionVrStopL1::run(std::string casename, int seq)
{
    creatMsg(HOME_L1, PressKey);
    _ss.SendMsg(m_totalMsg, 19);

    creatMsg(HOME_L1, ReleaseKey);
    _ss.SendMsg(m_totalMsg, 19);
    return true;
}

void ActionVrStopL1::init()
{
    m_msgTail[0] = 0x9f;
    m_msgTail[1] = 0x03;
    m_hardkeyMsgHead[0] = 0x9f;
    m_hardkeyMsgHead[1] = 0x02;
    m_hardkeyMsgHead[2] = 0x01;
    m_hardkeyMsgHead[3] = 0x20;
    m_hardkeyMsgHead[4] = 0x01;
    m_hardkeyMsgHead[5] = 0x20;
    m_hardkeyMsgHead[6] = 0xf3;
    m_hardkeyMsgHead[7] = 0x23;
    m_hardkeyMsgHead[8] = 0x00;
    m_hardkeyMsgHead[9] = 0x06;
    m_hardkeyMsgHead[10] = 0x20;
    m_hardkeyMsgHead[11] = 0x00;
    m_hardkeyMsgHead[12] = 0x00;
    m_hardkeyMsgHead[13] = 0x00;
    m_hardkeyMsgHead[14] = 0x76;
}

void ActionVrStopL1::creatMsg(int key, MouseEventType isRelease)
{
    memset((void *)m_totalMsg, 0, sizeof(m_totalMsg));
    memset((void *)m_msgBody, 0, sizeof(m_msgBody));
    m_checkSum = 0;

    if (!isRelease) {
        m_msgBody[0] |= 1 << (7 - key);
    }
    // build  message
    for (int i = 2; i < 15; i++) {
        m_checkSum ^= m_hardkeyMsgHead[i];
    }
    for (int i = 0; i < 1; i++) {
        m_checkSum ^= m_msgBody[i];
    }
    memcpy((void*)m_totalMsg, (void *)m_hardkeyMsgHead, 15);
    memcpy((void*)m_totalMsg + 15, (void *)m_msgBody, 1);
    memcpy((void*)m_totalMsg + 15 + 1, (void *)(&m_checkSum), 1);
    memcpy((void*)m_totalMsg + 15 + 1 + 1, (void *)(&m_msgTail), 2);
}
