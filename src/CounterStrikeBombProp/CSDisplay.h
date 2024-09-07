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

#ifndef COUNTERSTRIKEBOMBPROP_CSDISPLAY_H
#define COUNTERSTRIKEBOMBPROP_CSDISPLAY_H

#include "../CounterStrikeBombProp.h"
#include <LiquidCrystal_I2C.h>

#define PLANTED_ANIM_DURATION_PER_STATE 75
#define DEFUSING_CHAR_ANIM_DURATION 100
#define DEFUSING_ANIM_DURATION 500
#define BOMB_CODE_LENGTH 7
#define FLIPPED_CHAR_1 0
#define FLIPPED_CHAR_2 1
#define FLIPPED_CHAR_3 2
#define FLIPPED_CHAR_4 3
#define FLIPPED_CHAR_5 4
#define FLIPPED_CHAR_7 5

enum AnimationLCD {
    ANIMATION_LCD_NONE,
    ANIMATION_LCD_BOMB_CODE,
    ANIMATION_LCD_PLANTED,
    ANIMATION_LCD_DEFUSING
};

/**
 * @brief This class is responsible for managing the display of the Counter Strike Bomb Prop. It has some custom characters
 *        of the number chars, because the display is flipped. <br><br>
 * @details There are three animations, which can be displayed <br>
 *          - <b>Bomb code input:</b> The bomb code is being entered and the characters that are not entered yet are displayed as * <br>
 *          - <b>Planted:</b> The bomb is planted and the * character is moving from left to right <br>
 *          - <b>Defusing:</b> The bomb is being defused and the characters of the bomb code are being revealed one after another <br><br>
 *          Before using an object of this class, make sure to call the setup() method. <br><br>
 *          <h1>Methods</h1> <br>
 *          <b>setup():</b> Setup the keypad with the given parameters. <br><br>
 *          <b>tick():</b> If a button is pressed, it will be added to the queue (if allowedToEnqueue is true). <br><br>
 *          <b>getButton():</b> Get the next button that was pressed from the queue. <br><br>
 *          <b>hasButton():</b> Check if there is a button in the queue. <br><br>
 *          <b>resetQueue():</b> Reset the queue so it is empty. <br><br>
 *          <b>setAllowedToEnqueue():</b> Set the flag that indicates if it's allowed to enqueue a key or not. <br><br>
 *          <b>isAnyKeyPressed():</b> Checks if there is any key pressed or held down. <br><br>
 * @see LiquidCrystal_I2C
 */
class CSDisplay {
private:
    byte displayAddress;                // I2C address of the display
    byte displayCharsPerLine;           // Number of characters per line
    byte displayLines;                  // Number of lines
    bool displayBacklight;              // Backlight state
    char* bombCode;                     // Current bomb code

    // some custom characters (because the display is flipped)
    uint8_t flipped_1[8] = { 0x0E, 0x04, 0x04, 0x04, 0x04, 0x06, 0x04, 0x00 };
    uint8_t flipped_2[8] = { 0x1F, 0x02, 0x04, 0x08, 0x10, 0x11, 0x0E, 0x00 };
    uint8_t flipped_3[8] = { 0x0E, 0x11, 0x10, 0x08, 0x04, 0x08, 0x1F, 0x00 };
    uint8_t flipped_4[8] = { 0x08, 0x08, 0x1F, 0x09, 0x0A, 0x0C, 0x08, 0x00 };
    uint8_t flipped_5[8] = { 0x0E, 0x11, 0x10, 0x10, 0x0F, 0x01, 0x1F, 0x00 };
    uint8_t flipped_7[8] = { 0x02, 0x02, 0x02, 0x04, 0x08, 0x10, 0x1F, 0x00 };

    LiquidCrystal_I2C *lcd = nullptr;   // LiquidCrystal_I2C object for the display

    AnimationLCD currentAnimation;      // Current animation that should be displayed
    byte currentAnimationState;         // Current animation state
    bool animationPlantedCountUp;       // If the planted animation should count up or down (position of the * character)
    bool hasDefuseKit;                  // If the defuser has a defuse kit (fast or slow defuse animation)
    unsigned long lastTime;             // Last time the animation was updated
    unsigned long defuseBeginTime;      // Time the defusing animation started
    int currentDefusingCharIndex;       // Current character index of the defusing animation
    bool DefusedAnimationShowChars;     // If the defusing animation should show the actual characters or *******

    /**
     * @brief Play one tick of the bomb code animation
     */
    void tickBombCodeAnimation();

    /**
     * @brief Play one tick of the planted animation
     */
    void tickPlantedAnimation();

    /**
     * @brief Play one tick of the defusing animation
     */
    void tickDefusingAnimation();

    /**
     * @brief Print a character to the display at the current position. This function prints the character upside down,
     * but only if the character is one of these: 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, *
     *
     * @param c The character to print
     */
    void printCharToDisplay(char c);

public:
    /**
     * @brief Constructor for the CSDisplay class.
     */
    CSDisplay();

    /**
     * @brief Destructor for the CSDisplay class.
     */
    ~CSDisplay();

    /**
     * @brief Setup the display with the given parameters
     *
     * @param displayAddress The I2C address of the display
     * @param displayCharsPerLine The number of characters per line
     * @param displayLines The number of lines
     * @param displayBacklight The backlight state
     */
    void setup(byte displayAddress, byte displayCharsPerLine, byte displayLines, bool displayBacklight);

    /**
     * @brief Activate the backlight of the display
     */
    void activateBacklight();

    /**
     * @brief Deactivate the backlight of the display
     */
    void deactivateBacklight();

    /**
     * @brief Clear the display (doesn't reset the cursor)
     */
    void clearDisplay();

    /**
     * @brief Set the current animation to display the bomb code while it is being entered (not entered characters will be displayed as *)
     */
    void startDisplayBombCodeAnimation();

    /**
     * @brief Set the current animation to the planted animation (moving * character)
     */
    void startPlantedAnimation();

    /**
     * @brief Set the current animation to the defusing animation (display showing ******* and one after another the
     * characters of the bomb code will be revealed while the current character is showing a random character)
     */
    void startDefusingAnimation(bool hasDefuseKit);

    /**
     * @brief Stop the current animation
     */
    void stopAnimation();

    /**
     * @brief Play a tick of the current animation
     */
    void tick();

    /**
     * @brief Add a character to the bomb code
     *
     * @param c The character to add
     * @return True if the bomb code is complete, false otherwise
     */
    bool addToBombCode(char c);

    /**
     * @brief Clear the bomb code
     */
    void clearBombCode();
};
#endif