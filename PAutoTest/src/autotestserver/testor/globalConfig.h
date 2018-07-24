#include <string>

struct BeepType1 {
    int beepLimit = 500;
    float freq20 = 625;
    float freq70 = 2187.5;
    float freq71 = 2218.75;
};

struct BeepType2 {
    int beepLimit = 500;
    float freq20 = 625;
    float freq70 = 2187.5;
    float freq71 = 2218.75;
};

struct BeepType3 {
    int beepLimit = 500;
    float freq20 = 625;
    float freq70 = 2187.5;
    float freq71 = 2218.75;
};

struct VadConfig {
    int beginSpeechsFrames = 1;
    int endSpeechsFrames = 10;
    int initFrames = 1;
    int sampSpereFrame = 480;
    int spaceSrartFrames = 4;
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
    float beep_f_range = 64;
    float beep_voice_len = 0.05;
    float beep_voice_range = 0.015;
};

struct ConfigInfo {
    std::string version = "version659b";//车型
    int times = 3;//重复运行次数(错误情况下)
    int sampling_rate = 16000;

    VadConfig vadConf;

    Config17 conf17;

    Config659b conf659b;
};
