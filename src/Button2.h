/////////////////////////////////////////////////////////////////
/*
  Button2.h - Arduino Library to simplify working with buttons.
  Copyright (C) 2017-2024 Lennart Hennigs.
  Released under the MIT license.

*/
/////////////////////////////////////////////////////////////////

#pragma once

#ifndef Button2_h
#define Button2_h

/////////////////////////////////////////////////////////////////

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
#include <functional>
#endif
#include <Arduino.h>

#include "Hardware.h"

/////////////////////////////////////////////////////////////////

const unsigned int BTN_DEBOUNCE_MS = 50;
const unsigned int BTN_LONGCLICK_MS = 200;
const unsigned int BTN_DOUBLECLICK_MS = 300;

const unsigned int BTN_UNDEFINED_PIN = 255;
const unsigned int BTN_VIRTUAL_PIN = 254;

/////////////////////////////////////////////////////////////////

enum clickType {
  single_click,
  double_click,
  triple_click,
  long_click,
  empty
};

class Button2 {
 protected:
  int id;
  byte pin;
  byte state;
  byte prev_state;
  byte click_count = 0;
  byte last_click_count = 0;
  clickType last_click_type = empty;
  bool was_pressed = false;
  unsigned long click_ms;
  unsigned long down_ms;

  bool longclick_retriggerable;
  uint16_t longclick_counter = 0;
  bool longclick_detected = false;
  bool longclick_reported = false;

  unsigned int debounce_time_ms = BTN_DEBOUNCE_MS;
  unsigned int longclick_time_ms = BTN_LONGCLICK_MS;
  unsigned int doubleclick_time_ms = BTN_DOUBLECLICK_MS;

  unsigned int down_time_ms = 0;
  bool pressed_triggered = false;

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
  typedef std::function<void(Button2 &btn)> CallbackFunction;
  typedef std::function<byte()> StateCallbackFunction;
#else
  typedef void (*CallbackFunction)(Button2 &);
  typedef byte (*StateCallbackFunction)();
#endif

  StateCallbackFunction get_state_cb = NULL;

  CallbackFunction pressed_cb = NULL;
  CallbackFunction released_cb = NULL;
  CallbackFunction change_cb = NULL;
  CallbackFunction tap_cb = NULL;
  CallbackFunction click_cb = NULL;
  CallbackFunction long_cb = NULL;
  CallbackFunction longclick_detected_cb = NULL;
  CallbackFunction double_cb = NULL;
  CallbackFunction triple_cb = NULL;

  void _handlePress(long now);
  void _handleRelease(long now);
  void _releasedNow(long now);
  void _pressedNow(long now);
  void _validKeypress();
  void _checkForLongClick(long now);
  void _reportClicks();
  void _setID();

 public:
  Button2();
  Button2(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean activeLow = true, Hardware* hardware = new ArduinoHardware());

  void begin(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean activeLow = true, Hardware* hardware = new ArduinoHardware());

  void setDebounceTime(unsigned int ms);
  void setLongClickTime(unsigned int ms);
  void setDoubleClickTime(unsigned int ms);

  unsigned int getDebounceTime() const;
  unsigned int getLongClickTime() const;
  unsigned int getDoubleClickTime() const;
  byte getPin() const;

  void reset();

  void setButtonStateFunction(StateCallbackFunction f);

  void setChangedHandler(CallbackFunction f);
  void setPressedHandler(CallbackFunction f);
  void setReleasedHandler(CallbackFunction f);

  void setTapHandler(CallbackFunction f);
  void setClickHandler(CallbackFunction f);
  void setDoubleClickHandler(CallbackFunction f);
  void setTripleClickHandler(CallbackFunction f);

  void setLongClickHandler(CallbackFunction f);
  void setLongClickDetectedHandler(CallbackFunction f);

  void setLongClickDetectedRetriggerable(bool retriggerable);

  unsigned int wasPressedFor() const;
  boolean isPressed() const;
  boolean isPressedRaw() const;
  void resetPressedState();
  byte resetClickCount();

  bool wasPressed() const;
  clickType read(bool keepState = false);
  clickType wait(bool keepState = false);
  void waitForClick(bool keepState = false);
  void waitForDouble(bool keepState = false);
  void waitForTriple(bool keepState = false);
  void waitForLong(bool keepState = false);

  byte getNumberOfClicks() const;
  byte getLongClickCount() const;

  clickType getType() const;
  String clickToString(clickType type) const;

  int getID() const;
  void setID(int newID);

  bool operator==(Button2 &rhs);

  void loop();

 private:
  static int _nextID;
  byte _pressedState;
  byte _getState() const;
  Hardware* hw;

};
/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
