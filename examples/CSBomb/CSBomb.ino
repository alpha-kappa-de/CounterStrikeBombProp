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

#include <CounterStrikeBombProp.h>

// -------------------------------------------------------------------------------------
//                           Set up your configuration here
// -------------------------------------------------------------------------------------
long            bombTimer                                   = 40000;    // 40 seconds = 40000 ms

byte            buzzerPin                                   = A1;
unsigned int    buzzerFrequency                             = 4186;     // 4186 Hz = C8
unsigned int    buzzerFrequencyHigh                         = 5233;     // 5233 Hz = C9
unsigned long   buzzerDuration                              = 125;
byte            ledRedPin                                   = A0;
bool            blinkRedLedToBuzzer                         = true;

byte            ledYellowPin                                = 5;

byte            switchPin                                   = A3;
byte            switchOnState                               = HIGH;
byte            switchDebounceTime                          = 100;

byte            displayAddress                              = 0x27;
byte            displayCharsPerLine                         = 8;        // 16x1 Display with I2C Adapter is handled
byte            displayLines                                = 2;        // internally as 8x2
bool            displayBacklight                            = false;

const byte      keypadNumRows                               = 4;
const byte      keypadNumCols                               = 3;
byte            keypadPinsRows[keypadNumRows]               = {7, 12, 11, 9};
byte            keypadPinsCols[keypadNumCols]               = {8, 6, 10};
char            keypadKeys[keypadNumRows][keypadNumCols]    = {
                                                                  {'1', '2', '3'},
                                                                  {'4', '5', '6'},
                                                                  {'7', '8', '9'},
                                                                  {'*', '0', '#'}
};

const byte      keypadQueueSize                             = 10;       // Maximum number of keys that can be enqueued

byte            mp3ModuleRXPin                              = 3;        // SoftwareSerial TX on the Arduino
byte            mp3ModuleTXPin                              = 2;        // SoftwareSerial RX on the Arduino
byte            mp3ModuleVolume                             = 20;       // 0-30

byte            mp3SoundIdInit                              = 1;        // when the switch is toggled
byte            mp3SoundIdRadioBombPlanted                  = 2;        // when code is entered completely
byte            mp3SoundIdRadioBombTickingDown              = 3;        // plays some seconds before explosion
byte            mp3SoundIdBeforeExplosion                   = 4;        // high pitch sound right before detonation
byte            mp3SoundIdExplosionTerWin                   = 5;        // actual explosion sound
byte            mp3SoundIdDisarmStart                       = 6;        // defuse begin sound
byte            mp3SoundIdBombDefusedCTWin                  = 7;        // defuse finished sound
byte            mp3SoundIdSilence                           = 8;        // silence, which plays between some other sounds for better timing

// -------------------------------------------------------------------------------------
//            Config end. Do not change anything below unless you know what you do
// -------------------------------------------------------------------------------------

// Define the components
CSSwitch sideSwitch = CSSwitch();
CSKeypad keypad = CSKeypad();
CSYellowLED yellowLED = CSYellowLED();
CSDisplay display = CSDisplay();
CSSound sound = CSSound();


// some variables for the state machine in the loop
byte currentState;
bool bombPlanted;
unsigned long bombTimerStart;
unsigned long nextBeep;
unsigned long defuseStart;
bool hasKit;
float bombTimerPercentage;
bool bombIsTickingDownSoundPlayed;
bool bombIsBeforeExplosionSoundPlayed;
bool stopBeeping;

