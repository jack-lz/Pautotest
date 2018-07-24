#include "fingerprint.h"
#include <algorithm>
#include<fstream>
#include <iostream>
#include <set>
#include <vector>
#include "../testor/ConfigAna.h"
#include "LOG.h"

using namespace std;

//#define BEEPLEN_01 0.05
//#define BEEPLEN_02 0.25
//#define BEEPLEN_03 3.00
//#define BEEPLEN_04 0.10
//#define BEEPLEN_05 0.50
//#define BEEPLEN_RANGE 1
//#define BEEP_F 3000

//#define BEEP_I 250
//#define BEEP_F_MIN 1937
//#define BEEP_F_MAX 2032
//#define BEEP_F_RANGE 64
//#define BEEPLEN_VOICE 0.05
//#define BEEPLEN_VOICE_RANGE 0.015

//const std::map<int, float>RecFreq;

TExtractFeature::TExtractFeature(){
    frame_num = 0;
    fftw_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* FRAME_SIZE);
    fftw_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* FRAME_SIZE);
    fftw_p = fftw_plan_dft_1d(FRAME_SIZE, fftw_in, fftw_out, FFTW_FORWARD, FFTW_ESTIMATE);

    MALLOC(threshold, FRAME_HEIGHT, float);
    MALLOC(peak, FRAME_HEIGHT, float);

    MALLOC(Gauss_base, FRAME_HEIGHT*2+1, float);
    Gauss = Gauss_base + FRAME_HEIGHT;
    for (int i=-FRAME_HEIGHT; i<=FRAME_HEIGHT; i++)
        Gauss[i] = (float)exp(-0.5*i*i/Gauss_Var/Gauss_Var);
    MALLOC(Hamming, FRAME_SIZE, float);
    for (int i = 0; i<FRAME_SIZE; i++)
        Hamming[i] = (float)(0.54 - 0.46*cos(2 * PI*i / (FRAME_SIZE - 1)));
    MALLOC(search, MAX_FEATURE_PER_FRAME, int);
}

int TExtractFeature::AddSearch(int frame, int freq){
    int min_place = 0;	//使spectrum[frame][search[k]]最小的k
    int flag = 0;		//可插入标志
    for (int k=0; k<MAX_FEATURE_PER_FRAME; k++){
        if (search[k]==0){	//search[k]为空,可将新值填入
            flag = 1;
            min_place = k;
            break;
        }
        if (spectrum[frame][search[k]]<spectrum[frame][freq])	//可以插入
            flag = 1;
        if (spectrum[frame][search[k]]<spectrum[frame][search[min_place]])
            min_place = k;
    }
    if (flag==0) return 0;
    search[min_place] = freq;
    return 1;
}

void TExtractFeature::Init(){
    MALLOC2D(spectrum, frame_num, FRAME_HEIGHT, float);
    MALLOC2D(feature, frame_num, FRAME_HEIGHT, int);
    for (int i=0; i<frame_num; i++)
        memset(feature[i], 0, sizeof(int)*FRAME_HEIGHT);
}

int TExtractFeature::NotExtremum(int j, float Arr[]){
    if (j==0 && Arr[j]<=Arr[j+1])
        return 1;
    if (j==FRAME_HEIGHT-1 && Arr[j]<=Arr[j-1])
        return 1;
    if (j>0 && j<FRAME_HEIGHT-1 && (Arr[j]<=Arr[j+1] || Arr[j]<=Arr[j-1]))
        return 1;
    return 0;
}

int TExtractFeature::ExtractPair(const char *filename, THash &hash, int offset, float decay, int Id, int mode){
    decay_factor = decay;
    Wav2Spectrum(filename, offset);

    if (10 > frame_num) {
        LOG("frame_num is too short:%d \n", frame_num);
        return -1;
    }

    Spectrum2Feature();
    if (mode == 0)
        Feature2PairInsert(hash, Id);
    else
        Feature2PairSearch(hash);

    return frame_num;
}

string TExtractFeature::BeepJudge(const char *filename) {
    Json::Value res;
    res["beepFile"] = filename;
    if (0 >= Wav2Spectrum(filename, 0)) {
         res["Type"] = "error";
         return res.toStyledString();
    }

    int minF = FRAME_HEIGHT;
    int maxF = 0;
    int minT = frame_num;
    int maxT = 0;
    for (int i = 0; i < frame_num; ++i) {
        for (int j = 0; j < FRAME_HEIGHT; ++j) {
            if (g_configInfo.conf659b.beepLimit <= spectrum[i][j]) {
                if (minF > j) {
                    minF = j;
                } else if (maxF < j) {
                    maxF = j;
                }

                if (minT > i) {
                    minT = i;
                } else if (maxT < i) {
                    maxT = i;
                }
            }
        }
    }

    float iminF = (float)minF/(float)FRAME_HEIGHT* ((float)samples/2);
    float imaxF = (float)maxF/(float)FRAME_HEIGHT*((float)samples/2);

    float fminT = (float)(minT*(float)FRAME_HOP / float(samples));
    float fmaxT = (float)(maxT*(float)FRAME_HOP / float(samples));
    float fLen = fmaxT - fminT;

    if (((g_configInfo.conf659b.beep_f_min - g_configInfo.beep_f_range) <= iminF)
            && ((g_configInfo.conf659b.beep_f_min + g_configInfo.beep_f_range) >= iminF)
            && ((g_configInfo.conf659b.beep_f_max - g_configInfo.beep_f_range) <= imaxF)
            && ((g_configInfo.conf659b.beep_f_max + g_configInfo.beep_f_range) >= imaxF)
            && (fLen <= (g_configInfo.conf659b.beep_voice_len + g_configInfo.beep_f_range))
            && (fLen >= (g_configInfo.conf659b.beep_voice_len - g_configInfo.beep_f_range))) {
        res["Type"] = "StartVR";
        res["matchResult"] = true;
    } else {
        res["Type"] = "other";
        res["matchResult"] = false;
    }
    return res.toStyledString();
}

