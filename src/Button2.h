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

#ifndef BUTTON2_HAS_STD_FUNCTION      // for user force enable std::function
#ifndef BUTTON2_DISABLE_STD_FUNCTION  // for user force disable std::function
#if __cplusplus >= 201103L && !defined(__AVR__)
#include <functional>
#include <utility>
#define BUTTON2_HAS_STD_FUNCTION 1
#endif
#endif
#endif

#include <Arduino.h>

// Define Arduino constants if not available (for testing environments)
#ifndef INPUT
#define INPUT 0x0
#endif
#ifndef OUTPUT
#define OUTPUT 0x1
#endif
#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif
#ifndef HIGH
#define HIGH 0x1
#endif
#ifndef LOW
#define LOW 0x0
#endif

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
  // Memory layout optimized for minimal padding
  // Ordered by size: pointers/callbacks first, then long, int, uint16_t, uint8_t, bool

#ifdef BUTTON2_HAS_STD_FUNCTION
  typedef std::function<void(Button2 &btn)> CallbackFunction;
  typedef std::function<uint8_t()> StateCallbackFunction;
  #define BUTTON2_MOVE(v) std::move(v)
  #define BUTTON2_NULL nullptr
#else
  typedef void (*CallbackFunction)(Button2 &);
  typedef uint8_t (*StateCallbackFunction)();
  #define BUTTON2_MOVE
  #define BUTTON2_NULL NULL
#endif

  // Function pointers (largest members on most platforms)
  StateCallbackFunction get_state_cb = BUTTON2_NULL;
  CallbackFunction pressed_cb = BUTTON2_NULL;
  CallbackFunction released_cb = BUTTON2_NULL;
  CallbackFunction change_cb = BUTTON2_NULL;
  CallbackFunction tap_cb = BUTTON2_NULL;
  CallbackFunction click_cb = BUTTON2_NULL;
  CallbackFunction long_cb = BUTTON2_NULL;
  CallbackFunction longclick_detected_cb = BUTTON2_NULL;
  CallbackFunction double_cb = BUTTON2_NULL;
  CallbackFunction triple_cb = BUTTON2_NULL;

  // unsigned long (4 bytes on most platforms)
  unsigned long click_ms = 0;
  unsigned long down_ms = 0;

  // unsigned int / uint16_t (2 bytes)
  unsigned int debounce_time_ms = BTN_DEBOUNCE_MS;
  unsigned int longclick_time_ms = BTN_LONGCLICK_MS;
  unsigned int doubleclick_time_ms = BTN_DOUBLECLICK_MS;
  unsigned int down_time_ms = 0;
  uint16_t longclick_counter = 0;

  // int (2-4 bytes depending on platform)
  int id;

  // uint8_t (1 byte each)
  uint8_t pin;
  uint8_t state = HIGH;
  uint8_t prev_state = HIGH;
  uint8_t click_count = 0;
  uint8_t last_click_count = 0;
  uint8_t _pressedState;

  // clickType (typically 1 byte enum)
  clickType last_click_type = clickType::empty;

  // bool (1 byte each, grouped at end)
  bool was_pressed = false;
  bool longclick_retriggerable;
  bool longclick_detected = false;
  bool longclick_reported = false;
  bool pressed_triggered = false;

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
  Button2(uint8_t attachTo, uint8_t buttonMode = INPUT_PULLUP, bool activeLow = true);

  void begin(uint8_t attachTo, uint8_t buttonMode = INPUT_PULLUP, bool activeLow = true);

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
  const char* clickToString(clickType type) const;

  int getID() const;
  void setID(int newID);

  bool operator==(const Button2 &rhs) const;

  void loop();

 private:
  static int _nextID;
  uint8_t _getState() const;

};
/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
