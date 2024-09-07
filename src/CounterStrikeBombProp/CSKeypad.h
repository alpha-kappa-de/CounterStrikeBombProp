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

#ifndef COUNTERSTRIKEBOMBPROP_CSKEYPAD_H
#define COUNTERSTRIKEBOMBPROP_CSKEYPAD_H

#include "../CounterStrikeBombProp.h"
#include <Keypad.h>


/**
 * @brief This class is responsible for managing the keypad of the Counter Strike Bomb Prop. It has an internal queue where the chars of the pressed
 *        keys are stored.
 * @details The queue works as a FIFO queue, so the first key that was pressed is the first to be dequeued. <br><br>
 *          Before using an object of this class, make sure to call the setup() method. <br><br>
 *          <h1>Methods</h1> <br>
 *          <b>setup():</b> Setup the keypad with the given parameters. <br><br>
 *          <b>tick():</b> If a button is pressed, it will be added to the queue (if allowedToEnqueue is true). <br><br>
 *          <b>getButton():</b> Get the next button that was pressed from the queue. <br><br>
 *          <b>hasButton():</b> Check if there is a button in the queue. <br><br>
 *          <b>resetQueue():</b> Reset the queue so it is empty. <br><br>
 *          <b>setAllowedToEnqueue():</b> Set the flag that indicates if it's allowed to enqueue a key or not. <br><br>
 *          <b>isAnyKeyPressed():</b> Checks if there is any key pressed or held down. <br><br>
 * @see Keypad
 */
class CSKeypad {
private:
    bool allowedToEnqueue; // A flag to indicate if it's allowed to enqueue a key
    Keypad* keypad = nullptr; // the Keypad object
    byte numRows; // the number of rows on the keypad
    byte numCols; // the number of columns on the keypad
    byte* pinsRows = nullptr; // the Arduino pins for the rows as an array ({R0, R1, R2, ...})
    byte* pinsCols = nullptr; // the Arduino pins for the columns as an array ({C0, C1, C2, ...})
    char* keys = nullptr; // the keys on the keypad as a 2D array ({{'1', '2', '3'}, {'4', '5', '6'}, ...})
    char* keyQueue; // the queue where the keys are stored
    byte keyQueueSize; // the size of the queue
    byte enqueuedKeys; // the amount of keys in the queue
    byte queueReturnIndex; // the index of the next key to be dequeued
    byte queueInsertIndex; // the index of the next key to be enqueued

    /**
     * @brief Enqueue a key to the queue
     *
     * @param key The key to enqueue
     * @return true if the key was enqueued, otherwise false
     */
    bool enqueue(char key);

    /**
     * @brief Dequeue a key from the queue
     *
     * @return The char of the key that was dequeued or the NOBUTTON constant from the config if the queue is empty
     */
    char dequeue();

public:
    /**
     * @brief Constructor for the CSKeypad class.
     */
    CSKeypad();

    /**
     * @brief Destructor for the CSKeypad class.
     */
    ~CSKeypad();

    /**
     * @brief Setup the keypad with the given parameters
     *
     * @param numRows The number of rows on the keypad
     * @param numCols The number of columns on the keypad
     * @param pinsRows The Arduino pins for the rows as an array ({R0, R1, R2, ...})
     * @param pinsCols The Arduino pins for the columns as an array ({C0, C1, C2, ...})
     * @param keys The keys on the keypad as a 2D array ({{'1', '2', '3'}, {'4', '5', '6'}, ...})
     * @param keyQueueSize The size of the queue
     */
    void setup(byte numRows, byte numCols, byte* pinsRows, byte* pinsCols, char* keys, byte keyQueueSize);

    /**
     * @brief If a button is pressed, it will be added to the queue (if allowedToEnqueue is true)
     */
    void tick();

    /**
     * @brief Get the next button that was pressed from the queue
     *
     * @return The button that was pressed
     */
    char getButton();

    /**
     * @brief Check if there is a button in the queue
     *
     * @return true if there is a button in the queue, otherwise false
     */
    bool hasButton();

    /**
     * @brief Reset the queue so it is empty
     */
    void resetQueue();

    /**
     * @brief Set the flag that indicates if it's allowed to enqueue a key or not
     */
    void setAllowedToEnqueue(bool allowedToEnqueue);

    /**
     * @brief Checks if there is any key pressed or held down
     *
     * @return true if there is any key pressed/hold, otherwise false
     */
    bool isAnyKeyPressed();
};

#endif