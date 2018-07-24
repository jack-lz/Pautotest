#include "ActionJudgeBeepBeep.h"
#include "ActionComparePrompt.h"
#include "audio_process.h"
#include "recog.h"
#include <sstream>
#include <time.h>
#include <string>
#include "ConfigAna.h"
#include <json/json.h>
#include <unistd.h>
#include "LOG.h"

extern std::string g_sOutPath;

bool ActionJudgeBeepBeep::run(std::string casename, int seq)
{
    std::ostringstream oss;
    bool matchResult1 = true;
    bool matchResult2 = true;
    std::string time;

    int iStart = _filePath.rfind("/");
    int iEnd = _filePath.rfind(".");
    std::string sOutFile = _filePath.substr(iStart + 1, iEnd - iStart - 1);

    time_t t;
    std::time (&t);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%m%d_%H%M%S",localtime(&t) );
    time = tmp;

    oss << g_sOutPath << "/record_" << casename<< "_" << std::to_string(seq)<< "_" << sOutFile << "_" <<time << ".wav";
    std::string outFile = oss.str();

    iEnd = outFile.rfind(".");

    std::string part1path = outFile.substr(0, iEnd - 1) + "_part1.wav";
    std::string part2path = outFile.substr(0, iEnd - 1) + "_part2.wav";

    VadAudioDataFile vad;
    while (vad.m_needstop) {
        usleep(1000);
    }
    if (! vad.doVadToFile(part1path, _timeout)) {
        return false;
    }
    WavFile *beeprecordfile1 = new WavFile();
    beeprecordfile1->openR(part1path.c_str(), g_configInfo.sampling_rate);
    if(beeprecordfile1->m_iDataLen == 0 )
    {
        LOG("beep record file1 is  empty!\n");
        return false;
    }


    while (vad.m_needstop) {
        usleep(1000);
    }
    if (! vad.doVadToFile(part2path, _timeout)) {
        return false;
    }
    WavFile *beeprecordfile2 = new WavFile();
    beeprecordfile2->openR(part2path.c_str(), g_configInfo.sampling_rate);
    if(beeprecordfile2->m_iDataLen == 0 )
    {
        LOG("beep record file2 is  empty!\n");
        return false;
    }

    beeprecordfile1->close();
    delete beeprecordfile1;
    beeprecordfile2->close();
    delete beeprecordfile2;
//=========================Judge beep1 ===========================

    std::string part1_result = ProcessBeep(part1path.c_str(), t1->getBeepType());
    Json::Reader reader1;
    Json::Value root1;
    if (reader1.parse(part1_result.c_str(), root1))
    {
        matchResult1 = root1["matchResult"].asBool();
    }
    LOG("ProcessBeep t1:%d matchResult1:%d\n", t1->getBeepType(), matchResult1);


//======================Judge beep2 ===========================

    std::string part2_result = ProcessBeep(part2path.c_str(), t2->getBeepType());
    Json::Reader reader2;
    Json::Value root2;
    if (reader2.parse(part2_result.c_str(), root2))
    {
        matchResult2 = root2["matchResult"].asBool();
    }
    LOG("ProcessBeep t2:%d matchResult2:%d\n", t2->getBeepType(), matchResult2);

    return (matchResult1 & matchResult2);

}
