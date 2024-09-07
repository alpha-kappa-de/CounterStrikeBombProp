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

#include "CSYellowLED.h"

void CSYellowLED::resetFields() {
    animationActive = false;
    currentAnimationState = 0;
    lastTime = 0;

    this->minBrightness = 0;
    this->maxBrightness = 0;
    this->currentBrightness = 0;
    this->stepInSize = 0;
    this->stepOutSize = 0;
    this->cycleInMillis = 0;
    this->cycleOutMillis = 0;
    this->delayMillis = 0;
    this->loopAnimation = false;
}

CSYellowLED::CSYellowLED() { }

CSYellowLED::~CSYellowLED() { }

void CSYellowLED::setup(byte ledPin) {
    this->ledPin = ledPin;
    this->resetFields();
    pinMode(this->ledPin, OUTPUT);
    digitalWrite(this->ledPin, this->currentBrightness);

    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_YELLOW_LED, STR_INITIALIZED);
    #endif
}

void CSYellowLED::activateAnimation(byte minBrightness,
                                  byte maxBrightness,
                                  byte stepInSize,
                                  byte stepOutSize,
                                  int fadeInMillis,
                                  int fadeOutMillis,
                                  int delayMillis,
                                  bool loopAnimation,
                                  int startDelayMillis
                                  ) {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_YELLOW_LED, "Activating animation");
    #endif
    this->animationActive = true;
    this->minBrightness = minBrightness;
    this->maxBrightness = maxBrightness;
    this->stepInSize = stepInSize;
    this->stepOutSize = stepOutSize;
    this->cycleInMillis = fadeInMillis / ((maxBrightness - minBrightness) / stepInSize);
    this->cycleOutMillis = fadeOutMillis / ((maxBrightness - minBrightness) / stepOutSize);
    this->delayMillis = delayMillis;
    this->loopAnimation = loopAnimation;
    this->startDelayMillis = startDelayMillis;
    this->lastTime = millis();
}

void CSYellowLED::stopAnimation() {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_YELLOW_LED, "Stopping animation");
    #endif
    this->resetFields();
    digitalWrite(this->ledPin, this->currentBrightness);
}

void CSYellowLED::tick() {
    if (!this->animationActive) {
        return;
    }
    switch (this->currentAnimationState) {
        case 0: // init
            // // wait for start delay to pass
            if (this->lastTime + this->startDelayMillis < millis()) {
                this->currentBrightness = this->minBrightness;
                this->currentAnimationState = 1; // go to fade in
                this->lastTime = millis();
            }
            break;

        case 1: // fade in
            if (this->lastTime + this->cycleInMillis > millis()) {
                return; // wait for delay to pass
            }
            this->lastTime = millis();
            analogWrite(this->ledPin, this->currentBrightness);

            if (this->currentBrightness == this->maxBrightness) {
                this->currentAnimationState = 2; // go to delay between fade in and fade out
            } else {
                this->currentBrightness += this->stepInSize;
                if (this->currentBrightness > this->maxBrightness) {
                    this->currentBrightness = this->maxBrightness;
                }
            }
            break;

        case 2: //delay between fade in and fade out
            if (this->lastTime + this->delayMillis > millis()) {
                return; // wait for delay to pass
            }
            this->lastTime = millis();
            if (this->currentBrightness == this->maxBrightness) {
                this->currentAnimationState = 3; // go to fade out
            } else {
                this->currentAnimationState = 1; // go to fade in
            }
            break;

        case 3: // fade out
            if (this->lastTime + this->cycleOutMillis > millis()) {
                return; // wait for delay to pass
            }
            this->lastTime = millis();
            analogWrite(this->ledPin, this->currentBrightness);

            if (this->currentBrightness == this->minBrightness) {
                this->currentAnimationState = 2; // go to delay between fade in and fade out
                if (!this->loopAnimation) {
                    this->stopAnimation();
                }
            } else {
                this->currentBrightness -= this->stepOutSize;
                if (this->currentBrightness < this->minBrightness) {
                    this->currentBrightness = this->minBrightness;
                }
            }
            break;
    }
}