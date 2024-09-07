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

#include "CSSwitch.h"

CSSwitch::CSSwitch() { }

CSSwitch::~CSSwitch() { }

void CSSwitch::setup(byte switchPin, byte switchOnState, byte switchDebounceTime) {
    this->switchPin = switchPin;
    pinMode(switchPin, INPUT);

    this->switchOnState = switchOnState;
    this->switchDebounceTime = switchDebounceTime;
    currentState = false;
    lastMillis = 0;
    this->setAllowUpdate(true);

    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_SWITCH, STR_INITIALIZED);
    #endif
}

bool CSSwitch::tick() {
    if (allowUpdate) {
        bool switchOn = digitalRead(switchPin) == switchOnState;         // use this line to read the pin digital
        //bool switchOn = (analogRead(switchPin) > 950) == switchOnState;     // use this line to read the pin analog

        // check if the switch state has changed and if the debounce time has passed
        if (switchOn != currentState && millis() - lastMillis > switchDebounceTime) {
            #if defined(SERIAL_DEBUG)
                DebugPrint(PREFIX_SWITCH, "State changed to ");
                if (switchOn) {
                    Serial.println("ON");
                } else {
                    Serial.println("OFF");
                }
            #endif
            currentState = switchOn;
            lastMillis = millis();
        }
    }

    return currentState;
}

void CSSwitch::setAllowUpdate(bool allowUpdate) {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_SWITCH, "Allow update set to ");
        if (allowUpdate) {
            Serial.println("TRUE");
        } else {
            Serial.println("FALSE");
        }
    #endif
    this->allowUpdate = allowUpdate;
}