/* Phase Vocoder for Teensy 3.5+/4
 * Copyright (c) 2019, Colin Duffy
 *
 * Based off Stephan M. Bernsee smbPitchShift.
 * http://blogs.zynaptiq.com/bernsee/repo/smbPitchShift.cpp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef phase_vocoder_h_
#define phase_vocoder_h_

#include "Arduino.h"
#include "AudioStream.h"
#include <arm_math.h>
#include "arm_const_structs.h"

extern "C" {
    extern float atan2_fast(float y, float x);
    extern void split_rfft_f32 (float * pSrc, uint32_t fftLen, const float * pATable, const float * pBTable, float * pDst);
    extern void split_rifft_f32 (float * pSrc, uint32_t fftLen, const float * pATable, const float * pBTable, float * pDst);
    extern const float win1024_f32[];
    extern const float coefA_512_f32[];
    extern const float coefB_512_f32[];
}

#define FFT_SIZE    1024
#define OVER_SAMPLE 8
#define HALF_FFT_SIZE FFT_SIZE / 2

class AudioEffectPhaseVocoder : public AudioStream {
public:
    AudioEffectPhaseVocoder() : AudioStream(1, inputQueueArray),
                           pshift(0),
                           state(0)
    {
        memset(Last_Phase,  0, FFT_SIZE * sizeof(float));
        memset(Phase_Sum,   0, FFT_SIZE * sizeof(float));
        memset(Synth_Accum, 0, (FFT_SIZE+AUDIO_BLOCK_SAMPLES) * sizeof(float));
        instance2 = &arm_cfft_sR_f32_len1024;
        instance  = &arm_cfft_sR_f32_len512;
        window = win1024_f32;
        coefA = coefA_512_f32;
        coefB = coefB_512_f32;
    }

    float setPitchShift(float semitones) {
        pshift = powf(2.0f, semitones / 12.0f);
        return pshift;
    }

    virtual void update(void);
private:
    const float *window;
    const float *coefA;
    const float *coefB;
    audio_block_t outblock;
    const arm_cfft_instance_f32 *instance;
    const arm_cfft_instance_f32 *instance2;
    float pshift;
    uint8_t state;
    const long  STEP_SIZE    = FFT_SIZE / OVER_SAMPLE;
    const float FREQ_PER_BIN = AUDIO_SAMPLE_RATE_EXACT / (float)FFT_SIZE;
    const float EXPECT       = 2.0f * M_PI * (float)STEP_SIZE / (float)FFT_SIZE;
    float Max_Magns             [10];
    float Phase_Sum             [FFT_SIZE];
    float FFT_Frame             [FFT_SIZE];
    float Last_Phase            [FFT_SIZE];
    float Synth_Freq            [FFT_SIZE];
    float Synth_Magn            [FFT_SIZE];
    float Synth_Accum           [FFT_SIZE+AUDIO_BLOCK_SAMPLES];
    float FFT_Split_Frame       [FFT_SIZE * 2];
    float IFFT_Synth_Split_Frame[FFT_SIZE * 2];
    audio_block_t *blocklist[8];
    audio_block_t *inputQueueArray[1];
};

#endif
