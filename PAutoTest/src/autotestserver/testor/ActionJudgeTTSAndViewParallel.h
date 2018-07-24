#ifndef ACTIONJUDGETTSANDVIEWPARALLEL_H
#define ACTIONJUDGETTSANDVIEWPARALLEL_H

#include <thread>
#include "Action.h"

class ActionJudgeTTSAndViewParallel : public Action
{
public:
    ActionJudgeTTSAndViewParallel(std::string audioType
            , std::string audioFilePath, int audioTimeout
            , std::string picFilePath, int picTimeout)
        :Action(audioTimeout)
        ,_audioType(audioType)
        ,_audioFilePath(audioFilePath)
        ,_audioTimeout(audioTimeout)
        ,_picFilePath(picFilePath)
        ,_picTimeout(picTimeout)
        ,_audioResult(false)
        ,_picResult(false)
    {}

    virtual ~ActionJudgeTTSAndViewParallel()
    {}

    virtual bool run(std::string casename, int seq) override;


private:
    void audioThread();
    void picThread();

    void setAudioResult(bool result) { _audioResult = result; }
    bool getAudioResult() { return _audioResult; }

    void setPicResult(bool result) { _picResult = result; }
    bool getPicResult() { return _picResult; }

    const std::string _audioType;
    const std::string _audioFilePath;
    const int _audioTimeout;
    const std::string _picFilePath;
    const int _picTimeout;
    bool _audioResult;
    bool _picResult;
};

#endif /* ACTIONJUDGETTSANDVIEWPARALLEL_H  */
