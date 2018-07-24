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


#define MIN(x,y) ((x) < (y) ? (x) : (y))


using namespace std;

//====================判断文件是否存在

bool whether_exist(const char* filepath)
{
	fstream _file;
	_file.open(filepath, ios::in);
	if (!_file)
	{
        printf("%s not exist!", filepath);
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
        printf("ERROR: can't open file to play." );
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

bool VadAudioDataFile::doVadToFile(string outFileName, int timeout)
{
    auto wavFile = std::make_shared<WavFile>();
    if (! wavFile->openW(outFileName, g_configInfo.sampling_rate)) {
        printf("ERROR: can't open file to record.");
        return false;
    }

    auto onAudioDataFun = std::bind(&VadAudioDataFile::onAudioData, this, wavFile, std::placeholders::_1);
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
    wavFile->close();

    return true;
}


void VadAudioDataFile::onAudioData(std::shared_ptr<WavFile> wavFile, std::shared_ptr<std::vector<unsigned char> > data) {
    int startloc = 0;
    int curloc = 0;
    if (m_NeedVadInit) {
        m_svad.initNoise((short*)data->data(), g_configInfo.vadConf.InitFrames);
        m_NeedVadInit = false;
        curloc += g_configInfo.vadConf.SamplesPerFrame * g_configInfo.vadConf.InitFrames * 2;
    }
    while ((curloc + g_configInfo.vadConf.SamplesPerFrame * 2) < data->size()) {
        SnrVad::VADSTATE s = m_svad.process((short*)(data->data() + curloc));
        if (SnrVad::BeginOfSpeech == s) {
            unsigned char space[g_configInfo.vadConf.SamplesPerFrame *g_configInfo.vadConf.SpaceStartFrames * 2] = {0};
            wavFile->write(space, g_configInfo.vadConf.SamplesPerFrame *g_configInfo.vadConf.SpaceStartFrames * 2);
            startloc = curloc - g_configInfo.vadConf.SamplesPerFrame * g_configInfo.vadConf.SpaceStartFrames  * 2;
        } else if (SnrVad::EndOfSpeech == s) {
            std::unique_lock<std::mutex> locker(m_mtx);
            m_needstop = true;
            cv.notify_one();
            return;
        }

        curloc += g_configInfo.vadConf.SamplesPerFrame * 2;
    }

    if (SnrVad::SPEECH == m_svad.state()) {
        if(startloc < 0) {
            wavFile->write(m_buffer + g_configInfo.vadConf.SamplesPerFrame * g_configInfo.vadConf.SpaceStartFrames  * 2 + startloc, -startloc);
        }
        wavFile->write(data->data(), data->size());
    }

    m_time += data->size() / 32;
    if(m_timeout < m_time) {
        printf("VAD'time is out! \n");
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