string TExtractFeature::BeepJudge17(const char *filename, int beepType) {
    Json::Value res;

    res["beepFile"] = filename;
    if (0 >= Wav2Spectrum(filename, 0)) {
         res["Type"] = "error";
         return res.toStyledString();
    }

    float peak_limit = 0;
    int peak_freq = 0;
    for (int i = 0; i < frame_num; ++i) {
        for (int j = 0; j < FRAME_HEIGHT; ++j) {
            if (peak_limit <= spectrum[i][j]) {
                peak_limit = spectrum[i][j];
                peak_freq = j;
            }
        }
    }

    float iFreq = (float) peak_freq/(float)FRAME_HEIGHT* ((float)samples/2);
    if (g_configInfo.conf17.beep1.startVR_type1 == beepType) {
        res["Type"] = "startVR_type1";
    } else if (g_configInfo.conf17.beep2.returnVR_type2 == beepType) {
        res["Type"] = "returnVR_type2";
    } else if (g_configInfo.conf17.beep3.endVR_type3 == beepType) {
        res["Type"] = "endVR_type3";
    }

    if((iFreq <= g_configInfo.conf17.beep1.freq + g_configInfo.beep_f_range) && (iFreq >= g_configInfo.conf17.beep1.freq - g_configInfo.beep_f_range))
    {
        res["matchResult"] = true;
    }else if((iFreq <= g_configInfo.conf17.beep2.freq + g_configInfo.beep_f_range) && (iFreq >= g_configInfo.conf17.beep2.freq - g_configInfo.beep_f_range))
    {
        res["matchResult"] = true;
    }else if((iFreq <= g_configInfo.conf17.beep3.freq + g_configInfo.beep_f_range) && (iFreq >= g_configInfo.conf17.beep3.freq - g_configInfo.beep_f_range))
    {
        res["matchResult"] = true;
    }
    else
    {
        res["matchResult"] = false;
    }

   return res.toStyledString();
}

int TExtractFeature::Wav2Spectrum(const char *filename, int offset){
    TWav wav(filename);
//    int block_num = wav.wav_info.size2/wav.wav_info.block_align;
    int block_num = wav.wav_info.data_size/wav.wav_info.format.block_align;
    if (0 >= block_num-offset-FRAME_SIZE) {
        return block_num-offset-FRAME_SIZE;
    }

    frame_num = (block_num-offset-FRAME_SIZE)/FRAME_HOP+2;
//    samples = wav.wav_info.sample_per_sec;
    samples = wav.wav_info.format.samples_per_sec;

    Init();

//    fseek(wav.fp, offset*wav.wav_info.block_align, SEEK_CUR);
    fseek(wav.wav_info.fp, offset*wav.wav_info.format.block_align, SEEK_CUR);

    ofstream ofile("/home/toby/Desktop/17cy/beep2_sta.csv");
    string split = " ";
    ofile << "i" << split << "j" << split<<"spectrum"<<split<<"time"<<endl;
    for (int i=0; i<frame_num; i++){
        for (int j=0; j<FRAME_SIZE; j++){
            fftw_in[j][0] = wav.GetSample() * Hamming[j];
            fftw_in[j][1] = 0;
        }

//		fseek(wav.fp, (FRAME_HOP-FRAME_SIZE)*wav.wav_info.block_align, SEEK_CUR);
        fseek(wav.wav_info.fp, (FRAME_HOP-FRAME_SIZE)*wav.wav_info.format.block_align, SEEK_CUR);
        fftw_execute(fftw_p);

        for (int j=0; j<FRAME_HEIGHT; j++){
            if (j<LOW_FREQ)
                spectrum[i][j] = 0;
            else
                spectrum[i][j] = (float)sqrt(fftw_out[j][0]*fftw_out[j][0]+fftw_out[j][1]*fftw_out[j][1])/FRAME_SIZE;
         ofile << i << split << j << split<<spectrum[i][j]<<split<<(float)(i*(float)FRAME_HOP / float(samples))<<endl;
        }
    }

    ofile.flush();
    ofile.close();

    return frame_num;
}

