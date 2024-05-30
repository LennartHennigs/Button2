/////////////////////////////////////////////////////////////////
/*
  Button2.cpp - Arduino Library to simplify working with buttons.
  Copyright (C) 2017-2024 Lennart Hennigs.
  Released under the MIT license.
*/
/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////
// initialize static counter for the IDs

int Button2::_nextID = 0;

/////////////////////////////////////////////////////////////////
//  default constructor

Button2::Button2() {
  pin = BTN_UNDEFINED_PIN;
  _setID();
}

/////////////////////////////////////////////////////////////////
// constructor

Button2::Button2(byte attachTo, byte buttonMode /* = INPUT_PULLUP */, boolean activeLow /* = true */, Hardware *hardware /* = ArduinoHardware() */) {
  begin(attachTo, buttonMode, activeLow, hardware);
  _setID();
}

/////////////////////////////////////////////////////////////////

void Button2::begin(byte attachTo, byte buttonMode /* = INPUT_PULLUP */, boolean activeLow /* = true */, Hardware *hardware /* = ArduinoHardware() */) {
  hw = hardware;
  pin = attachTo;
  longclick_counter = 0;
  longclick_retriggerable = false;
  _pressedState = activeLow ? LOW : HIGH;

  if (attachTo != BTN_VIRTUAL_PIN) {
    hw->pinMode(attachTo, buttonMode);
  }
  //  state = activeLow ? HIGH : LOW;
  state = _getState();
  prev_state = state;
}

/////////////////////////////////////////////////////////////////

void Button2::setDebounceTime(unsigned int ms) {
  debounce_time_ms = ms;
}

/////////////////////////////////////////////////////////////////

void Button2::setLongClickTime(unsigned int ms) {
  longclick_time_ms = ms;
}

/////////////////////////////////////////////////////////////////

void Button2::setDoubleClickTime(unsigned int ms) {
  doubleclick_time_ms = ms;
}

/////////////////////////////////////////////////////////////////

unsigned int Button2::getDebounceTime() const {
  return debounce_time_ms;
}

/////////////////////////////////////////////////////////////////

unsigned int Button2::getLongClickTime() const {
  return longclick_time_ms;
}

/////////////////////////////////////////////////////////////////

unsigned int Button2::getDoubleClickTime() const {
  return doubleclick_time_ms;
}

/////////////////////////////////////////////////////////////////

byte Button2::getPin() const {
  return pin;
}

/////////////////////////////////////////////////////////////////

void Button2::setButtonStateFunction(StateCallbackFunction f) {
  get_state_cb = f;
}

/////////////////////////////////////////////////////////////////

bool Button2::operator==(Button2 &rhs) {
  return (this == &rhs);
}

/////////////////////////////////////////////////////////////////

void Button2::setChangedHandler(CallbackFunction f) {
  change_cb = f;
}

/////////////////////////////////////////////////////////////////

void Button2::setPressedHandler(CallbackFunction f) {
  pressed_cb = f;
}

/////////////////////////////////////////////////////////////////

void Button2::setReleasedHandler(CallbackFunction f) {
  released_cb = f;
}

/////////////////////////////////////////////////////////////////

void Button2::setClickHandler(CallbackFunction f) {
  click_cb = f;
}

/////////////////////////////////////////////////////////////////

void Button2::setTapHandler(CallbackFunction f) {
  tap_cb = f;
}

/////////////////////////////////////////////////////////////////

void Button2::setLongClickHandler(CallbackFunction f) {
  long_cb = f;
}

/////////////////////////////////////////////////////////////////

void Button2::setLongClickDetectedRetriggerable(bool retriggerable) {
  longclick_retriggerable = retriggerable;
}

/////////////////////////////////////////////////////////////////

void Button2::setLongClickDetectedHandler(CallbackFunction f) {
  longclick_detected_cb = f;
}

/////////////////////////////////////////////////////////////////

void Button2::setDoubleClickHandler(CallbackFunction f) {
  double_cb = f;
}

/////////////////////////////////////////////////////////////////

void Button2::setTripleClickHandler(CallbackFunction f) {
  triple_cb = f;
}

/////////////////////////////////////////////////////////////////

unsigned int Button2::wasPressedFor() const {
  return down_time_ms;
}

/////////////////////////////////////////////////////////////////

boolean Button2::isPressed() const {
  return (state == _pressedState);
}

/////////////////////////////////////////////////////////////////

boolean Button2::isPressedRaw() const {
  return (_getState() == _pressedState);
}

/////////////////////////////////////////////////////////////////

byte Button2::getNumberOfClicks() const {
  return last_click_count;
}

/////////////////////////////////////////////////////////////////

clickType Button2::getType() const {
  return last_click_type;
}

/////////////////////////////////////////////////////////////////

int Button2::getID() const {
  return id;
}

/////////////////////////////////////////////////////////////////

void Button2::setID(int newID) {
  id = newID;
}

/////////////////////////////////////////////////////////////////

String Button2::clickToString(clickType type) const {
  if (type == single_click) return "single click";
  if (type == double_click) return "double click";
  if (type == triple_click) return "triple click";
  if (type == long_click) return "long click";
  return "none";
}

/////////////////////////////////////////////////////////////////

bool Button2::wasPressed() const {
  return was_pressed;
}


/////////////////////////////////////////////////////////////////

void Button2::resetPressedState() {
  was_pressed = false;
  last_click_type = empty;
  click_count = 0;
  down_time_ms = 0;
  pressed_triggered = false;
  longclick_detected = false;
  longclick_reported = false;
  longclick_counter = 0;
}


/////////////////////////////////////////////////////////////////

byte Button2::resetClickCount() {
  byte tmp = last_click_count;
  last_click_count = 0;
  return tmp;
}

