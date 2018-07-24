#include "ActionJudgeEmptyRecordFile.h"
#include "audio_process.h"
#include "recog.h"
#include <sstream>
#include <time.h>
#include <string>
#include "ConfigAna.h"
#include <json/json.h>
#include "LOG.h"

extern std::string g_sOutPath;

bool ActionJudgeEmptyRecordFile::run(std::string casename, int seq)
{
    std::ostringstream oss;
    std::string time;

    int iStart = _filePath.rfind("/");
    int iEnd = _filePath.rfind(".");
    std::string sOutFile = _filePath.substr(iStart + 1, iEnd - iStart - 1);

    time_t t;
    std::time (&t);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%m%d_%H%M%S",localtime(&t) );
    time = tmp;

    oss << g_sOutPath << "/record_" << casename<< "_" <<std::to_string(seq) << "_"<< sOutFile << "_"<< time << ".wav";
    std::string outFile = oss.str();

    VadAudioDataFile vad;
    if (! vad.doVadToFile(outFile, _timeout)) {
        return false;
    }

    WavFile *whole = new WavFile();
    whole->openR(outFile.c_str(), g_configInfo.sampling_rate);
    if(whole->m_iDataLen == 0 )
    {
//        LOG("record file is  empty!\n");
        return true;
    }
    else
    {
        LOG("record file is not empty!\n");
        return false;
    }
    whole->close();
    delete whole;
}