int TExtractFeature::Spectrum2Feature(){
    memset(peak, 0, sizeof(float)*FRAME_HEIGHT);
    memset(threshold, 0, sizeof(float)*FRAME_HEIGHT);

    for (int i=0; i<10; i++)
    for (int j=0; j<FRAME_HEIGHT; j++){
        int a = spectrum[i][j];
        int b = peak[j];
        if (spectrum[i][j]>peak[j])
            peak[j] = spectrum[i][j];
    }
    for (int j=0; j<FRAME_HEIGHT; j++){
        if (NotExtremum(j, peak))
            continue;
        for (int k=0; k<FRAME_HEIGHT; k++)
            threshold[k] = (float)max3(threshold[k], peak[j]*Gauss[k-j], MIN_THRESHOLD);
    }

    for (int i=10; i<frame_num; i++){
        memset(search, 0, MAX_FEATURE_PER_FRAME*sizeof(int));
        for (int j=0; j<FRAME_HEIGHT; j++){
            if (spectrum[i][j]<=threshold[j])
                continue;
            if (NotExtremum(j, spectrum[i]))
                continue;
            AddSearch(i, j);
        }
        //记录特征点,更新threshold[]
        for (int j=0; j<MAX_FEATURE_PER_FRAME; j++){
            if (search[j] == 0)
                continue;
            feature[i][search[j]] = 1;
            for (int k=0; k<FRAME_HEIGHT; k++)
                threshold[k] = (float)max3(threshold[k], Gauss[k-search[j]]*spectrum[i][search[j]], MIN_THRESHOLD);
        }
        //使threshold[]衰减
        for (int j=0; j<FRAME_HEIGHT; j++)
            threshold[j] *= decay_factor;
    }

    //Init threshold[]
    memset(threshold, 0, sizeof(float)*FRAME_HEIGHT);
    for (int j=0; j<FRAME_HEIGHT; j++){
        if (NotExtremum(j, spectrum[frame_num-1]))
            continue;
        for (int k=0; k<FRAME_HEIGHT; k++)
            threshold[k] = max2(threshold[k], spectrum[frame_num-1][j]*Gauss[k-j]);
    }
    //decaying threshold法,逆向扫描,减少特征点
    for (int i=frame_num-1; i>=0; i--){
        for (int j=0; j<FRAME_HEIGHT; j++)
        if (feature[i][j]){
            if (spectrum[i][j]<threshold[j]){
                feature[i][j] = 0;
                continue;
            }
            for (int k=0; k<FRAME_HEIGHT; k++)
                threshold[k] = max2(threshold[k], spectrum[i][j]*Gauss[k-j]);
        }
        //使threshold[]衰减
        for (int j=0; j<FRAME_HEIGHT; j++)
            threshold[j] *= decay_factor;
    }
    return 0;
}

int TExtractFeature::Feature2PairInsert(THash &hash, int id){
    int pi, pj;
    int count;
    int flag;
    // [> int index = 0; <]
    for (int i = 0; i<frame_num; i++)
    for (int j = 0; j<FRAME_HEIGHT; j++){
        flag = 1;
        if (feature[i][j] == 0)
            continue;
        count = 0;
        for (pi = i + PAIR_OFFSET; flag && pi<i + PAIR_DX && pi<frame_num; pi++)
        for (pj = max2(j - PAIR_DY, 0); flag && pj<j + PAIR_DY && pj<FRAME_HEIGHT; pj++){
            if (feature[pi][pj] == 0)
                continue;
            count++;
            hash.InsertHash(j, pj-j + f2_f1_max, pi-i, id, i);
            if (count >= MAX_PAIR_PER_FEATURE) flag = 0;
        }
    }
    return 0;
}

int TExtractFeature::Feature2PairSearch(THash &hash){
    int pi, pj;
    int count;
    int flag;
    /* int index = 0; */
    for (int i = 0; i<frame_num; i++)
    for (int j = 0; j<FRAME_HEIGHT; j++){
        flag = 1;
        if (feature[i][j] == 0)
            continue;
        count = 0;
        for (pi = i + PAIR_OFFSET; flag && pi<i + PAIR_DX && pi<frame_num; pi++)
        for (pj = max2(j - PAIR_DY, 0); flag && pj<j + PAIR_DY && pj<FRAME_HEIGHT; pj++){
            if (feature[pi][pj] == 0)
                continue;
            count++;
            hash.Vote(j, pj-j+f2_f1_max, pi-i, i);
        }
    }
    return 0;
}

TExtractFeature::~TExtractFeature(){
    fftw_destroy_plan(fftw_p);
    fftw_free(fftw_in);
    fftw_free(fftw_out);

    FREE(Gauss_base);
    FREE(Hamming);
    FREE(threshold);
    FREE(peak);
    FREE(search);
}

void TExtractFeature::UnInit(){
    if (0 < frame_num) {
        FREE2D(spectrum, frame_num);
        FREE2D(feature, frame_num);
        frame_num = 0;
    }
}
