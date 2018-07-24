#ifndef RECORDBUTTON_H_INCLUDED
#define RECORDBUTTON_H_INCLUDED
#pragma once
#include "WavFile.h"
#include <thread>
#include <string>

#define AR_LOG_FILE_MAX_SIZE 1024*1024*20

extern std::string timestring();
extern std::string Get_localtime();
extern std::string g_sOutPath;

#define AR_LOG_FILE(fmt,...){\
                            printf("%s \n", ##__VA_ARGS__);\
                            std::string out_log = g_sOutPath + "/aclog.log";\
                            FILE *pFile = fopen(out_log.c_str(), "a+");\
                            fseek(pFile, 0, SEEK_END);\
                            int length = ftell(pFile);\
                            if (length >AR_LOG_FILE_MAX_SIZE) {\
                                fclose(pFile);\
                                system("rm out_log.c_str()");\
                                fopen(out_log.c_str(), "a+");\
                                                                      }\
                            fprintf(pFile, "time %s  %s(%d)-%s: %s \n", Get_localtime().c_str(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
                            fclose(pFile);\
                            }

typedef struct AUDIO_Recog {
	void *UsrData;
	char *SourceFilePath;
	char *saveFilePath;
	WavFile * audioin;
	std::thread* r_j_th;
    int fd;

} AUDIO_Recog;


#endif
