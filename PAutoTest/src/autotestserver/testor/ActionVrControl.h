#ifndef ACTIONVRCONTROL_H
#define ACTIONVRCONTROL_H

#include "Action.h"
#include "SocketServer.h"

#include <string>

class ActionVrControl : public Action
{
protected:
    enum HardKeyValue
    {
        HOME_L1 = 0,
        HookOnKey = 12,
        VoiceKey = 13,
        HomeKey = 40,
        VolumeUp = 3
    };

    enum MouseEventType
    {
        PressKey,
        ReleaseKey
    };

private:
    SocketServer &_ss;
    uint m_headUsefulLen;
    uint m_bodyUsefulLen;
    uint m_tailUsefulLen;
    uint m_totalUsefulLen;
    char m_remoteMsgHead[32];
    char m_msgBody[32];
    char m_checkSum;
    char m_msgTail[32];
    char m_totalMsg[128];
    HardKeyValue m_hardKeyValue;

protected:
    void init(HardKeyValue key);
    char packageChar(char highFourBitValue, char lowFourBitValue);
    char charToUint(char value);
    void creatMsg(int keyValue, MouseEventType type);

public:
    ActionVrControl(int timeout, SocketServer &ss, std::string final, HardKeyValue value)
        :Action(timeout, final)
        ,_ss(ss)
        , m_hardKeyValue(value)
    {
        init(m_hardKeyValue);
    }

    virtual ~ActionVrControl()
    {}

    virtual bool run(std::string casename, int seq) override;
    std::string get_final() const;

    int getHardKeyValue() const;
    SocketServer &getSs() const;
    uint getTotalUsefulLen() const;
    char *getTotalMsg();
};

class ActionVrStart : public ActionVrControl
{
public:
    ActionVrStart(int timeout,SocketServer &ss)
        :ActionVrControl(timeout, ss, "", VoiceKey)
    {}
};

class ActionVrStop : public ActionVrControl
{
public:
    ActionVrStop(int timeout,SocketServer &ss, std::string final)
        :ActionVrControl(timeout, ss, final, HomeKey)
    {}
};

class ActionVrStopL1 : public ActionVrStop
{
public:
    ActionVrStopL1(int timeout,SocketServer &ss, std::string final)
        :ActionVrStop(timeout,ss, final)
        ,_ss(ss)
    {
        init();
    }
    bool run(std::string casename, int seq) override;

protected:
    void init();
    void creatMsg(int keyValue, MouseEventType type);

private:
    SocketServer &_ss;
    char m_msgTail[32];
    char m_checkSum;
    char m_msgBody[32];
    char m_totalMsg[128];
    char m_hardkeyMsgHead[32];
};

class ActionHookOn : public ActionVrControl
{
public:
    ActionHookOn(int timeout,SocketServer &ss)
        :ActionVrControl(timeout, ss, "", HookOnKey)
    {}
};

class ActionLongPressPTT : public ActionVrControl
{
public:
    ActionLongPressPTT(int timeout,SocketServer &ss)
        :ActionVrControl(timeout, ss, "", VoiceKey)
    {}
    virtual bool run(std::string casename, int seq) override;
    bool runInThread() ;
};

class ActionVolumeUp : public ActionVrControl
{
public:
    ActionVolumeUp(int timeout,SocketServer &ss)
        :ActionVrControl(timeout, ss, "", VolumeUp)
    {}
};

#endif /* ACTIONVRCONTROL_H */
