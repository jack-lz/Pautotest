#ifndef _IFLY_RES_SELECT_H_
#define _IFLY_RES_SELECT_H_


#include <string>

typedef enum AUDIORECOGNITION_ERROR_TYPE {
    AUDIORECOGNITION_ERROR_TYPE_NO_ERROR = 0,
    AUDIORECOGNITION_ERROR_TYPE_NULLPOINT
} AudioRecognitionErrorType;


//+---------------------------------------------------------------------------+
//+ 对指定的音乐及录的音乐做比对，输出比对结果（是否匹配及匹配起始时间）
//+---------------------------------------------------------------------------+
//+ szMusicPath		- 指定的音乐文件路径
//+ szAudioPath		- 录的音乐文件路径
//+ return      	- 输出结果，json格式
//+---------------------------------------------------------------------------+
std::string ProcessSelelct(const char *szMusicPath, const char *szAudioPath);

//+---------------------------------------------------------------------------+
//+ 判断指定的音频文件中是否存在beep音，输出判断结果（是否存在及存在起始时间）
//+---------------------------------------------------------------------------+
//+ szBeepFilePath		- 指定的音乐文件路径
//+ return      	- 输出结果，json格式
//+---------------------------------------------------------------------------+

std::string ProcessBeep(const char *szBeepFilePath, int beepType);
//typedef string(*Proc_ProcessSelelct)(const char *szMusicPath, const char *szAudioPath);

#endif//_IFLY_RES_SELECT_H_
