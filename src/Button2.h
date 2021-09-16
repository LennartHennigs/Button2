/////////////////////////////////////////////////////////////////
/*
  Button2.h - Arduino Library to simplify working with buttons.
  Created by Lennart Hennigs.
*/
/////////////////////////////////////////////////////////////////
#pragma once

#ifndef Button2_h
#define Button2_h

/////////////////////////////////////////////////////////////////
#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
  #include <functional>
#endif
#include "Arduino.h"

/////////////////////////////////////////////////////////////////

#define DEBOUNCE_MS 50
#define LONGCLICK_MS 200
#define DOUBLECLICK_MS 300
#define CAPACITIVE_TOUCH_THRESHOLD 35

#define SINGLE_CLICK 1
#define DOUBLE_CLICK 2
#define TRIPLE_CLICK 3
#define LONG_CLICK 4

#define UNDEFINED_PIN 255

/////////////////////////////////////////////////////////////////

class Button2 {
protected:
  byte pin;
  byte state;
  byte prev_state;
  byte click_count = 0;
  byte last_click_type = 0;
  byte _pressedState;
  bool is_capacitive = false;
  unsigned long click_ms;
  unsigned long down_ms;

  bool longclick_detected_retriggerable;
  uint16_t longclick_detected_counter;
  bool longclick_detected = false;
  bool longclick_detected_reported = false;
  
  unsigned int debounce_time_ms;
  unsigned int longclick_time_ms;
  unsigned int doubleclick_time_ms;
  
  unsigned int down_time_ms = 0;
  bool pressed_triggered = false;

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
  typedef std::function<void(Button2 &btn)> CallbackFunction;
#else
  typedef void (*CallbackFunction)(Button2 &);
#endif

  CallbackFunction pressed_cb = NULL;
  CallbackFunction released_cb = NULL;
  CallbackFunction change_cb = NULL;
  CallbackFunction tap_cb = NULL;
  CallbackFunction click_cb = NULL;
  CallbackFunction long_cb = NULL;
  CallbackFunction double_cb = NULL;
  CallbackFunction triple_cb = NULL;
  CallbackFunction longclick_detected_cb = NULL;

public:
  Button2();
  Button2(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean isCapacitive = false, boolean activeLow = true);

  void begin(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean isCapacitive = false , boolean activeLow  = true);

  void setDebounceTime(unsigned int ms);
  void setLongClickTime(unsigned int ms);
  void setDoubleClickTime(unsigned int ms);
  
  unsigned int getDebounceTime() const;
  unsigned int getLongClickTime() const;
  unsigned int getDoubleClickTime() const;
  byte getAttachPin() const;

  void setLongClickDetectedRetriggerable(bool retriggerable);

  void reset();

  void setChangedHandler(CallbackFunction f);
  void setPressedHandler(CallbackFunction f);
  void setReleasedHandler(CallbackFunction f);

  void setTapHandler(CallbackFunction f);
  void setClickHandler(CallbackFunction f);
  void setDoubleClickHandler(CallbackFunction f);
  void setTripleClickHandler(CallbackFunction f);

  void setLongClickHandler(CallbackFunction f);
  void setLongClickDetectedHandler(CallbackFunction f);

  unsigned int wasPressedFor() const;
  boolean isPressed() const;
  boolean isPressedRaw() const;

  byte getNumberOfClicks() const;
  byte getClickType() const;

  bool operator == (Button2 &rhs);

  void loop();

private: 
  byte _getState();

};
/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
