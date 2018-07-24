#include <fstream>
#include <memory>
#include <functional>

#include "audio_output_alsa.h"
#include "audio_input_alsa.h"

std::shared_ptr<std::vector<unsigned char>> read_pcm_data(const char* pcmFileName)
{
    std::ifstream pcmFile(pcmFileName, std::ios::binary);
    if (! pcmFile) {
        std::cout << "ERROR: can't open file to play." << std::endl;
        return nullptr;
    }

    pcmFile.seekg(0, std::ios::end);
    auto dataSize = pcmFile.tellg();
    pcmFile.seekg(0);

    auto data = std::make_shared<std::vector<unsigned char>>(dataSize);
    pcmFile.read(reinterpret_cast<char*>(data->data()), dataSize);

    return data;
}

void test_audio_out()
{
    std::cout << "INFO : start audio out ......" << std::endl;

    auto data = read_pcm_data("record.pcm");
    if (! data) {
        return;
    }

    AudioOutputALSA Audio_output;
    Audio_output.Start();
    Audio_output.Send(data);

    // IMPORTANT:
    // wait the playing thread run before call the Stop()!!
    // otherwise, the playing will be stoped immediatelly
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    Audio_output.Stop();
}

class AudioFileWriter
{
public:
    AudioFileWriter(const char* pcmFileName)
        :dataFile_(pcmFileName, std::ios::binary)
    {
    }

    ~AudioFileWriter()
    {
        dataFile_.flush();
    }

    operator bool() const
    {
        return static_cast<bool>(dataFile_);
    }

    void onAudioData(std::shared_ptr<std::vector<unsigned char>> data)
    {
        if (! dataFile_) {
            return;
        }

        dataFile_.write(reinterpret_cast<char*>(data->data()), data->size());
    }

private:
    std::ofstream dataFile_;
};

void test_audio_in()
{
    std::cout << "INFO : start audio in ......" << std::endl;
    AudioFileWriter audioFile("record.pcm");
    if (! audioFile) {
        std::cout << "ERROR: can't open file to record." << std::endl;
        return;
    }

    auto onAudioDataFun = std::bind(&AudioFileWriter::onAudioData, &audioFile, std::placeholders::_1);

    AudioInputALSA audioIn;
    audioIn.AddDataListener(onAudioDataFun);

    audioIn.Start();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    audioIn.Stop();
}

int main()
{
    test_audio_in();

    test_audio_out();

    return 0;
}
