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

#ifndef COUNTERSTRIKEBOMBPROP_CSSWITCH_H
#define COUNTERSTRIKEBOMBPROP_CSSWITCH_H

#include "../CounterStrikeBombProp.h"


/**
 * @brief This class is responsible for managing the switch of the Counter Strike Bomb Prop.
 * @details The switch is debounced with a specified time. The tick() method reads the current state of the switch (if
 *          the allowUpdate flag is set) and return the currentState as a boolean. If the allowUpdate flag is set to
 *          false, the state of the switch will not be updated and tick() will return the last state that was set,
 *          before the flag was set to false. <br><br>
 *          Before using an object of this class, make sure to call the setup() method. <br><br>
 *
 * <h1>Methods</h1><br>
 * <b>tick():</b> Returns the current state of the switch debounced (if allowed to update), else returns the last state <br>
 * <b>setAllowUpdate():</b> Sets the flag to allow or disallow the update of the switch state. <br>
 */
class CSSwitch {
private:
    byte switchPin; // The pin the switch is connected to
    byte switchOnState; // The state (HIGH or LOW) in which the switch should be considered ON
    byte switchDebounceTime; // The time in milliseconds the switch should be debounced
    bool allowUpdate; // A flag to indicate if the state of the switch should be updated

    bool currentState; // The current state of the switch
    unsigned long lastMillis; // The last time the current state was set


public:
    /**
     * @brief Constructor for the CSSwitch class.
     */
    CSSwitch();

    /**
     * @brief Destructor for the CSSwitch class.
     */
    ~CSSwitch();

    /**
     * @brief Initializes the CSSwitch class by setting the pin mode of the switch pin to INPUT and setting the
     *        currentState to false. The allowUpdate flag is set to true.
     * @param switchPin The pin the switch is connected to
     * @param switchOnState The state (HIGH or LOW) in which the switch should be considered ON
     * @param switchDebounceTime The time in milliseconds the switch should be debounced
     */
    void setup(byte switchPin, byte switchOnState, byte switchDebounceTime);

    /**
     * @brief Gets the current state of the switch debounced with the specified time in the config if the allowUpdate
     *        flag is set to true and returns the current state. If the allowUpdate flag is set to false, the state of
     *        the switch will not be updated and the last state before the flag was set to false will be returned.
     *
     * @return The current state of the switch if it's allowed to update, otherwise the last state.
     */
    bool tick();

    /*
     * @brief Sets the flag to allow or disallow the update of the switch state.
     */
    void setAllowUpdate(bool allowUpdate);
};


#endif
