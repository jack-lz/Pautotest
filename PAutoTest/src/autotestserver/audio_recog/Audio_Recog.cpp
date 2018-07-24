#include <stdlib.h>
#include "Audio_Recog.h"
#include "recog.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include <algorithm>
#include "audio_process.h"
#include "audio_output_alsa.h"
#include "audio_input_alsa.h"
#include <string.h>
#include "ConfigAna.h"
#include "WavFile.h"
#include "LOG.h"


#define MIN(x,y) ((x) < (y) ? (x) : (y))


using namespace std;

//====================判断文件是否存在

bool whether_exist(const char* filepath)
{
    fstream _file;
    _file.open(filepath, ios::in);
    if (!_file)
    {
        LOG("%s not exist!\n", filepath);
        _file.close();
        return false;
    }
    _file.close();
    return true;
}

std::shared_ptr<std::vector<unsigned char>> read_pcm_data(const char* pcmFileName)
{
    std::ifstream pcmFile(pcmFileName, std::ios::binary);
    if (! pcmFile) {
        LOG("ERROR: can't open file to play.\n" );
        return nullptr;
    }

    pcmFile.seekg(0, std::ios::end);
    auto dataSize = pcmFile.tellg();
    pcmFile.seekg(0);

    auto data = std::make_shared<std::vector<unsigned char>>(dataSize);
    pcmFile.read(reinterpret_cast<char*>(data->data()), dataSize);

    return data;
}

VadAudioDataFile::VadAudioDataFile()
                :m_state(0)
                ,m_activeCount(0)
                ,m_needstop(false)
                ,m_timeout(10000)
                ,m_time(0)
                ,m_tmp(0)
                ,m_minsize(0)
                ,m_NeedVadInit(true)
                ,m_svad(g_configInfo.vadConf.SamplesPerFrame, g_configInfo.vadConf.BeginSpeechsFrames, g_configInfo.vadConf.EndSpeechsFrames)
                ,m_buffer_pos(0)
{
    m_buffer = (char*)malloc(sizeof(char) * g_configInfo.vadConf.SamplesPerFrame * g_configInfo.vadConf.SpaceStartFrames  * 2);
}

VadAudioDataFile::~VadAudioDataFile()
{
    if(NULL != m_buffer) {
        free(m_buffer);
        m_buffer = NULL;
    }
}

bool VadAudioDataFile::doVadToFile(string outFileName, int timeout, int filelenth)
{
    m_minsize = 0;
    auto wavFile = std::make_shared<WavFile>();
    if (! wavFile->openW(outFileName, g_configInfo.sampling_rate)) {
        LOG("ERROR: can't open file to record.\n");
        return false;
    }

    auto onAudioDataFun = std::bind(&VadAudioDataFile::onAudioData, this, wavFile, std::placeholders::_1, filelenth);
    AudioInputALSA audioIn;
    audioIn.AddDataListener(onAudioDataFun);
    m_timeout = timeout;
    audioIn.Start();
    {
        std::unique_lock<std::mutex> locker(m_mtx);
        while(! m_needstop) {
            cv.wait(locker);
        }
    }
    audioIn.Stop();
    m_needstop = false;
    wavFile->close();

    return true;
}


void VadAudioDataFile::onAudioData(std::shared_ptr<WavFile> wavFile, std::shared_ptr<std::vector<unsigned char> > data, int filelenth) {
    int startloc = 0;
    int curloc = 0;
    if (m_NeedVadInit) {
        m_svad.initNoise((short*)data->data(), g_configInfo.vadConf.InitFrames);
        m_NeedVadInit = false;
        curloc += g_configInfo.vadConf.SamplesPerFrame * g_configInfo.vadConf.InitFrames * 2;
    }
    int temp = m_minsize;
    while ((curloc + g_configInfo.vadConf.SamplesPerFrame * 2) < data->size()) {
        SnrVad::VADSTATE s = m_svad.process((short*)(data->data() + curloc));
        if (SnrVad::BeginOfSpeech == s) {
            printf("===================write data !\n");
            unsigned char space[g_configInfo.vadConf.SamplesPerFrame *g_configInfo.vadConf.SpaceStartFrames * 2] = {0};
            m_minsize += g_configInfo.vadConf.SamplesPerFrame *g_configInfo.vadConf.SpaceStartFrames * 2;
            wavFile->write(space, g_configInfo.vadConf.SamplesPerFrame *g_configInfo.vadConf.SpaceStartFrames * 2);
            if(temp==0)
            {
               startloc = curloc - g_configInfo.vadConf.SamplesPerFrame * g_configInfo.vadConf.SpaceStartFrames  * 2;
            }

        } else if (SnrVad::EndOfSpeech == s) {
            if (temp < filelenth) {
                m_svad.setState(SnrVad::SPEECH);
                curloc += g_configInfo.vadConf.SamplesPerFrame * 2;
                temp += g_configInfo.vadConf.SamplesPerFrame * 2;
                continue;
            }

            std::unique_lock<std::mutex> locker(m_mtx);
            m_needstop = true;
            cv.notify_one();
            return;
        }

        curloc += g_configInfo.vadConf.SamplesPerFrame * 2;

    }

    if (SnrVad::SPEECH == m_svad.state()) {
        if(startloc < 0) {
            int tmp = wavFile->write(m_buffer + g_configInfo.vadConf.SamplesPerFrame * g_configInfo.vadConf.SpaceStartFrames  * 2 + startloc, -startloc);
            m_minsize += tmp;
        }
        wavFile->write(data->data(), data->size());
        m_minsize += data->size();
    }


//    m_tmp += int(data->size() * 1000 / (g_configInfo.sampling_rate*2));

    m_time += data->size() / 32 ;

    if(m_timeout < m_time) {
        LOG("VAD'time is out! \n");
        std::unique_lock<std::mutex> locker(m_mtx);
        m_needstop = true;
        cv.notify_one();
        return;
    }

    if (SnrVad::SILENCE == m_svad.state()) {
        int save_size = MIN(data->size(), g_configInfo.vadConf.SamplesPerFrame * g_configInfo.vadConf.SpaceStartFrames  * 2);
        int curpos = MIN(m_buffer_pos, g_configInfo.vadConf.SamplesPerFrame * g_configInfo.vadConf.SpaceStartFrames  * 2 - save_size);
        if (curpos < m_buffer_pos) {
            memcpy(m_buffer, m_buffer + m_buffer_pos - curpos, curpos);
        }
        memcpy(m_buffer + curpos,data->data() + (data->size() - save_size), save_size);
        m_buffer_pos = curpos + save_size;
    }

}

