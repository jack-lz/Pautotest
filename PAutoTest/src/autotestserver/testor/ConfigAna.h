#ifndef CONFIGANA_H
#define CONFIGANA_H


#include <map>
#include <string>
#include <vector>


//startVR
struct BeepType1 {
    int startVR_type1 = 1;
    float freq = 1800;
    float beep1_voice_len = 0.1;
};

//returnVR
struct BeepType2 {
    int returnVR_type2 = 2;
    float freq= 1800;
    float beep2_voice_len = 0.50;
};

//endVR
struct BeepType3 {
    int endVR_type3 = 3;
    float freq = 1800;
    float beep3_voice_len = 0.70;
};

struct VadConfig {
    int BeginSpeechsFrames = 1;
    int EndSpeechsFrames = 10;
    int InitFrames = 1;
    int SamplesPerFrame = 480;
    int SpaceStartFrames = 4;
};

struct Config17 {
    BeepType1 beep1;
    BeepType2 beep2;
    BeepType3 beep3;
};

struct Config659b {
    int beepLimit = 250;
    float beep_f_min = 1937;
    float beep_f_max = 2032;
    float beep_voice_len = 0.05;
    float beep_voice_range = 0.015;
};

struct ConfigInfo {
    std::string version = "version659b";//车型
    int times = 3;//重复运行次数(错误情况下)
    unsigned int sampling_rate = 16000;
    float splitRange = 0.2;
    float beep_f_range = 64.0;
    VadConfig vadConf;

    Config17 conf17;

    Config659b conf659b;
};


extern ConfigInfo g_configInfo;

class ConfigAna
{

public:
    ConfigAna(){}

    virtual ~ConfigAna() {}

    void analyzeConfFile(const char* filepath);

    void split(const std::string& str, const std::string& separator, std::vector<std::string>& dest);

    std::string& trim(std::string &s);
};

#endif /* CONFIGANA_H */
