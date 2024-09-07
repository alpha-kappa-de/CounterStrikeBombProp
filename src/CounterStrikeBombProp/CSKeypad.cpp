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

#include "CSKeypad.h"

bool CSKeypad::enqueue(char key) {
    if (this->enqueuedKeys == this->keyQueueSize) {
        // Queue is full, cannot insert
        #if defined(SERIAL_DEBUG)
            DebugPrintln(PREFIX_KEYPAD, "Queue is full, cannot insert key");
        #endif
        return false;
    }

    // Insert the key and increment the insert index, wrapping if necessary
    this->keyQueue[this->queueInsertIndex] = key;
    this->enqueuedKeys++;
    this->queueInsertIndex = (this->queueInsertIndex + 1) % this->keyQueueSize;
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_KEYPAD, "Enqueued key: \"");
        Serial.print(key);
        Serial.print("\" (Int: ");
        Serial.print((int)key);
        Serial.println(")");
    #endif
    return true;
}

char CSKeypad::dequeue() {
    if (this->enqueuedKeys == 0) {
        // Queue is empty, return no button
        #if defined(SERIAL_DEBUG)
            DebugPrintln(PREFIX_KEYPAD, "Queue is empty, cannot obtain key");
        #endif
        return NO_KEY;
    }

    // Retrieve the key and increment the return index, wrapping if necessary
    char key = this->keyQueue[this->queueReturnIndex];
    this->enqueuedKeys--;
    this->queueReturnIndex = (this->queueReturnIndex + 1) % this->keyQueueSize;
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_KEYPAD, "Dequeued key: \"");
        Serial.print(key);
        Serial.print("\" (Int: ");
        Serial.print((int)key);
        Serial.println(")");
    #endif
    return key;
}

CSKeypad::CSKeypad() { }

CSKeypad::~CSKeypad() {
    delete this->keypad;
    delete[] this->keyQueue;
}

void CSKeypad::setup(byte numRows, byte numCols, byte* pinsRows, byte* pinsCols, char* keys, byte keyQueueSize) {
    this->allowedToEnqueue = true;
    this->keyQueueSize = keyQueueSize;
    this->keyQueue = new char[keyQueueSize];
    this->resetQueue();

    this->numRows = numRows;
    this->numCols = numCols;
    this->pinsRows = pinsRows;
    this->pinsCols = pinsCols;
    this->keys = keys;
    this->keypad = new Keypad(makeKeymap(this->keys), this->pinsRows, this->pinsCols, this->numRows, this->numCols);

    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_KEYPAD, STR_INITIALIZED);
    #endif
}



void CSKeypad::tick() {
    if (!this->allowedToEnqueue) {
        return; // Do not enqueue keys if not allowed
    }
    char key = this->keypad->getKey();
    if (key) {
        this->enqueue(key);
    }
}

char CSKeypad::getButton() {
    if (this->enqueuedKeys > 0) {
        char key;
        key = this->dequeue();
        return key;
    }
    return NO_KEY;
}

bool CSKeypad::hasButton() {
    return this->enqueuedKeys > 0;
}

void CSKeypad::resetQueue() {
    #if defined(SERIAL_DEBUG)
        DebugPrintln(PREFIX_KEYPAD, "Resetting queue");
    #endif
    this->enqueuedKeys = 0;
    this->queueReturnIndex = 0;
    this->queueInsertIndex = 0;
}

void CSKeypad::setAllowedToEnqueue(bool allowedToEnqueue) {
    #if defined(SERIAL_DEBUG)
        DebugPrint(PREFIX_KEYPAD, "Allowed to enqueue set to ");
        if (allowedToEnqueue) {
            Serial.println("TRUE");
        } else {
            Serial.println("FALSE");
        }
    #endif
    this->allowedToEnqueue = allowedToEnqueue;
}

bool CSKeypad::isAnyKeyPressed() {
    // getState() returns a KeyState enum (IDLE, PRESSED, HOLD, RELEASED)
    return this->keypad->getState();
}
