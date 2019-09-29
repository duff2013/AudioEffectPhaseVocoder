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
#include <Arduino.h>
#include "effect_vocoder.h"

void AudioEffectVocoder::update(void) {
    audio_block_t *block;
    
    block = receiveReadOnly();
    if (!block) return;
    
#if defined(__ARM_ARCH_7EM__)
    switch (state) {
        case 0:
            blocklist[0] = block;
            state = 1;
            break;
        case 1:
            blocklist[1] = block;
            state = 2;
            break;
        case 2:
            blocklist[2] = block;
            state = 3;
            break;
        case 3:
            blocklist[3] = block;
            state = 4;
            break;
        case 4:
            blocklist[4] = block;
            state = 5;
            break;
        case 5:
            blocklist[5] = block;
            state = 6;
            break;
        case 6:
            blocklist[6] = block;
            state = 7;
            break;
        case 7:
            blocklist[7] = block;
            float diff_phase;
            long qpd, index = 0;
            
            for (int k = 0; k < 8; k++) {
                arm_q15_to_float(blocklist[k]->data, FFT_Frame + (k * AUDIO_BLOCK_SAMPLES), AUDIO_BLOCK_SAMPLES);
            }
            
            for (int k = 0; k < FFT_SIZE; k++) {
                FFT_Frame[k] = FFT_Frame[k] * window[k];
            }
            
            arm_cfft_f32(instance, FFT_Frame, 0, 1);
            split_rfft_f32(FFT_Frame, HALF_FFT_SIZE, coefA, coefB, FFT_Split_Frame);
            
            memset(Synth_Magn, 0, HALF_FFT_SIZE * sizeof(float));
            memset(Synth_Freq, 0, HALF_FFT_SIZE * sizeof(float));
            
            for (int k = 0; k < HALF_FFT_SIZE; k++) {
                index = k * pshift;
                if (index < HALF_FFT_SIZE) {
                    float real = FFT_Split_Frame[2 * k];
                    float imag = FFT_Split_Frame[2 * k + 1];
                    
                    float phase = atan2_fast(imag, real);
                    float magn = 2.0f * sqrtf(real * real + imag * imag);
                    
                    float last_phase = Last_Phase[k];
                    Last_Phase[k] = phase;
                    
                    int q = 0.07957747154594767f * (4.0f * phase - 4.0f * last_phase - PI * k);
                    qpd = (q & 1) ^ q;
                    
                    diff_phase = -0.000000397912725f * (433096375.5250785f * qpd - 137858858 * phase + 137858858 * last_phase);
                    
                    Synth_Magn[index] += magn;
                    Synth_Freq[index] = diff_phase * pshift;
                }
            }
            
            for (int k = 0; k < HALF_FFT_SIZE; k++) {
                float magn = Synth_Magn[k];
                diff_phase = Synth_Freq[k];
                
                diff_phase = 0.01823690973512442f * diff_phase;
                
                Phase_Sum[k] += diff_phase;
                float phase = Phase_Sum[k];
                
                FFT_Split_Frame[2 * k]     = magn * arm_cos_f32(phase);
                FFT_Split_Frame[2 * k + 1] = magn * arm_sin_f32(phase);
            }
            
            split_rifft_f32(FFT_Split_Frame, HALF_FFT_SIZE, coefA, coefB, IFFT_Synth_Split_Frame);
            arm_cfft_f32(instance, IFFT_Synth_Split_Frame, 1, 1);
            
            for (int k = 0; k < FFT_SIZE; k++) {
                Synth_Accum[k] += (IFFT_Synth_Split_Frame[k] * FFT_SIZE * window[k]) / (HALF_FFT_SIZE * OVER_SAMPLE);
            }
            
            audio_block_t *synthBlock = allocate();
            arm_float_to_q15(Synth_Accum, synthBlock->data, AUDIO_BLOCK_SAMPLES);
            
            memmove(Synth_Accum,  Synth_Accum + STEP_SIZE, FFT_SIZE * sizeof(float));
            
            transmit(synthBlock);
            release(synthBlock);
            release(blocklist[0]);
            blocklist[0] = blocklist[1];
            blocklist[1] = blocklist[2];
            blocklist[2] = blocklist[3];
            blocklist[3] = blocklist[4];
            blocklist[4] = blocklist[5];
            blocklist[5] = blocklist[6];
            blocklist[6] = blocklist[7];
            state = 7;
            break;
    }
#else
    release(block);
#endif
}
