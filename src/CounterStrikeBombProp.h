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

#ifndef COUNTERSTRIKEBOMBPROP_COUNTERSTRIKEBOMBPROP_H
#define COUNTERSTRIKEBOMBPROP_COUNTERSTRIKEBOMBPROP_H

// #define SERIAL_DEBUG    // Uncomment this line to enable serial debug
#define SERIAL_DEBUG_BAUD_RATE 115200
#define CONFIG_PADDING 35


#include <Arduino.h>
#include <Wire.h>
#include "CounterStrikeBombProp/CSKeypad.h"
#include "CounterStrikeBombProp/CSDisplay.h"
#include "CounterStrikeBombProp/CSSound.h"
#include "CounterStrikeBombProp/CSYellowLED.h"
#include "CounterStrikeBombProp/CSSwitch.h"


 #ifdef SERIAL_DEBUG
 #warning "Serial Debug is enabled for Counter Strike Bomb Prop. On some boards, this can make the arduino to run out of memory. Make sure to comment SERIAL_DEBUG after testing."
 #endif

// Prefixes for the serial print statements for each component
// These prefixes are used to identify the source of the print statement and to use less space
#define PREFIX_CS_BOMB    "[ CS BOMB ]: "
#define PREFIX_DISPLAY    "[ DISPLAY ]: "
#define PREFIX_KEYPAD     "[ KEYPAD  ]: "
#define PREFIX_SOUND      "[  SOUND  ]: "
#define PREFIX_BUZZER     "[ BUZZER  ]: "
#define PREFIX_SWITCH     "[ SWITCH  ]: "
#define PREFIX_YELLOW_LED "[  Y LED  ]: "

#define CONFIG_VARIABLE_PREFIX_BUZZER "Buzzer "
#define CONFIG_VARIABLE_PREFIX_LED "LED "
#define CONFIG_VARIABLE_PREFIX_SWITCH "Switch "
#define CONFIG_VARIABLE_PREFIX_DISPLAY "Display "
#define CONFIG_VARIABLE_PREFIX_KEYPAD "Keypad "
#define CONFIG_VARIABLE_PREFIX_MP3 "Mp3 Module "
#define CONFIG_VARIABLE_PREFIX_SOUND "Sound ID "
#define STR_PLAY_SOUND "Play Sound "
#define STR_NOT_PLAY_SOUND "No MP3 "
#define STR_INITIALIZED "Ready!"

// some functions for printing with a prefix, padding and a separator line

static void DebugPrint(const char *str) {
    Serial.print(str);
}

static void DebugPrint(const char *prefix, const char *str) {
    Serial.print(prefix);
    Serial.print(str);
}

static void DebugPrintln(const char *str) {
    Serial.println(str);
}

static void DebugPrintln(const char *prefix, const char *str) {
    DebugPrint(prefix, str);
    Serial.println();
}

static void DebugPrintSeperatorLine(const char *prefix) {
    Serial.print(prefix);
    for (byte i = 0; i < 80; i++) {
        Serial.print("-");
    }
    Serial.println();
}

static void DebugPrintWithPadding(const char *modulePrefix, const char *keyPrefix, const char *key) {
    Serial.print(modulePrefix);
    Serial.print(keyPrefix);
    Serial.print(key);
    int spaces = CONFIG_PADDING - strlen(key) - strlen(keyPrefix);
    for (int i = 0; i < spaces; ++i) {
        Serial.print(" ");
    }
    Serial.print(": ");
}

// I implemented the following functions to print the configuration of the bomb in the beginning, but on the arduino nano it was too big
// and the board ran out of memory. I left the functions here in case you want to use them on an arduino with more memory.

static void DebugPrintConfigHeader(const char *prefix, const char *str) {
    DebugPrintSeperatorLine(prefix);
    DebugPrintln(prefix, str);
    DebugPrintSeperatorLine(prefix);
}

static void DebugPrintConfigLine(const char *modulePrefix, const char *keyPrefix, const char *key, int value, int base = DEC) {
    DebugPrintWithPadding(modulePrefix, keyPrefix, key);
    if (base == HEX) {
        Serial.print("0x");
        Serial.println(value, HEX);
    } else {
        Serial.println(value, base);
    }
}

static void DebugPrintConfigLine(const char *modulePrefix, const char *keyPrefix, const char *key, const char *value) {
    DebugPrintWithPadding(modulePrefix, keyPrefix, key);
    Serial.println(value);
}

static void DebugPrintConfigLine(const char *modulePrefix, const char *keyPrefix, const char *key, byte *array, byte size) {
    DebugPrintWithPadding(modulePrefix, keyPrefix, key);
    Serial.print("{");
    for (byte i = 0; i < size; ++i) {
        Serial.print(array[i]);
        if (i < size - 1) {
            Serial.print(", ");
        } else {
            Serial.println("}");
        }
    }
}

