/////////////////////////////////////////////////////////////////

#pragma once

#ifndef Button2Hardware_h
#define Button2Hardware_h

/////////////////////////////////////////////////////////////////

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
