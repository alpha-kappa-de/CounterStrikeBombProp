/*
 * MIT License
 *
 * Copyright (c) 2024 alpha-kappa-de
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef COUNTERSTRIKEBOMBPROP_CSSOUND_H
#define COUNTERSTRIKEBOMBPROP_CSSOUND_H

#include "../CounterStrikeBombProp.h"
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

/**
 * @brief This class is responsible for managing the sound module and buzzer (+ the red LED, which blinks when the
 *        buzzer is activated) of the Counter Strike Bomb Prop. It uses the DFRobotDFPlayerMini library for the MP3
 *        module and the SoftwareSerial library for the communication with the MP3 module. <br><br>
 * @details The mp3 module, buzzer and led get initialized when calling the setup() function. The tick function should
 *          be called in the loop() function to deactivate the red LED when the buzzer is not active. The playSoundXXX()
 *          functions can be used to send an instruction to the mp3 Module to play the different sounds. <br><br>
 *          Before using an object of this class, make sure to call the setup() method. <br><br>
 *          <h1>Methods</h1> <br>
 *          <b>setup():</b> Setup the sound module with the given parameters <br><br>
 *          <b>tick():</b> Tick method which should be called in the loop(). Deactivates the red LED when the buzzer is
 *                         not active <br><br>
 *          <b>beepBuzzer():</b> Beep the buzzer with the given frequency and duration <br><br>
 *          <b>playSound():</b> Sends an instruction to the mp3 module to play the sound with the given sound id. This
 *                              is for testing purposes or extensions <br><br>
 *          <b>playSoundInit():</b> Sends an instruction to the mp3 module to play the init sound <br><br>
 *          <b>playSoundRadioBombPlanted():</b> Sends an instruction to the mp3 module to play the radio bomb planted
 *                                              sound <br><br>
 *          <b>playSoundRadioBombTickingDown():</b> Sends an instruction to the mp3 module to play the radio bomb
 *                                                  ticking down sound <br><br>
 *          <b>playSoundBeforeExplosion():</b> Sends an instruction to the mp3 module to play the sound before explosion <br><br>
 *          <b>playSoundExplosionTerWin():</b> Sends an instruction to the mp3 module to play the explosion + terrorists
 *                                             win sound <br><br>
 *          <b>playSoundDisarmStart():</b> Sends an instruction to the mp3 module to play the disarm start sound <br><br>
 *          <b>playSoundBombDefusedCTWin():</b> Sends an instruction to the mp3 module to play the bomb defused + CT win
 *                                              sound <br><br>
 *          <b>playSoundSilence():</b> Sends an instruction to the mp3 module to play the silence sound (for better
 *                                     timing, play this after another sound finishes, so that the next played sound
 *                                     gets played almost instantly) <br><br>
 *          <b>stopSound():</b> Sends an instruction to the mp3 module to stop the currently played sound <br><br>

 * @see DFRobotDFPlayerMini, SoftwareSerial
 */
class CSSound {
private:
    byte buzzerPin;                         // Buzzer pin
    unsigned int buzzerFrequency;           // Buzzer frequency for normal beeps
    unsigned int buzzerFrequencyHigh;       // Buzzer frequency for high pitch beeps (last 10 seconds)
    unsigned long buzzerDuration;           // Duration of a beep
    byte ledRedPin;                         // Red LED pin
    bool blinkLedToBuzzer;                  // If true, the red LED will blink when the buzzer is activated
    unsigned long lastTime;                 // Last millis() time, so that the code is not blocking

    byte mp3ModuleRXPin;                    // MP3 module RX pin (should be connected to the TX pin of the Arduino)
    byte mp3ModuleTXPin;                    // MP3 module TX pin (should be connected to the RX pin of the Arduino)
    byte mp3ModuleVolume;                   // MP3 module volume (0-30)
    bool mp3ModuleInitialized;              // If the MP3 module is initialized (is set in the setup() method)

    byte mp3SoundIdInit;                    // when the switch is toggled
    byte mp3SoundIdRadioBombPlanted;        // when code is entered completely
    byte mp3SoundIdRadioBombTickingDown;    // plays some seconds before explosion
    byte mp3SoundIdBeforeExplosion;         // high pitch sound right before detonation
    byte mp3SoundIdExplosionTerWin;         // actual explosion sound
    byte mp3SoundIdDisarmStart;             // defuse begin sound
    byte mp3SoundIdBombDefusedCTWin;        // defuse finished sound
    byte mp3SoundIdSilence;                 // silence, which plays between some other sounds for better timing

