//#include <iostream>
//#include <fstream>
#include <cmath>
#include <algorithm>

/**
 * @brief check vad by Signal / Noise rate
 */
class SnrVad {
public:
    enum VADSTATE {
        NONE,
        BeginOfSpeech,
        EndOfSpeech
    };

    enum SNDSTATE {
        SILENCE,
        SPEECH
    };

    SnrVad(int samplesPerFrame, int beginSpeechFrame = 3, int endSpeechFrame = 15)
        :kBeginSpeechFrame(beginSpeechFrame)
        ,kEndSpeechFrame(endSpeechFrame)
        ,kSamplesPerFrame(samplesPerFrame)
        ,_noiseLevel(0.0)
        ,_state(SILENCE)
        ,_state_count(0)
    {}

    ~SnrVad() {}

    /**
     * @brief init the environment noise level
     *
     * @param samples
     */
    void initNoise(short* samples, int frames);

    VADSTATE process(short* samples);

    SNDSTATE state() const { return _state; }
    void setState(SNDSTATE state)  { _state = state; }
    int state_count() const { return _state_count; }

private:
    float getRmsdB(float rms);

    float rms(short* samples, int length);

    const int kBeginSpeechFrame; // begin silence samples
    const int kEndSpeechFrame; // end silence samples
    const int kSamplesPerFrame;
//    const int kSnrOfSlienceToSpeech = 5;    // silence -> speech, SN: 5db
//    const int kSnrOfSpeechToSlience = 3;    // speech -> silence, SN: 3db
       const int kSnrOfSlienceToSpeech = 8;    // silence -> speech, SN: 5db
       const int kSnrOfSpeechToSlience = 5;    // speech -> silence, SN: 3db

    float _noiseLevel;
    SNDSTATE _state;
    int _state_count;
};

