#include "ActionPlaying.h"

#include <fstream>
#include <iostream>
#include <thread>
#include "LOG.h"

#include "audio_output_alsa.h"

std::shared_ptr<std::vector<unsigned char> > ActionPlaying::readPcmData(std::string pcmFileName)
{
    std::ifstream pcmFile(pcmFileName, std::ios::binary);
    if (! pcmFile) {
//        LOG("ERROR: can't open file to play.\n");
        return nullptr;
    }

    pcmFile.seekg(0, std::ios::end);
    auto dataSize = pcmFile.tellg();
    pcmFile.seekg(0);

    auto data = std::make_shared<std::vector<unsigned char>>(dataSize);
    pcmFile.read(reinterpret_cast<char*>(data->data()), dataSize);

    return data;
}

void ActionPlaying::play(std::shared_ptr<std::vector<unsigned char>> data)
{
    AudioOutputALSA Audio_output;
    Audio_output.Start();
    Audio_output.Send(data);

    // IMPORTANT:
    // wait the playing thread run before call the Stop()!!
    // otherwise, the playing will be stoped immediatelly
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    Audio_output.Stop();
}

bool ActionPlaying::run(std::string casename, int seq)
{
    auto data = readPcmData(_filePath);

    if (! data) {
        return false;
    }

    play(data);

    return true;
}
