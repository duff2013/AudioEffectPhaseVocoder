/*
 * Phase Vocoder example Female Voice Wet Mix.
 */
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>
#include "effect_vocoder.h"

// WAV files converted to code by wav2sketch
#include "AudioSampleBurning_in_my_soul.h"
#include "AudioSampleDo_what_you_want_to_do.h"

AudioPlayMemory      voice;
AudioEffectVocoder   vocoder;
AudioMixer4          mix;
AudioOutputUSB       usb;
AudioOutputAnalog    dac;

AudioConnection patch1    (voice, vocoder);
AudioConnection patch2    (vocoder, 0, mix, 0);
AudioConnection patch3    (voice,   0, mix, 1);
AudioConnection patch_usbl(mix,     0, usb, 0);
AudioConnection patch_usbr(mix,     0, usb, 1);
AudioConnection patch_dac (mix, dac);

Bounce button0 = Bounce(3, 5);
Bounce button1 = Bounce(4, 5);
Bounce button2 = Bounce(5, 5);
Bounce button3 = Bounce(6, 5);
Bounce button4 = Bounce(7, 5);

//const unsigned int *sample = AudioSampleBurning_in_my_soul;
const unsigned int *sample = AudioSampleDo_what_you_want_to_do;

void setup() {
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  AudioMemory(15);
  //dac.analogReference(INTERNAL);
  mix.gain(0, 0.4);
  mix.gain(1, 0.4);
}

void loop() {
  button0.update();
  button1.update();
  button2.update();
  button3.update();
  button4.update();

  if (button0.fallingEdge()) {
    vocoder.setPitchShift(-5);
    Serial.print("Pitch Shift -5 Semitones | ");
    Serial.print("Max CPU Usage = ");
    Serial.print(AudioProcessorUsageMax(), 1);
    Serial.println("%");
    voice.play(sample);
  }
  if (button1.fallingEdge()) {
    vocoder.setPitchShift(-3);
    Serial.print("Pitch Shift -3 Semitones | ");
    Serial.print("Max CPU Usage = ");
    Serial.print(AudioProcessorUsageMax(), 1);
    Serial.println("%");
    voice.play(sample);
  }
  if (button2.fallingEdge()) {
    vocoder.setPitchShift(0);
    Serial.print("Pitch Shift 0 Semitones | ");
    Serial.print("Max CPU Usage = ");
    Serial.print(AudioProcessorUsageMax(), 1);
    Serial.println("%");
    voice.play(sample);
  }
  if (button3.fallingEdge()) {
    vocoder.setPitchShift(3);
    Serial.print("Pitch Shift 3 Semitones | ");
    Serial.print("Max CPU Usage = ");
    Serial.print(AudioProcessorUsageMax(), 1);
    Serial.println("%");
    voice.play(sample);
  }
  if (button4.fallingEdge()) {
    vocoder.setPitchShift(5);
    Serial.print("Pitch Shift 5 Semitones | ");
    Serial.print("Max CPU Usage = ");
    Serial.print(AudioProcessorUsageMax(), 1);
    Serial.println("%");
    voice.play(sample);
  }
}
