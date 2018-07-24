#include "vad.h"
#include "LOG.h"

void SnrVad::initNoise(short* samples, int frames) {
    _noiseLevel = rms(samples, kSamplesPerFrame * frames);
    _state = SILENCE;
    _state_count = 0;
}

SnrVad::VADSTATE SnrVad::process(short* samples) {
    float signalLevel = rms(samples, kSamplesPerFrame);
    float snr = getRmsdB(signalLevel);
//    LOG("snr:%f _noiseLevel:%f signalLevel:%f \n", snr, _noiseLevel, signalLevel);

    switch (_state) {
    case SILENCE:
        if (snr > kSnrOfSlienceToSpeech) {
            _state_count++;
            if (_state_count >= kBeginSpeechFrame) {
                _state = SPEECH;
                _state_count = 0;
                return BeginOfSpeech;
            }
        }
        else {
            _state_count = 0;
        }
        break;
    case SPEECH:
        if (snr < kSnrOfSpeechToSlience) {
            _state_count++;
            if (_state_count >= kEndSpeechFrame) {
                _state = SILENCE;
                _state_count = 0;
                return EndOfSpeech;
            }
        }
        else {
            _state_count = 0;
        }
        break;
    default:
        break;
    }

    return NONE;
}

float SnrVad::getRmsdB(float rms) {
    if (_noiseLevel < rms) {
        _noiseLevel = (0.999f * _noiseLevel) + (0.001f * rms);
    } else {
        _noiseLevel = (0.95f * _noiseLevel) + (0.05f * rms);
    }
    if (((double) _noiseLevel) <= 0.0 || ((double) (rms / _noiseLevel)) <= 1.0E-6) {
        return -120.0f;
    }
    float originalDb = 10.0f * ((float) std::log10((double) (rms / _noiseLevel)));
    // Clamp db to [0, 60]
    return std::min(std::max(0.0f, originalDb), 60.0f);
}

float SnrVad::rms(short* samples, int length) {
    float sum = 0.0f;
    if (0 >= length) {
        return 0.0;
    }

    for (short* ps = samples; ps < samples + length; ++ps) {
        sum += (float)((*ps) * (*ps));
    }

    return (float) std::sqrt((double) (sum / ((float) length)));
}