/////////////////////////////////////////////////////////////////

clickType Button2::read(bool keepState /* = false */) {
  if (keepState) return last_click_type;

  clickType res = last_click_type;
  resetPressedState();
  return res;
}

/////////////////////////////////////////////////////////////////

clickType Button2::wait(bool keepState /* = false */) {
  while (!wasPressed()) {
    loop();
  }
  return read(keepState);
}

/////////////////////////////////////////////////////////////////

void Button2::waitForClick(bool keepState /* = false */) {
  do {
    while (!wasPressed()) {
      loop();
    }
  } while (read() != single_click);
}

/////////////////////////////////////////////////////////////////

void Button2::waitForDouble(bool keepState /* = false */) {
  do {
    while (!wasPressed()) {
      loop();
    }
  } while (read() != double_click);
}

/////////////////////////////////////////////////////////////////

void Button2::waitForTriple(bool keepState /* = false */) {
  do {
    while (!wasPressed()) {
      loop();
    }
  } while (read() != triple_click);
}

/////////////////////////////////////////////////////////////////

void Button2::waitForLong(bool keepState /* = false */) {
  do {
    while (!wasPressed()) {
      loop();
    }
  } while (read() != long_click);
}

/////////////////////////////////////////////////////////////////

void Button2::reset() {
  pin = BTN_UNDEFINED_PIN;

  resetPressedState();

  pressed_cb = NULL;
  released_cb = NULL;
  change_cb = NULL;
  tap_cb = NULL;
  click_cb = NULL;
  long_cb = NULL;
  longclick_detected_cb = NULL;
  double_cb = NULL;
  triple_cb = NULL;
}

/////////////////////////////////////////////////////////////////

void Button2::loop() {
  if (pin == BTN_UNDEFINED_PIN) return;
  
  prev_state = state;
  state = _getState();

  if (state == _pressedState) {
    _handlePress(millis());
  } else {
    _handleRelease(millis());
  }
}

/////////////////////////////////////////////////////////////////

void Button2::_handlePress(long now) {
  // is it pressed now?
  if (prev_state != _pressedState) {
    _pressedNow(now);
    return;
  }
  // is it pressed for a while?
  if (!pressed_triggered) {
    if (now - down_ms >= debounce_time_ms) {
      pressed_triggered = true;
      _validKeypress();
    }
  }
  // only check for long press on the first click
  if (click_count == 1) {
    _checkForLongClick(now);
  }
}

/////////////////////////////////////////////////////////////////

void Button2::_setID() {
  id = _nextID;
  _nextID++;
}

/////////////////////////////////////////////////////////////////

void Button2::_handleRelease(long now) {
  // is it released right now?
  if (prev_state == _pressedState) {
    _releasedNow(now);
    return;
  }
  // report click after double click time has passed
  if (now - click_ms > doubleclick_time_ms) {
    _reportClicks();
  }
}

/////////////////////////////////////////////////////////////////

void Button2::_pressedNow(long now) {
  down_ms = now;
  pressed_triggered = false;
  click_ms = down_ms;
}

/////////////////////////////////////////////////////////////////

void Button2::_validKeypress() {
  click_count++;
  if (change_cb != NULL) change_cb(*this);
  if (pressed_cb != NULL) pressed_cb(*this);
}

/////////////////////////////////////////////////////////////////

void Button2::_checkForLongClick(long now) {
  if (longclick_detected_cb == NULL) return;
  if (longclick_reported) return;

  // has the longclick_ms period has been exceeded?
  if (now - down_ms < (longclick_time_ms * (longclick_counter + 1))) return;
  // report multiple?

  if (!longclick_retriggerable) {
    longclick_reported = true;
  }
  last_click_count = 1;
  last_click_type = long_click;
  longclick_counter++;
  longclick_detected_cb(*this);
  longclick_detected = true;
}

/////////////////////////////////////////////////////////////////

byte Button2::getLongClickCount() const {
  return longclick_counter;
}

/////////////////////////////////////////////////////////////////

void Button2::_reportClicks() {
  // no click
  if (click_count == 0) return;

  last_click_count = click_count;
  
  // single or long press
  if (click_count == 1) {
    // long press
    if (longclick_detected) {
      last_click_type = long_click;
      if (long_cb != NULL) long_cb(*this);
      longclick_counter = 0;
    // single click
    } else {
      last_click_type = single_click;
      if (click_cb != NULL) click_cb (*this);
    }

  // double click
  } else if (click_count == 2) {
      last_click_type = double_click;
      if (double_cb != NULL) double_cb(*this);

  // triple or x-clicks
  } else {
      last_click_type = triple_click;
      if (triple_cb != NULL) triple_cb(*this);
  }

  was_pressed = true;
  click_count = 0;
  click_ms = 0;
  longclick_detected = false;
  longclick_reported = false;
}

/////////////////////////////////////////////////////////////////

void Button2::_releasedNow(long now) {
  down_time_ms = now - down_ms;
  // is it beyond debounce time?
  if (down_time_ms < debounce_time_ms) return;
  // trigger release
  if (change_cb != NULL) change_cb(*this);
  if (released_cb != NULL) released_cb(*this);
  // trigger tap
  if (tap_cb != NULL) tap_cb(*this);
  // was it a longclick? (precedes single / double / triple clicks)
  if (down_time_ms >= longclick_time_ms) {
    longclick_detected = true;
  }
}

/////////////////////////////////////////////////////////////////

byte Button2::_getState() const {
  if (get_state_cb != NULL) {
    return get_state_cb();
  } else {
    return hw->digitalRead(pin);
  }
}

/////////////////////////////////////////////////////////////////