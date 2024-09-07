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

#ifndef COUNTERSTRIKEBOMBPROP_CSYELLOWLED_H
#define COUNTERSTRIKEBOMBPROP_CSYELLOWLED_H

#include "../CounterStrikeBombProp.h"


/**
 * @brief This class is responsible for managing the yellow LED of the Counter Strike Bomb Prop.
 * @details The animation is written as a state machine. The animation will fade in for a specified time to the
 *          specified brightness, stay at that brightness for a specified time and then fade out to the minimum brightness. The
 *          animation will loop until stopAnimation() is called. (or not looped, when the parameter is set to do so) <br><br>
 *          Before using an object of this class, make sure to call the setup() method. <br><br>
 *          <h1>Methods</h1> <br>
 *          <b>setup():</b> Setup the yellow LED with the given parameters. <br>
 *          <b>activateAnimation():</b> Activate the animation so that it plays when the tick() method is called <br>
 *          <b>stopAnimation():</b> Stops the animation by resetting all fields. The LED will be turned off <br>
 *          <b>tick():</b> A state machine to handle the fading animation. Should be called in the loop(). Plays a tick
 *          of the animation <br>
 */
class CSYellowLED {
private:
    byte ledPin; // The pin the LED is connected to

    // Animation fields
    bool animationActive; // A flag to indicate if the animation is active
    byte currentAnimationState; // The current state of the animation used for the state machine
    unsigned long lastTime; // The last time for non-blocking animation delays
    byte minBrightness; // The minimum brightness of the LED
    byte maxBrightness; // The maximum brightness of the LED
    byte currentBrightness; // The current brightness of the LED
    byte stepInSize; // The increase step of the brightness when fading in
    byte stepOutSize; // The decrease step of the brightness when fading out
    int cycleInMillis; // The time in milliseconds the LED will take to fade in
    int cycleOutMillis; // The time in milliseconds the LED will take to fade out
    int delayMillis; // The time in milliseconds the LED will stay at the min or max brightness before fading out or in
    bool loopAnimation; // A flag to indicate if the animation should loop (bomb idle) or not (bomb exploding)
    int startDelayMillis; // The time in milliseconds to wait before starting the animation

    /**
     * @brief Resets all fields of the YellowLED
     */
    void resetFields();

public:
    /**
     * @brief Constructor for the CSYellowLED class.
     */
    CSYellowLED();

    /**
     * @brief Destructor for the CSYellowLED class.
     */
    ~CSYellowLED();

    /**
     * @brief Initializes the CSYellowLED class by setting the pin mode of the LED pin to OUTPUT.
     *
     * @param ledPin The pin the LED is connected to
     */
    void setup(byte ledPin);

    /**
     * @brief Set the animation to fade so that this animation plays when the tick() method is called
     *
     * @param minBrightness The minimum brightness of the LED
     * @param maxBrightness The maximum brightness of the LED
     * @param stepInSize The increase step of the brightness when fading in
     * @param stepOutSize The decrease step of the brightness when fading out
     * @param fadeInMillis The time in milliseconds the LED will take to fade in
     * @param fadeOutMillis The time in milliseconds the LED will take to fade out
     * @param delayMillis The time in milliseconds the LED will stay at the min or max brightness before fading out or in
     * @param loopAnimation A flag to indicate if the animation should loop (bomb idle) or not (bomb exploding)
     * @param startDelayMillis The time in milliseconds to wait before starting the animation
     */
    void activateAnimation(byte minBrightness,
                           byte maxBrightness,
                           byte stepInSize,
                           byte stepOutSize,
                           int fadeInMillis,
                           int fadeOutMillis,
                           int delayMillis,
                           bool loopAnimation = true,
                           int startDelayMillis = 0
                           );

    /**
     * @brief Stops the animation by resetting all fields. The LED will be turned off
     */
    void stopAnimation();

    /**
     * @brief A state machine to handle the fading animation. Should be called in the loop(). Plays a tick of the animation
     */
    void tick();
};

#endif
