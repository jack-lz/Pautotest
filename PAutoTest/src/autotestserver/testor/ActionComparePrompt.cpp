#include "ActionComparePrompt.h"
#include "recog.h"
#include <sstream>
#include <time.h>
#include <string>
#include <json/json.h>
#include "audio_process.h"
#include <unistd.h>
#include "ConfigAna.h"
#include "LOG.h"


extern std::string g_sOutPath;

bool ActionComparePrompt::run(std::string casename, int seq)
{
    std::ostringstream oss;
    bool matchResult = true;
    std::string time;

    int iStart = _filePath.rfind("/");
    int iEnd = _filePath.rfind(".");
    std::string sOutFile = _filePath.substr(iStart + 1, iEnd - iStart - 1);

    time_t t;
    std::time (&t);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%m%d_%H%M%S",localtime(&t) );
    time = tmp;

    oss << g_sOutPath << "/record_" << casename << "_" << std::to_string(seq) << "_"<< sOutFile << "_"<< time << ".wav";
    std::string outFile = oss.str();

    WavFile *prompt = new WavFile();
    bool ret = prompt->openR(_filePath.c_str(), g_configInfo.sampling_rate);
    if (!ret) {
        LOG("open tts file failed, tts file not exist! return!!!!!\n");
        return false;
    }

    VadAudioDataFile vad;
    while (vad.m_needstop) {
        usleep(1000);
    }
    if (! vad.doVadToFile(outFile, _timeout)) {
        return false;
    }

    WavFile *ttsrecordfile = new WavFile();
    ttsrecordfile->openR(outFile.c_str(), g_configInfo.sampling_rate);
    if(ttsrecordfile->m_iDataLen == 0 )
    {
        LOG("tts record file is  empty!\n");
        return false;
    }

    std::string result = ProcessSelelct(outFile.c_str(), _filePath.c_str());

    Json::Reader reader;
    Json::Value root;
    if (reader.parse(result, root))
    {
        matchResult = root["matchResult"].asBool();
    }   
    LOG("ProcessSelelct matchResult:%s\n", result.c_str());


    prompt->close();
    ttsrecordfile->close();
    delete prompt;
    delete ttsrecordfile;
    return (matchResult);
}
