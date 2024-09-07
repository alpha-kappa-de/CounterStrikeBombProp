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

#include "CSSound.h"

CSSound::CSSound() { }

CSSound::~CSSound() {
    delete this->mp3;
    delete this->mp3Serial;
}

void CSSound::setup(byte buzzerPin, unsigned int buzzerFrequency, unsigned int buzzerFrequencyHigh,
                    unsigned long buzzerDuration, byte ledRedPin, bool blinkLedToBuzzer, byte mp3ModuleRXPin,
                    byte mp3ModuleTXPin, byte mp3ModuleVolume, byte mp3SoundIdInit, byte mp3SoundIdRadioBombPlanted,
                    byte mp3SoundIdRadioBombTickingDown, byte mp3SoundIdBeforeExplosion, byte mp3SoundIdExplosionTerWin,
                    byte mp3SoundIdDisarmStart, byte mp3SoundIdBombDefusedCTWin, byte mp3SoundIdSilence) {
    this->mp3ModuleRXPin = mp3ModuleRXPin;
    this->mp3ModuleTXPin = mp3ModuleTXPin;
    this->mp3ModuleVolume = mp3ModuleVolume;
    this->mp3ModuleInitialized = false;
    this->lastTime = 0;

    this->mp3SoundIdInit = mp3SoundIdInit;
    this->mp3SoundIdRadioBombPlanted = mp3SoundIdRadioBombPlanted;
    this->mp3SoundIdRadioBombTickingDown = mp3SoundIdRadioBombTickingDown;
    this->mp3SoundIdBeforeExplosion = mp3SoundIdBeforeExplosion;
    this->mp3SoundIdExplosionTerWin = mp3SoundIdExplosionTerWin;
    this->mp3SoundIdDisarmStart = mp3SoundIdDisarmStart;
    this->mp3SoundIdBombDefusedCTWin = mp3SoundIdBombDefusedCTWin;
    this->mp3SoundIdSilence = mp3SoundIdSilence;

    this->mp3Serial = new SoftwareSerial(this->mp3ModuleTXPin, this->mp3ModuleRXPin); // inverted because params are arduinos RX and TX
    this->mp3Serial->begin(9600);
    this->mp3 = new DFRobotDFPlayerMini();

    bool mp3InitSuccess = this->mp3->begin(*this->mp3Serial);

    if (!mp3InitSuccess) {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, CONFIG_VARIABLE_PREFIX_MP3);
        Serial.println("Error!");
    #endif
    } else {
        this->mp3->volume(this->mp3ModuleVolume);
        this->mp3ModuleInitialized = true;
        #if defined(SERIAL_DEBUG)
            DebugPrint(PREFIX_SOUND, CONFIG_VARIABLE_PREFIX_MP3);
            Serial.println(STR_INITIALIZED);
        #endif
    }

    this->blinkLedToBuzzer = blinkLedToBuzzer;

    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_SOUND, STR_INITIALIZED);
    #endif

    this->buzzerPin = buzzerPin;
    this->buzzerFrequency = buzzerFrequency;
    this->buzzerFrequencyHigh = buzzerFrequencyHigh;
    this->buzzerDuration = buzzerDuration;
    this->ledRedPin = ledRedPin;

    pinMode(this->ledRedPin, OUTPUT);
    digitalWrite(this->ledRedPin, LOW);

    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_BUZZER, STR_INITIALIZED);
    #endif
}

void CSSound::tick() {
    if (this->blinkLedToBuzzer && this->lastTime + this->buzzerDuration < millis()) {
        digitalWrite(this->ledRedPin, LOW);
    }
}

void CSSound::beepBuzzer(bool highPitch) {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_BUZZER, "Activate Buzzer");
    #endif

    if (this->blinkLedToBuzzer) {
        digitalWrite(this->ledRedPin, HIGH);
        this->lastTime = millis();
    }

    if (highPitch) {
        tone(this->buzzerPin, this->buzzerFrequencyHigh, this->buzzerDuration);
    } else {
        tone(this->buzzerPin, this->buzzerFrequency, this->buzzerDuration);
    }
}

