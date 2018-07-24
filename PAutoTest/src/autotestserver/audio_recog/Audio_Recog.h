#ifndef  AUDIO_RECOG_H
#define  AUDIO_RECOG_H
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>
#include <condition_variable>
// #include "RecordButton_Define.h"
#include "WavFile.h"
#include "vad.h"

typedef enum AUDIO_RECOG_ERROR_TYPE {
    AUDIORECORD_ERROR_TYPE_NO_ERROR,
    AUDIORECORD_ERROR_TYPE_CREATE,
    AUDIORECORD_ERROR_TYPE_START,
    AUDIORECORD_ERROR_TYPE_END,
    AUDIORECORD_ERROR_TYPE_DELETE,
    AUDIORECORD_ERROR_TYPE_PLAY,
    AUDIORECORD_ERROR_TYPE_BEEPJUDGE,
    AUDIORECORD_ERROR_TYPE_FILE_NOTEXIST,
} AUDIO_RECOG_ERROR_TYPE;

AUDIO_RECOG_ERROR_TYPE RecordAndJudgeBeep(void *hinst, char * savebeep, std::string result);

AUDIO_RECOG_ERROR_TYPE RecordAndJudgePromote(void *hinst, char * sourcefile, char* savefile, std::string result);

class VadAudioDataFile
{
public:
    VadAudioDataFile();
    ~VadAudioDataFile();

    bool doVadToFile(std::string outFileName, int timeout, int filelenth = 0);

    bool m_needstop;

private:
    void onAudioData(std::shared_ptr<WavFile> wavFile, std::shared_ptr<std::vector<unsigned char>> data, int filelenth = 0);

    std::mutex m_mtx;
    std::condition_variable cv;
    int m_timeout;
    int m_time;
    int m_tmp;
    int m_minsize;

    int m_state;
    int m_activeCount;
    bool m_NeedVadInit;
    SnrVad m_svad;
    char* m_buffer;
    int m_buffer_pos;
};

#endif // AUDIO_RECOG_H
