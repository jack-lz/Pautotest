#include "ConfigAna.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include "LOG.h"

ConfigInfo g_configInfo;


void ConfigAna::analyzeConfFile(const char *filepath)
{
    std::ifstream conf;
    conf.open(filepath);
    if (!conf) {
        LOG("config file %s failed\n",filepath);
    };

    std::vector<std::string> confVec;

    std::map<std::string,std::string> confMap;
    std::string node;
    std::string key;
    std::string value;
    while(getline(conf,node))
    {
        split(node,":",confVec);
        key = trim(confVec[0]);
        value = trim(confVec[1]);
        confMap.insert(std::make_pair(key,value));
    }
    g_configInfo.version = confMap["version"];
    g_configInfo.times = std::atoi(confMap["extrTimes"].c_str());
    g_configInfo.beep_f_range = std::atof(confMap["beep_f_range"].c_str());
    g_configInfo.sampling_rate = std::atoi(confMap["sampRate"].c_str());
    g_configInfo.vadConf.BeginSpeechsFrames = std::atoi(confMap["vad_beginSpeechsFrames"].c_str());
    g_configInfo.vadConf.EndSpeechsFrames = std::atoi(confMap["vad_EndSpeechsFrames"].c_str());
    g_configInfo.vadConf.InitFrames = std::atoi(confMap["vad_InitFrames"].c_str());
    g_configInfo.vadConf.SamplesPerFrame = std::atoi(confMap["vad_SamplesPerFrame"].c_str());
    g_configInfo.vadConf.SpaceStartFrames = std::atoi(confMap["vad_SpaceStartFrames"].c_str());


    if (0 == strcmp("version17",g_configInfo.version.c_str())) {
        g_configInfo.conf17.beep1.startVR_type1 = std::atoi(confMap["startVR_type1"].c_str());
        g_configInfo.conf17.beep1.freq = std::atof(confMap["beep1_freq"].c_str());
        g_configInfo.conf17.beep1.beep1_voice_len = std::atof(confMap["beep2_voice_len"].c_str());
        g_configInfo.conf17.beep2.returnVR_type2 = std::atoi(confMap["returnVR_type2"].c_str());
        g_configInfo.conf17.beep2.freq = std::atof(confMap["beep2_freq"].c_str());
        g_configInfo.conf17.beep2.beep2_voice_len = std::atof(confMap["beep2_voice_len"].c_str());
        g_configInfo.conf17.beep3.endVR_type3 = std::atoi(confMap["endVR_type3"].c_str());
        g_configInfo.conf17.beep3.freq = std::atof(confMap["beep3_freq"].c_str());
        g_configInfo.conf17.beep3.beep3_voice_len = std::atof(confMap["beep3_voice_len"].c_str());
    } else if (0 == strcmp("version659b",g_configInfo.version.c_str())) {
        g_configInfo.conf659b.beepLimit = std::atoi(confMap["beep_limit"].c_str());
        g_configInfo.conf659b.beep_f_min = std::atof(confMap["beep_f_min"].c_str());
        g_configInfo.conf659b.beep_f_max = std::atof(confMap["beep_f_max"].c_str());
        g_configInfo.conf659b.beep_voice_len = std::atof(confMap["beep_voice_len"].c_str());
        g_configInfo.conf659b.beep_voice_range = std::atof(confMap["beep_voice_range"].c_str());

    }
}

void ConfigAna::split(const std::string& str, const std::string& separator, std::vector<std::string>& dest){
    std::string substring;
    std::string::size_type start = 0, index;
    dest.clear();
    index = str.find_first_of(separator,start);
    do
    {
        if (index != std::string::npos)
        {
            substring = str.substr(start,index-start);
            dest.push_back(substring);
            start =index+separator.size();
            index = str.find(separator,start);
            if (start == std::string::npos) break;
        }
    }while(index != std::string::npos);

    //the last part
    substring = str.substr(start);
    dest.push_back(substring);
}

std::string& ConfigAna::trim(std::string &s)
{
    if (s.empty())
    {
        return s;
    }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}