static void DebugPrintConfigLine(const char *modulePrefix, const char *keyPrefix, const char *key, char *array, byte rows, byte cols) {
    DebugPrintWithPadding(modulePrefix, keyPrefix, key);
    Serial.print("{");
    for (byte i = 0; i < rows; ++i) {
        Serial.print("{");
        for (byte j = 0; j < cols; ++j) {
            Serial.print(array[i * cols + j]);
            if (j < cols - 1) {
                Serial.print(", ");
            }
        }
        if (i < rows - 1) {
            Serial.print("}, ");
        } else {
            Serial.println("}}");
        }
    }
}

static void DebugPrintConfig(int bombTimer, byte buzzerPin, unsigned int buzzerFrequency,
                             unsigned int buzzerFrequencyHigh, unsigned long buzzerDuration, byte ledRedPin,
                             bool blinkRedLedToBuzzer, byte ledYellowPin, byte switchPin, byte switchOnState,
                             byte switchDebounceTime, byte displayAddress, byte displayCharsPerLine,
                             byte displayLines, bool displayBacklight, const byte keypadNumRows,
                             const byte keypadNumCols, byte *keypadPinsRows, byte *keypadPinsCols, char *keypadKeys,
                             const byte keypadQueueSize, byte mp3ModuleRXPin, byte mp3ModuleTXPin,
                             byte mp3ModuleVolume, byte mp3SoundIdInit, byte mp3SoundIdRadioBombPlanted,
                             byte mp3SoundIdRadioBombTickingDown, byte mp3SoundIdBeforeExplosion,
                             byte mp3SoundIdExplosionTerWin, byte mp3SoundIdDisarmStart,
                             byte mp3SoundIdBombDefusedCTWin, byte mp3SoundIdSilence) {
    DebugPrintConfigHeader(PREFIX_CS_BOMB, "CURRENT CONFIGURATION");
    DebugPrintWithPadding(PREFIX_CS_BOMB, "", "Serial Baud Rate");
    Serial.println(SERIAL_DEBUG_BAUD_RATE);

    DebugPrintConfigLine(PREFIX_CS_BOMB, "", "Bomb Timer (sec)", bombTimer);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_BUZZER, "Pin", buzzerPin);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_BUZZER, "Freq", buzzerFrequency);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_BUZZER, "Freq High", buzzerFrequencyHigh);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_BUZZER, "Duration", buzzerDuration);

    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_LED, "Yellow Pin", ledYellowPin);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_LED, "Red Pin", ledRedPin);
    DebugPrintConfigLine(PREFIX_CS_BOMB, "", "Blink Red LED to Buzzer", blinkRedLedToBuzzer ? "YES" : "NO");

    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SWITCH, "Pin", switchPin);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SWITCH, "On State", switchOnState);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SWITCH, "Debounce Time", switchDebounceTime);

    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_DISPLAY, "Address", displayAddress, HEX);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_DISPLAY, "Chars/Line", displayCharsPerLine);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_DISPLAY, "Lines", displayLines);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_DISPLAY, "Backlight", displayBacklight ? "ON" : "OFF");

    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_KEYPAD, "Queue Size", keypadQueueSize);

    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_KEYPAD, "Rows Pins", keypadPinsRows, keypadNumRows);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_KEYPAD, "Columns Pins", keypadPinsCols, keypadNumCols);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_KEYPAD, "Keys", keypadKeys, keypadNumRows, keypadNumCols);

    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_MP3, "RX Pin", mp3ModuleRXPin);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_MP3, "TX Pin", mp3ModuleTXPin);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_MP3, "Volume", mp3ModuleVolume);

    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SOUND, "Init", mp3SoundIdInit);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SOUND, "RadioBombPlanted", mp3SoundIdRadioBombPlanted);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SOUND, "RadioBombTickingDown", mp3SoundIdRadioBombTickingDown);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SOUND, "BeforeExplosion", mp3SoundIdBeforeExplosion);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SOUND, "ExplosionTerWin", mp3SoundIdExplosionTerWin);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SOUND, "DisarmStart", mp3SoundIdDisarmStart);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SOUND, "BombDefusedCTWin", mp3SoundIdBombDefusedCTWin);
    DebugPrintConfigLine(PREFIX_CS_BOMB, CONFIG_VARIABLE_PREFIX_SOUND, "Silence", mp3SoundIdSilence);

    DebugPrintSeperatorLine(PREFIX_CS_BOMB);
}

#endif
