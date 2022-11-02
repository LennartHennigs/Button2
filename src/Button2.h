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
// uncommenting one of these compiler switches will remove the 
//  code for one of these click types

// #define _IGNORE_DOUBLE
// #define _IGNORE_TRIPLE
// #define _IGNORE_LONG

/////////////////////////////////////////////////////////////////

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
  #include <functional>
#endif
#include <Arduino.h>

/////////////////////////////////////////////////////////////////

#ifdef _IGNORE_DOUBLE
  #pragma message "Not compiling double click code"
#endif
#ifdef _IGNORE_TRIPLE
  #pragma message "Not compiling triple click code"
#endif
#ifdef _IGNORE_LONG
  #pragma message "Not compiling long click code"
#endif

/////////////////////////////////////////////////////////////////

#define DEBOUNCE_MS 50
#define LONGCLICK_MS 200
#define DOUBLECLICK_MS 300

#define UNDEFINED_PIN 255
#define VIRTUAL_PIN 254

/////////////////////////////////////////////////////////////////

enum clickType {
  single_click,
#ifndef _IGNORE_DOUBLE
  double_click,
#endif
#ifndef _IGNORE_TRIPLE
  triple_click,
#endif
#ifndef _IGNORE_LONG
  long_click,
#endif
  empty
};

class Button2 {

protected:
  int id;
  byte pin;
  byte state;
  byte prev_state;
  byte click_count = 0;
  clickType last_click_type = empty;
  bool was_pressed = false;
  unsigned long click_ms;
  unsigned long down_ms;

#ifndef _IGNORE_LONG
  bool longclick_detected_retriggerable;
  uint16_t longclick_detected_counter = 0;
  bool longclick_detected = false;
  bool longclick_detected_reported = false;
#endif
  
  unsigned int debounce_time_ms = DEBOUNCE_MS;
  unsigned int longclick_time_ms = LONGCLICK_MS;
  unsigned int doubleclick_time_ms = DOUBLECLICK_MS;
  
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
#ifndef _IGNORE_LONG
  CallbackFunction long_cb = NULL;
  CallbackFunction longclick_detected_cb = NULL;
#endif
#ifndef _IGNORE_DOUBLE
  CallbackFunction double_cb = NULL;
#endif
#ifndef _IGNORE_TRIPLE
  CallbackFunction triple_cb = NULL;
#endif

public:
  Button2();
  Button2(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean activeLow = true);

  void begin(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean activeLow  = true);

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
#ifndef _IGNORE_DOUBLE 
  void setDoubleClickHandler(CallbackFunction f);
#endif
#ifndef _IGNORE_TRIPLE
  void setTripleClickHandler(CallbackFunction f);
#endif
#ifndef _IGNORE_LONG
  void setLongClickHandler(CallbackFunction f);
  void setLongClickDetectedHandler(CallbackFunction f);
  void setLongClickDetectedRetriggerable(bool retriggerable);
#endif
  unsigned int wasPressedFor() const;
  boolean isPressed() const;
  boolean isPressedRaw();

  bool wasPressed() const;
  clickType read(bool keepState = false);
  clickType wait(bool keepState = false);
  void waitForClick(bool keepState = false);
#ifndef _IGNORE_DOUBLE
  void waitForDouble(bool keepState = false);
#endif
#ifndef _IGNORE_TRIPLE
  void waitForTriple(bool keepState = false);
#endif
#ifndef _IGNORE_LONG
  void waitForLong(bool keepState = false);
#endif
  byte getNumberOfClicks() const;
  clickType getType() const;
  String clickToString(clickType type) const;

  int getID() const;
  void setID(int newID);

  bool operator == (Button2 &rhs);

  void loop();

private: 
  static int _nextID;
  byte _pressedState;

  byte _getState() const;
};
/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
