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

#include "CSDisplay.h"

CSDisplay::CSDisplay() { }

CSDisplay::~CSDisplay() { }

void CSDisplay::setup(byte displayAddress, byte displayCharsPerLine, byte displayLines, bool displayBacklight) {
    this->displayAddress = displayAddress;
    this->displayCharsPerLine = displayCharsPerLine;
    this->displayLines = displayLines;
    this->displayBacklight = displayBacklight;

    this->lcd = new LiquidCrystal_I2C(this->displayAddress, this->displayCharsPerLine, this->displayLines);
    this->bombCode = new char[BOMB_CODE_LENGTH];
    for (int i = 0; i < BOMB_CODE_LENGTH; i++) {
        this->bombCode[i] = '*';
    }

    this->currentAnimation = ANIMATION_LCD_NONE;
    this->currentAnimationState = 0;
    this->animationPlantedCountUp = true;
    this->hasDefuseKit = false;
    this->lastTime = 0;
    this->currentDefusingCharIndex = 0;
    this->defuseBeginTime = 0;
    this->DefusedAnimationShowChars = false;

    this->lcd->init();

    if (this->displayBacklight) {
        this->activateBacklight();
    } else {
        this->deactivateBacklight();
    }
    this->clearDisplay();

    this->lcd->createChar(FLIPPED_CHAR_1, this->flipped_1);
    this->lcd->createChar(FLIPPED_CHAR_2, this->flipped_2);
    this->lcd->createChar(FLIPPED_CHAR_3, this->flipped_3);
    this->lcd->createChar(FLIPPED_CHAR_4, this->flipped_4);
    this->lcd->createChar(FLIPPED_CHAR_5, this->flipped_5);
    this->lcd->createChar(FLIPPED_CHAR_7, this->flipped_7);

    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_DISPLAY, STR_INITIALIZED);
    #endif
}


void CSDisplay::activateBacklight() {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_DISPLAY, "Activating BL");
    #endif
    this->lcd->backlight();
}

void CSDisplay::deactivateBacklight() {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_DISPLAY, "Deactivating BL");
    #endif
    this->lcd->noBacklight();
}

void CSDisplay::clearDisplay() {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_DISPLAY, "Clearing");
    #endif
    this->lcd->clear();
}

void CSDisplay::startDisplayBombCodeAnimation() {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_DISPLAY, "Start bomb code anim");
    #endif
    this->currentAnimation = ANIMATION_LCD_BOMB_CODE;
    this->currentAnimationState = 0;
    this->clearBombCode();
    this->lastTime = millis();
}

void CSDisplay::startPlantedAnimation() {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_DISPLAY, "Start planted anim");
    #endif
    this->currentAnimation = ANIMATION_LCD_PLANTED;
    this->currentAnimationState = 0;
    this->lastTime = millis();
}

void CSDisplay::startDefusingAnimation(bool hasDefuseKit) {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_DISPLAY, "Start bomb defuse anim (defuse kit: ");
        Serial.print(hasDefuseKit ? "YES" : "NO");
        Serial.println(")");
    #endif
    this->currentAnimation = ANIMATION_LCD_DEFUSING;
    this->currentAnimationState = 0;
    this->lastTime = millis();
    this->hasDefuseKit = hasDefuseKit;
}

void CSDisplay::stopAnimation() {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_DISPLAY, "Stopping anim");
    #endif

    this->clearDisplay();
    this->currentAnimation = ANIMATION_LCD_NONE;
    this->currentAnimationState = 0;
    this->animationPlantedCountUp = true;
    this->hasDefuseKit = false;
    this->lastTime = 0;
    this->currentDefusingCharIndex = 0;
    this->defuseBeginTime = 0;
    this->DefusedAnimationShowChars = false;
}

void CSDisplay::tick() {
    switch (this->currentAnimation) {
        case ANIMATION_LCD_BOMB_CODE:
            this->tickBombCodeAnimation();
            break;
        case ANIMATION_LCD_PLANTED:
            this->tickPlantedAnimation();
            break;
        case ANIMATION_LCD_DEFUSING:
            this->tickDefusingAnimation();
            break;
        default:
            break;
    }
}

bool CSDisplay::addToBombCode(char c) {
    // shift all characters to the left
    for (int i = 0; i < BOMB_CODE_LENGTH - 1; i++) {
        this->bombCode[i] = this->bombCode[i + 1];
    }
    this->bombCode[BOMB_CODE_LENGTH - 1] = c;
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_DISPLAY, "Bomb code: ");
        for (int i = 0; i < BOMB_CODE_LENGTH; i++) {
             Serial.print(this->bombCode[i]);
        }
        Serial.println();
    #endif

    if (this->bombCode[0] != '*') {
        return true;
    }
    return false;
}