void setup() {
    currentState = 0;
    bombPlanted = false;
    bombTimerStart = 0;
    nextBeep = 0;
    bombTimerPercentage = 0.0;
    bombIsTickingDownSoundPlayed = false;
    bombIsBeforeExplosionSoundPlayed = false;
    stopBeeping = false;
    hasKit = false;
    defuseStart = 0;

    #if defined(SERIAL_DEBUG)
        Serial.begin(SERIAL_DEBUG_BAUD_RATE);
        Serial.println();

    // I implemented the function to print the configuration of the bomb in the beginning, but on the arduino nano it was too big
    // and the board ran out of memory. I left the function here in case you want to use it on an arduino with more memory.

//         DebugPrintConfig(bombTimer, buzzerPin, buzzerFrequency, buzzerFrequencyHigh, buzzerDuration, ledRedPin,
//                          blinkRedLedToBuzzer, ledYellowPin, switchPin, switchOnState, switchDebounceTime,
//                          displayAddress, displayCharsPerLine, displayLines, displayBacklight, keypadNumRows,
//                          keypadNumCols, keypadPinsRows, keypadPinsCols, *keypadKeys, keypadQueueSize,
//                          mp3ModuleRXPin, mp3ModuleTXPin, mp3ModuleVolume, mp3SoundIdInit, mp3SoundIdRadioBombPlanted,
//                          mp3SoundIdRadioBombTickingDown, mp3SoundIdBeforeExplosion, mp3SoundIdExplosionTerWin,
//                          mp3SoundIdDisarmStart, mp3SoundIdBombDefusedCTWin, mp3SoundIdSilence);
    #endif

    // Initialize the components
    sideSwitch.setup(switchPin, switchOnState, switchDebounceTime);
    keypad.setup(keypadNumRows, keypadNumCols, keypadPinsRows, keypadPinsCols, *keypadKeys, keypadQueueSize);
    yellowLED.setup(ledYellowPin);
    display.setup(displayAddress, displayCharsPerLine, displayLines, displayBacklight);
    sound.setup(buzzerPin, buzzerFrequency, buzzerFrequencyHigh, buzzerDuration, ledRedPin, blinkRedLedToBuzzer,
                mp3ModuleRXPin, mp3ModuleTXPin, mp3ModuleVolume, mp3SoundIdInit, mp3SoundIdRadioBombPlanted,
                mp3SoundIdRadioBombTickingDown, mp3SoundIdBeforeExplosion, mp3SoundIdExplosionTerWin,
                mp3SoundIdDisarmStart, mp3SoundIdBombDefusedCTWin, mp3SoundIdSilence);

    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_CS_BOMB, STR_INITIALIZED);
        Serial.println();
        Serial.println();
        Serial.println();
    #endif
}

