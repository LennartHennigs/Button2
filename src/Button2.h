/////////////////////////////////////////////////////////////////
/*
  Button2.h - Arduino Library to simplify working with buttons.
  Copyright (C) 2017-2025 Lennart Hennigs.
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
  uint8_t pin;
  uint8_t state;
  uint8_t prev_state;
  uint8_t click_count = 0;
  uint8_t last_click_count = 0;
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
  typedef std::function<uint8_t()> StateCallbackFunction;
#else
  typedef void (*CallbackFunction)(Button2 &);
  typedef uint8_t (*StateCallbackFunction)();
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
  Button2(uint8_t attachTo, uint8_t buttonMode = INPUT_PULLUP, bool activeLow = true, Hardware* hardware = new ArduinoHardware());

  void begin(uint8_t attachTo, uint8_t buttonMode = INPUT_PULLUP, bool activeLow = true, Hardware* hardware = new ArduinoHardware());

  void setDebounceTime(unsigned int ms);
  void setLongClickTime(unsigned int ms);
  void setDoubleClickTime(unsigned int ms);

  unsigned int getDebounceTime() const;
  unsigned int getLongClickTime() const;
  unsigned int getDoubleClickTime() const;
  uint8_t getPin() const;

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
  bool isPressed() const;
  bool isPressedRaw() const;
  void resetPressedState();
  uint8_t resetClickCount();

  bool wasPressed() const;
  clickType read(bool keepState = false);
  clickType wait(bool keepState = false);
  void waitForClick(bool keepState = false);
  void waitForDouble(bool keepState = false);
  void waitForTriple(bool keepState = false);
  void waitForLong(bool keepState = false);

  uint8_t getNumberOfClicks() const;
  uint8_t getLongClickCount() const;

  clickType getType() const;
  String clickToString(clickType type) const;

  int getID() const;
  void setID(int newID);

  bool operator==(Button2 &rhs);

  void loop();

 private:
  static int _nextID;
  uint8_t _pressedState;
  uint8_t _getState() const;
  Hardware* hw;

};
/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