void CSSound::playSound(byte soundId) {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, this->mp3ModuleInitialized ? STR_PLAY_SOUND : STR_NOT_PLAY_SOUND);
        Serial.print("(");
        Serial.print(CONFIG_VARIABLE_PREFIX_SOUND);
        Serial.print(soundId);
        Serial.println(")");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->play(soundId);
}

void CSSound::playSoundInit() {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, this->mp3ModuleInitialized ? STR_PLAY_SOUND : STR_NOT_PLAY_SOUND);
        Serial.print("Init (");
        Serial.print(CONFIG_VARIABLE_PREFIX_SOUND);
        Serial.print(this->mp3SoundIdInit);
        Serial.println(")");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->play(this->mp3SoundIdInit);
}

void CSSound::playSoundRadioBombPlanted() {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, this->mp3ModuleInitialized ? STR_PLAY_SOUND : STR_NOT_PLAY_SOUND);
        Serial.print("Radio Bomb Planted (");
        Serial.print(CONFIG_VARIABLE_PREFIX_SOUND);
        Serial.print(this->mp3SoundIdRadioBombPlanted);
        Serial.println(")");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->play(this->mp3SoundIdRadioBombPlanted);
}

void CSSound::playSoundRadioBombTickingDown() {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, this->mp3ModuleInitialized ? STR_PLAY_SOUND : STR_NOT_PLAY_SOUND);
        Serial.print("Radio Bomb Ticking Down (");
        Serial.print(CONFIG_VARIABLE_PREFIX_SOUND);
        Serial.print(this->mp3SoundIdRadioBombTickingDown);
        Serial.println(")");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->play(this->mp3SoundIdRadioBombTickingDown);
}

void CSSound::playSoundBeforeExplosion() {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, this->mp3ModuleInitialized ? STR_PLAY_SOUND : STR_NOT_PLAY_SOUND);
        Serial.print("Before Explosion (");
        Serial.print(CONFIG_VARIABLE_PREFIX_SOUND);
        Serial.print(this->mp3SoundIdBeforeExplosion);
        Serial.println(")");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->play(this->mp3SoundIdBeforeExplosion);
}

void CSSound::playSoundExplosionTerWin() {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, this->mp3ModuleInitialized ? STR_PLAY_SOUND : STR_NOT_PLAY_SOUND);
        Serial.print("Explosion Ter Win (");
        Serial.print(CONFIG_VARIABLE_PREFIX_SOUND);
        Serial.print(this->mp3SoundIdExplosionTerWin);
        Serial.println(")");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->play(this->mp3SoundIdExplosionTerWin);
}

void CSSound::playSoundDisarmStart() {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, this->mp3ModuleInitialized ? STR_PLAY_SOUND : STR_NOT_PLAY_SOUND);
        Serial.print("Disarm Start (");
        Serial.print(CONFIG_VARIABLE_PREFIX_SOUND);
        Serial.print(this->mp3SoundIdDisarmStart);
        Serial.println(")");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->play(this->mp3SoundIdDisarmStart);
}

void CSSound::playSoundBombDefusedCTWin() {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, this->mp3ModuleInitialized ? STR_PLAY_SOUND : STR_NOT_PLAY_SOUND);
        Serial.print("Bomb Defused CT Win (");
        Serial.print(CONFIG_VARIABLE_PREFIX_SOUND);
        Serial.print(this->mp3SoundIdBombDefusedCTWin);
        Serial.println(")");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->play(this->mp3SoundIdBombDefusedCTWin);
}

void CSSound::playSoundSilence() {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, this->mp3ModuleInitialized ? STR_PLAY_SOUND : STR_NOT_PLAY_SOUND);
        Serial.print("Silence (");
        Serial.print(CONFIG_VARIABLE_PREFIX_SOUND);
        Serial.print(this->mp3SoundIdSilence);
        Serial.println(")");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->play(this->mp3SoundIdSilence);
}

void CSSound::stopSound() {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SOUND, STR_NOT_PLAY_SOUND);
        Serial.println("Stop Sound");
    #endif

    if (!this->mp3ModuleInitialized) {
        return;
    }
    this->mp3->stop();
}