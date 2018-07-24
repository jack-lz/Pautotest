#include "hash.h"
#include "fingerprint.h"
#include "recog.h"
#include <json/json.h>
#include "../testor/ConfigAna.h"

using namespace std;
#define MATCH_PER 0.35

string ProcessSelelct(const char *szMusicPath, const char *szAudioPath){
    Json::Value res;

    const char* File1 = szMusicPath;
    const char* File2 = szAudioPath;

    TWav wav1(szMusicPath);
    TWav wav2(szAudioPath);

    if (wav1.wav_info.data_size < wav2.wav_info.data_size) {
        File1 = szAudioPath;
        File2 = szMusicPath;
    }

    res["File1"] = File1;
    res["File2"] = File2;
    if ((NULL == File1) || (NULL == File2)) {
		
        res["result_type"] = AUDIORECOGNITION_ERROR_TYPE_NULLPOINT;
        return res.toStyledString();
	}

	THash myhash;
	TExtractFeature fingerprint;

	myhash.BuildInit();
    myhash.AddSongList(File1);

    int frame_num = 0;
    frame_num = fingerprint.ExtractPair(File1, myhash, 0, (float)0.98, 0, 0);
	fingerprint.UnInit();
    float fFile1Time = (float)(frame_num * (float)FRAME_HOP / float(fingerprint.samples));
    res["File1Time"] = fFile1Time;

	myhash.ReBuildInit();
	myhash.VoteInit();

    frame_num = fingerprint.ExtractPair(File2, myhash, 0, (float)0.98, 0, 1);
	fingerprint.UnInit();
    frame_num= fingerprint.ExtractPair(File2, myhash, 8, (float)0.98, 0, 1);
    fingerprint.UnInit();
    frame_num = fingerprint.ExtractPair(File2, myhash, 16, (float)0.98, 0, 1);
    fingerprint.UnInit();
    frame_num = fingerprint.ExtractPair(File2, myhash, 24, (float)0.98, 0, 1);
    fingerprint.UnInit();
    float fFile2Time = (float)(frame_num * (float)FRAME_HOP / float(fingerprint.samples));
    res["File2Time"] = fFile2Time;
	size_t offset;
    size_t max = myhash.VoteResult(offset);
//    res["match"] = Json::Value(static_cast<uint>(max));
    res["keyNum"] = Json::Value(static_cast<uint>(myhash.data_num));
    res["match"] = Json::Value(static_cast<uint>(max));
    if(0 == max) {
        res["matchPer"] = Json::Value(static_cast<uint>(0));
        res["matchResult"] = false;
    } else {
        float fMatchPer = (float)max/(float)myhash.data_num;
        res["matchPer"] = fMatchPer;
        res["matchLevel"] = MATCH_PER * (min(fFile1Time, fFile2Time) / fFile1Time);
        if((MATCH_PER * (min(fFile1Time, fFile2Time) / fFile1Time)) <= fMatchPer) {
            res["matchResult"] = true;
        } else {
            res["matchResult"] = false;
        }
    }
//    res["startTime"] = (float)(offset*(float)FRAME_HOP / float(fingerprint.samples));
//    res["endTime"] = res["startTime"].asDouble() + res["recordTime"].asDouble();
	myhash.BuildUnInit();
    res["resultType"] = AUDIORECOGNITION_ERROR_TYPE_NO_ERROR;
    return res.toStyledString();
}

string ProcessBeep(const char *szBeepFilePath, int beepType) {
    TExtractFeature fingerprint;
    string result = "";
//    const char* beepPath = "/home/wangjiawei/autotest/自动化测试_VR/201805171019/recod_beep0.wav";
    if (0 == strcmp("version659b",g_configInfo.version.c_str())) {
        result = fingerprint.BeepJudge(szBeepFilePath);
    }
    else if (0 == strcmp("version17",g_configInfo.version.c_str())) {
        result = fingerprint.BeepJudge17(szBeepFilePath,beepType);
    }
//    string result = fingerprint.BeepJudge(szBeepFilePath);
//    string result = fingerprint.BeepJudge17(szBeepFilePath);
    fingerprint.UnInit();
    return result;
}
