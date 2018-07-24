#include "ActionJudgeBeep.h"
#include "recog.h"
#include <sstream>
#include <time.h>
#include <string>
#include <json/json.h>
#include "audio_process.h"
#include <unistd.h>
#include "LOG.h"


extern std::string g_sOutPath;

bool ActionJudgeBeep::run(std::string casename, int seq)
{
    std::ostringstream oss;
    bool matchResult = true;
    std::string time;

    time_t t;
    std::time (&t);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%m%d_%H%M%S",localtime(&t) );
    time = tmp;

    oss << g_sOutPath << "/record_" <<casename << "_" << std::to_string(seq) << "_"<< time << ".wav";
    std::string outFile = oss.str();

    VadAudioDataFile vad;

    if (! vad.doVadToFile(outFile, _timeout)) {
        LOG("JudegBeep::vad.doVadToFile(outFile) is false\n");
        return false;
    }

    while (vad.m_needstop) {
        usleep(1000);
    }

    WavFile *beeprecordfile = new WavFile();
    beeprecordfile->openR(outFile.c_str(), g_configInfo.sampling_rate);
    if(beeprecordfile->m_iDataLen == 0 )
    {
        LOG("beep record file is  empty!\n");
        return false;
    }

    // FixMe: parse the result
    std::string result = ProcessBeep(outFile.c_str(), type->getBeepType());
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(result, root))
    {
        matchResult = root["matchResult"].asBool();
    }
    LOG("ProcessBeep matchResult:%d \n", matchResult);
    beeprecordfile->close();
    delete beeprecordfile;

    return (matchResult);
}
