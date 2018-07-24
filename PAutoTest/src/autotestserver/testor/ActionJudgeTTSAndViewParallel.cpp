#include "ActionJudgeTTSAndViewParallel.h"

//#include "Audio_Recog.h"
#include "audio_process.h"
#include "PicRecogProcess.h"
#include "recog.h"
#include <sstream>
#include <iostream>
#include <time.h>
#include <string>
#include "LOG.h"

#include <json/json.h>

using namespace std;

//int count1 = 0;
extern std::string g_sOutPath;

void ActionJudgeTTSAndViewParallel::audioThread()
{

    LOG("audio_recog thread start!\n");
    std::ostringstream oss;
    bool matchResult = true;
    std::string time;

    int iStart = _audioFilePath.rfind("/");
    int iEnd = _audioFilePath.rfind(".");
    std::string sOutFile = _audioFilePath.substr(iStart + 1, iEnd - iStart - 1);

    time_t t;
    std::time (&t);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%m%d-%H%M%S",localtime(&t) );
    time = tmp;

    oss << g_sOutPath << "/record_" << sOutFile << time << ".wav";
    std::string outFile = oss.str();
//    count1 ++;
#if 0
    setAudioResult(true);
    return;
#endif
    VadAudioDataFile vad;
    if (! vad.doVadToFile(outFile, _audioTimeout)) {
        setAudioResult(false);
        return ;
    }

    std::string result = ProcessSelelct(outFile.c_str(), _audioFilePath.c_str());

    Json::Reader reader;
    Json::Value root;
    if (reader.parse(result, root))
    {
        matchResult = root["matchResult"].asBool();
    }
//  LOG("ProcessSelelct _filePath:%s outFile:%s result:%s \n", _filePath.c_str(), outFile.c_str(), result.c_str());
    LOG("ProcessBeep beep_Type:%d\n", matchResult);
    setAudioResult (matchResult);
    return;
}

void ActionJudgeTTSAndViewParallel::picThread()
{
    PicRecogProcess pic_recog;
    LOG("pic_recog thread start!\n");
    setPicResult(pic_recog.Run(_picFilePath, _picTimeout));
}

bool ActionJudgeTTSAndViewParallel::run(std::string casename, int seq)
{
    std::thread taskAudio(&ActionJudgeTTSAndViewParallel::audioThread,this);
    std::thread taskPic(&ActionJudgeTTSAndViewParallel::picThread,this);

    taskAudio.join();
    taskPic.join();

    return (getPicResult() && getAudioResult());
}
