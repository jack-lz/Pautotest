#ifndef AUDIO_PROCESS_H
#define AUDIO_PROCESS_H

#pragma once
#include <stdio.h>
#include <string>
#include "Audio_Recog.h"


AUDIO_RECOG_ERROR_TYPE JudgeOnly(const char * sourcefilepath, const char *savefilepath, std::string &result);
bool whether_exist(const char* filepath);




#endif // AUDIO_PROCESS_H

