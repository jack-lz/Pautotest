#include "audio_process.h"
#include <sys/time.h>
#include <fstream>
#include "recog.h"
#include "LOG.h"

std::string Get_localtime()
{
    time_t cur_time;
    struct tm *local_time;
    //get current time;
    cur_time = time(NULL);
    local_time = localtime(&cur_time);
    std::string localtime_;
    localtime_ = std::to_string(local_time->tm_year+1900) + '-' + std::to_string((local_time->tm_mon + 1)) + '-' + std::to_string(local_time->tm_mday)
                + ' '+ std::to_string(local_time->tm_hour) + ':' + std::to_string(local_time->tm_min) + ':' + std::to_string(local_time->tm_sec);
   return localtime_;
}

AUDIO_RECOG_ERROR_TYPE JudgeOnly(const char * sourcefilepath, const char *savefilepath, std::string &result){
    if (!whether_exist(sourcefilepath))
    {
        //LOG("sourcefilepath:%s not exist! \n",sourcefilepath);
        return AUDIORECORD_ERROR_TYPE_FILE_NOTEXIST;
    }
    if (!whether_exist(savefilepath))
    {
        LOG("savefilepath:%s not exist! \n", savefilepath);
        return AUDIORECORD_ERROR_TYPE_FILE_NOTEXIST;
    }

    std::string source = sourcefilepath;
    std::string save = savefilepath;
    std::string source_wav = "SourceFile.wav";
    std::string save_wav = "saveFile.wav";

    //=========格式转换成wav=========

    std::string syscmd_source = "ffmpeg -i  " + source + " -f wav -ar 44100 -ac 1  " + source_wav + " -y";
    system(syscmd_source.c_str());

    if (!whether_exist(source_wav.c_str()))
    {
        LOG("SourceFile.wav not exist!\n");
        return AUDIORECORD_ERROR_TYPE_FILE_NOTEXIST;
    }

    std::string syscmd_save = "ffmpeg -i  " + save + " -f wav -ar 44100 -ac 1  " + save_wav + " -y";
    system(syscmd_save.c_str());

    if (!whether_exist(save_wav.c_str()))
    {
        LOG("saveFile.wav not exist! \n");
        return AUDIORECORD_ERROR_TYPE_FILE_NOTEXIST;
    }

    //=============Jugde=============
    std::string outcome;
    outcome = ProcessSelelct(source_wav.c_str(), save_wav.c_str());
    result = outcome;
//    LOG("result=%s\n",result.c_str());
    //============删除临时文件============
    std::string source_wav_str = "rm  " + source_wav;
    system(source_wav_str.c_str());
    std::string save_wav_str = "rm  " + save_wav;
    system(save_wav_str.c_str());

    return  AUDIORECORD_ERROR_TYPE_NO_ERROR;
}