void loop() {
    yellowLED.tick();
    keypad.tick();
    display.tick();
    sound.tick();
    bombTimerPercentage = (float(millis() - bombTimerStart) / bombTimer) * 100;

    switch (currentState) {
        case 0: // init (Activate yellow LED animation and go to standby)
            yellowLED.activateAnimation(0, 255, 15, 15, 300, 300, 0);
            keypad.setAllowedToEnqueue(false);
            keypad.resetQueue();
            display.stopAnimation();
            sound.playSoundSilence();   // play silence so that the init sound is played almost instantly when the switch is triggered
            currentState = 1; // Go to standby state
            break;

        case 1: // Standby (Wait for the switch to be activated)
            if (sideSwitch.tick()) {
                sound.playSoundInit();
                yellowLED.stopAnimation();
                keypad.setAllowedToEnqueue(true);
                display.startDisplayBombCodeAnimation();
                currentState = 2; // Go to activated state
            }
            break;

        case 2: // Activated (Wait for the password to be entered)
            if (!sideSwitch.tick()) {
                // The switch is turned off, stop the sound and go back to init state
                sound.stopSound();
                currentState = 0; // Go back to init state (reset) if the switch is turned off
                break;
            }
            if (keypad.hasButton()) {
                char button = keypad.getButton();
                sound.playSoundSilence();
                switch (button) {
                    case '*':
                    case '#':
                        // let the user start over with the bomb code if he presses * or #
                        display.clearBombCode();
                        break;
                    default:
                        if (display.addToBombCode(button)) {
                            // Bomb code is complete
                            display.tick(); // Update the display to show the last entered character
                            sound.playSoundRadioBombPlanted();
                            bombTimerStart = millis();
                            bombPlanted = true;
                            nextBeep = millis() + buzzerDuration + (866 - 8.5 * bombTimerPercentage);
                            currentState = 3; // Go to the state where it waits for the sound to finish and then to the bomb planted state
                        }
                        break;
                }
            }
            break;

        case 3: // Bomb planted (Wait for the radio bomb planted sound to finish)
            if (millis() - bombTimerStart >= 1500) {
                sound.playSoundSilence();
                display.stopAnimation();
                display.startPlantedAnimation();
                currentState = 4; // Go to bomb planted state
            }
            // beep the buzzer while the radio bomb planted sound is playing
            // defusing is not possible here
            if (millis() >= nextBeep && !stopBeeping) {
                // high pitch beep if the bomb is planted, and it's the last 10 seconds, else a normal beep
                sound.beepBuzzer(bombPlanted && (millis() - bombTimerStart) > (bombTimer - 10000));
                nextBeep = millis() + buzzerDuration + int(866 - 8.5 * bombTimerPercentage);
                #if defined(SERIAL_DEBUG)
                    DebugPrint(PREFIX_CS_BOMB, "Beep! (Timer: ");
                    Serial.print(bombTimerPercentage);
                    DebugPrintln(" %)");
                #endif
            }
            break;

        case 4: // Bomb planted (Wait for the bomb to explode/get defused)
            if (keypad.hasButton()) {
                switch (keypad.getButton()) {
                    case '#':
                        hasKit = true;  // The user has a defuse kit. Set the flag and fall through to the next case
                    case '*':
                        sound.playSoundDisarmStart();
                        display.stopAnimation();
                        display.startDefusingAnimation(hasKit);
                        defuseStart = millis();
                        break;
                    default:
                        break;
                }
            }

            // if defusing started
            if (defuseStart > 0) {
                // check if the defuse time is over
                if (millis() - defuseStart >= (hasKit ? 5000 : 10000)) {
                    sound.playSoundBombDefusedCTWin();
                    yellowLED.stopAnimation();
                    currentState = 5; // Go to end state
                }

                // check if the key is not held down anymore
                if (!keypad.isAnyKeyPressed()) {
                    // key is not held down anymore, stop defusing -> reset the defusing parameters
                    sound.playSoundSilence();
                    display.stopAnimation();
                    display.startPlantedAnimation();
                    defuseStart = 0;
                    hasKit = false;
                }
            }

            // beep the buzzer while the bomb is ticking down
            if (millis() >= nextBeep && !stopBeeping) {
                // high pitch beep if the bomb is planted, and it's the last 10 seconds, else a normal beep
                sound.beepBuzzer(bombPlanted && (millis() - bombTimerStart) > (bombTimer - 10000));
                nextBeep = millis() + buzzerDuration + int(866 - 8.5 * bombTimerPercentage);
                #if defined(SERIAL_DEBUG)
                    DebugPrint(PREFIX_CS_BOMB, "Beep! (Timer: ");
                    Serial.print(bombTimerPercentage);
                    DebugPrintln(" %)");
                #endif
            }

            // play the radio bomb ticking down sound 5.5 seconds before the bomb explodes
            if (!bombIsTickingDownSoundPlayed && millis() - bombTimerStart >= bombTimer - 5500) {
                sound.playSoundRadioBombTickingDown();
                bombIsTickingDownSoundPlayed = true;
            }

            // play the before explosion sound 2.5 seconds before the bomb explodes
            if (!bombIsBeforeExplosionSoundPlayed && millis() - bombTimerStart >= bombTimer - 2500) {
                sound.playSoundBeforeExplosion();
                yellowLED.activateAnimation(0, 255, 15, 15, 300, 1000, 700, false, 500);
                bombIsBeforeExplosionSoundPlayed = true;
            }

            if (millis() - bombTimerStart >= bombTimer - 1500) {
                stopBeeping = true;
            }

            if (millis() - bombTimerStart >= bombTimer - 400) {
                sound.playSoundExplosionTerWin();
                display.stopAnimation();
                yellowLED.stopAnimation();
                keypad.setAllowedToEnqueue(true);
                currentState = 5; // Go to end state
            }
            break;

        case 5: // End (reset the bomb and go back to beginning if the switch is turned off)
            if (!sideSwitch.tick()) {
                // Go back to init state (reset) if the switch is turned off
                sound.playSoundSilence();
                currentState = 0;
                bombPlanted = false;
                bombTimerStart = 0;
                nextBeep = 0;
                bombTimerPercentage = 0.0;
                bombIsTickingDownSoundPlayed = false;
                bombIsBeforeExplosionSoundPlayed = false;
                stopBeeping = false;
                defuseStart = 0;
                hasKit = false;
                break;
            }
            break;
        default:
            break;
    }
}