void CSDisplay::clearBombCode() {
    for (int i = 0; i < BOMB_CODE_LENGTH; i++) {
        this->bombCode[i] = '*';
    }
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_DISPLAY, "Bomb code: ");
        for (int i = 0; i < BOMB_CODE_LENGTH; i++) {
            Serial.print(this->bombCode[i]);
        }
        Serial.println();
    #endif
}

void CSDisplay::tickBombCodeAnimation() {
    this->lcd->setCursor(0, 1);
    char currentChar;
    for (int i = BOMB_CODE_LENGTH - 1; i >= 0; i--) {
        this->printCharToDisplay(this->bombCode[i]);
    }
}

void CSDisplay::tickPlantedAnimation() {
    // use the currentAnimationState to determine the position of the '*'
    this->lcd->setCursor(this->currentAnimationState, 1);
    this->lcd->print('*');

    // if the time has passed
    if ((this->lastTime + PLANTED_ANIM_DURATION_PER_STATE) <= millis()) {
        this->lastTime = millis();

        // remove the '*' from the previous position
        this->lcd->setCursor(this->currentAnimationState, 1);
        this->lcd->print(' ');

        // update the currentAnimationState / position of the '*'
        if (this->animationPlantedCountUp) {
            this->currentAnimationState++;
        } else {
            this->currentAnimationState--;
        }

        // if the '*' has reached the end of the line, change the direction for the next tick
        if (this->currentAnimationState == this->displayCharsPerLine - 1) {
            this->animationPlantedCountUp = false;
        } else if (this->currentAnimationState == 0) {
            this->animationPlantedCountUp = true;
        }
    }
}

void CSDisplay::tickDefusingAnimation() {
    float defusingPercentage = (float(millis() - this->defuseBeginTime) / float(this->hasDefuseKit ? 5000 : 10000)) * 100;

    switch (this->currentAnimationState) {
        case 0: // print "*******" to the display
            this->lcd->setCursor(0, 1);
            for (int i = 0; i < BOMB_CODE_LENGTH; i++) {
                this->lcd->print("*");
            }
            this->currentAnimationState = 1;
            this->defuseBeginTime = millis();
            this->lastTime = millis();
            this->currentDefusingCharIndex = 0;
            this->lcd->setCursor(BOMB_CODE_LENGTH - 1 - this->currentDefusingCharIndex, 1);
            break;
        case 1: // animate the currentDefusingCharIndex char
            if (this->currentDefusingCharIndex < int(defusingPercentage / 14.2857) % BOMB_CODE_LENGTH) { // 100% / 7 chars = 14.2857%
                this->printCharToDisplay(this->bombCode[this->currentDefusingCharIndex]);
                this->currentDefusingCharIndex++;
                this->lcd->setCursor(BOMB_CODE_LENGTH - 1 - this->currentDefusingCharIndex, 1);
            }

            if ((this->lastTime + DEFUSING_CHAR_ANIM_DURATION) <= millis()) {
                this->lastTime = millis();
                this->printCharToDisplay(char('0' + random(0, 10)));
                this->lcd->setCursor(BOMB_CODE_LENGTH - 1 - this->currentDefusingCharIndex, 1);
            }

            if (defusingPercentage >= 100) {
                this->lastTime = millis();
                this->currentAnimationState = 2;
            }
            break;
        case 2: // defusing finished. Flash the bomb code (show "*******" and bombcode for DEFUSING_ANIM_DURATION ms each)
            if ((this->lastTime + DEFUSING_ANIM_DURATION) <= millis()) {
                this->lastTime = millis();
                this->lcd->setCursor(0, 1);
                if (this->DefusedAnimationShowChars) {
                    for (int i = 0; i < BOMB_CODE_LENGTH; i++) {
                        this->lcd->print("*");
                    }
                    this->DefusedAnimationShowChars = false;
                } else {
                    for (int i = BOMB_CODE_LENGTH - 1; i >= 0; i--) {
                        this->printCharToDisplay(this->bombCode[i]);
                    }
                    this->DefusedAnimationShowChars = true;
                }
            }
    }
}

void CSDisplay::printCharToDisplay(char c) {
    switch (c) {  // *, 0 and 8 stay the same. 6 and 9 need to be switched
        case '1':
            this->lcd->write(FLIPPED_CHAR_1);
            break;
        case '2':
            this->lcd->write(FLIPPED_CHAR_2);
            break;
        case '3':
            this->lcd->write(FLIPPED_CHAR_3);
            break;
        case '4':
            this->lcd->write(FLIPPED_CHAR_4);
            break;
        case '5':
            this->lcd->write(FLIPPED_CHAR_5);
            break;
        case '6':
            this->lcd->write('9');
            break;
        case '7':
            this->lcd->write(FLIPPED_CHAR_7);
            break;
        case '9':
            this->lcd->write('6');
            break;
        default:
            this->lcd->print(c);
            break;
    }
}
