#include "testor/Controler.h"
#include "audio_output_alsa.h"
#include "Audio_Recog.h"
#include "audio_process.h"
#include "recog.h"
#include "ConfigAna.h"
#include "LOG.h"

bool test = false;
extern std::string g_sOutPath = "";

int main(int argc, char* argv[])
{
    Controler ctrl;
    ConfigAna conA;
   if (5 > argc) {
        LOG("Usage: vats TestCases.xml testResult.xml outpath configpath \n");
        return 1;
    }

    std::string configPath = argv[4];
    g_sOutPath = argv[3];
    conA.analyzeConfFile(configPath.c_str());

    ctrl.Run(argv[1], argv[2]);

    if (test) {
        VadAudioDataFile vadf;
        AudioOutputALSA aout;
        aout.Start();
        aout.Stop();
        ProcessBeep(g_sOutPath.c_str(), 1);
    }

    return 0;
}