    SoftwareSerial* mp3Serial = nullptr;    // SoftwareSerial object for the MP3 module
    DFRobotDFPlayerMini* mp3 = nullptr;     // DFRobotDFPlayerMini object for the MP3 module

public:
    /**
     * @brief Constructor
     */
    CSSound();

    /**
     * @brief Destructor
     */
    ~CSSound();

    /**
     * @brief Setup the sound module with the given parameters.
     * @param buzzerPin The pin of the buzzer
     * @param buzzerFrequency The frequency of the buzzer for normal beeps
     * @param buzzerFrequencyHigh The frequency of the buzzer for high pitch beeps (last 10 seconds)
     * @param buzzerDuration The duration of a beep
     * @param ledRedPin The pin of the red LED
     * @param blinkLedToBuzzer If true, the red LED will blink when the buzzer is activated
     * @param mp3ModuleRXPin The RX pin of the MP3 module (should be connected to the SoftwareSerial TX pin of the Arduino)
     * @param mp3ModuleTXPin The TX pin of the MP3 module (should be connected to the SoftwareSerial RX pin of the Arduino)
     * @param mp3ModuleVolume The volume of the MP3 module (0-30)
     * @param mp3SoundIdInit the sound id of the sound which is played when the switch is toggled
     * @param mp3SoundIdRadioBombPlanted the sound id of the sound which is played when code is entered completely
     * @param mp3SoundIdRadioBombTickingDown the sound id of the sound which is played some seconds before explosion
     * @param mp3SoundIdBeforeExplosion the sound id of the sound which is played right before detonation
     * @param mp3SoundIdExplosionTerWin the sound id of the sound which is played when the bomb explodes
     * @param mp3SoundIdDisarmStart the sound id of the sound which is played when defusing starts
     * @param mp3SoundIdBombDefusedCTWin the sound id of the sound which is played when defusing is finished
     * @param mp3SoundIdSilence the sound id of the sound which is played between some other sounds for better timing
     */
    void setup(byte buzzerPin, unsigned int buzzerFrequency, unsigned int buzzerFrequencyHigh, unsigned long buzzerDuration, byte ledRedPin, bool blinkLedToBuzzer, byte mp3ModuleRXPin, byte mp3ModuleTXPin, byte mp3ModuleVolume, byte mp3SoundIdInit, byte mp3SoundIdRadioBombPlanted, byte mp3SoundIdRadioBombTickingDown, byte mp3SoundIdBeforeExplosion, byte mp3SoundIdExplosionTerWin, byte mp3SoundIdDisarmStart, byte mp3SoundIdBombDefusedCTWin, byte mp3SoundIdSilence);

    /**
     * @brief Tick method which should be called in the loop(). Deactivates the red LED when the buzzer is not active.
     */
    void tick();

    /**
     * @brief Beep the buzzer with the given frequency and duration.
     * @param highPitch If true, the buzzer will beep with the high pitch frequency, otherwise with the normal frequency
     */
    void beepBuzzer(bool highPitch);

    /**
     * @brief Sends an instruction to the mp3 module to play the sound with the given sound id. This is for testing purposes or extensions.
     * @param soundId The sound id of the sound to play
     */
    void playSound(byte soundId);

    /**
     * @brief Sends an instruction to the mp3 module to play the init sound.
     */
    void playSoundInit();

    /**
     * @brief Sends an instruction to the mp3 module to play the radio bomb planted sound.
     */
    void playSoundRadioBombPlanted();

    /**
     * @brief Sends an instruction to the mp3 module to play the radio bomb ticking down sound.
     */
    void playSoundRadioBombTickingDown();

    /**
     * @brief Sends an instruction to the mp3 module to play the sound before explosion.
     */
    void playSoundBeforeExplosion();

    /**
     * @brief Sends an instruction to the mp3 module to play the explosion + terrorists win sound.
     */
    void playSoundExplosionTerWin();

    /**
     * @brief Sends an instruction to the mp3 module to play the disarm start sound.
     */
    void playSoundDisarmStart();

    /**
     * @brief Sends an instruction to the mp3 module to play the bomb defused + CT win sound
     */
    void playSoundBombDefusedCTWin();

    /**
     * @brief Sends an instruction to the mp3 module to play the silence sound (for better timing, play this after
     *        another sound finishes, so that the next played sound gets played almost instantly)
     */
    void playSoundSilence();

    /**
     * @brief Sends an instruction to the mp3 module to stop the currently played sound
     */
    void stopSound();
};

#endif
