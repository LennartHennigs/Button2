/////////////////////////////////////////////////////////////////
/*
  Hardware.h - Implements Arduino Hardware Abstraction Layer.
  Part or Button2 library for Arduino.
  Copyright (C) 2017-2024 Lennart Hennigs.
  Released under the MIT license.
*/
/////////////////////////////////////////////////////////////////
#include <Arduino.h>
/////////////////////////////////////////////////////////////////

#pragma once

#ifndef AbstractHardware_h
#define AbstractHardware_h

/////////////////////////////////////////////////////////////////
// disable warnings for unused parameters

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/////////////////////////////////////////////////////////////////
 // virtual class for hardware abstraction

class Hardware {
public:
    virtual int digitalRead(int pin) = 0;
    virtual void pinMode(int pin, int mode) = 0;
    virtual void digitalWrite(int pin, int value) = 0;
};

/////////////////////////////////////////////////////////////////
// implementation for Arduino

// for Arduino API version 2.0 – used by UNO R4, RP2040, ...
#if defined(ARDUINO_API_VERSION)
class ArduinoHardware : public Hardware {
public:
    int digitalRead(int pin) {
        return ::digitalRead(pin);
    }

    void pinMode(int pin, int mode) {
        ::pinMode(pin, static_cast<PinMode>(mode));
    }

    void digitalWrite(int pin, int value) {
        ::digitalWrite(pin, static_cast<PinStatus>(value));
    }
};
#else
class ArduinoHardware : public Hardware {
public:
    int digitalRead(int pin) {
        
        return ::digitalRead(pin);
    }
    void pinMode(int pin, int mode) {
        ::pinMode(pin, mode);
    }
    void digitalWrite(int pin, int value) {
        ::digitalWrite(pin, value);
    }
};
#endif

/////////////////////////////////////////////////////////////////
// implementation for testing

class MockHardware : public Hardware {
private:
    int pin_state[255];
    int pin_mode[255];

public:
    MockHardware() {
        for(int i = 0; i < 255; i++) {
            pin_state[i] = HIGH;
            pin_mode[i] = OUTPUT;
        }
    }
    
    int digitalRead(int pin) override {
        return pin_state[pin];
    }
    void pinMode(int pin, int mode) override {
       pin_mode[pin] = mode;
    }
    void digitalWrite(int pin, int value) override {
        pin_state[pin] = value;
    }
    int getPinMode(int pin) {
        return pin_mode[pin];
    }
};

/////////////////////////////////////////////////////////////////
#pragma GCC diagnostic pop
#endif
/////////////////////////////////////////////////////////////////
