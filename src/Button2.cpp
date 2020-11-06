/////////////////////////////////////////////////////////////////
/*
  Button2.cpp - Arduino Library to simplify working with buttons.
  Created by Lennart Hennigs.
*/
/////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "Button2.h"

/////////////////////////////////////////////////////////////////

Button2::Button2(byte attachTo, byte buttonMode /* = INPUT_PULLUP */, boolean isCapacitive /* = false */, boolean activeLow /* = true */, unsigned int debounceTimeout /* = DEBOUNCE_MS */) {
  pin = attachTo;
  setDebounceTime(debounceTimeout);
  if (!isCapacitive) {
    pinMode(attachTo, buttonMode);
  } else {
    capacitive = true;
    pressed = activeLow ? LOW : HIGH;
    state = activeLow ? HIGH : LOW;
  }
}

/////////////////////////////////////////////////////////////////

bool Button2::operator==(Button2 &rhs) {
      return (this==&rhs);    
}
      
/////////////////////////////////////////////////////////////////

void Button2::setDebounceTime(unsigned int ms) {
      debounce_time_ms = ms;
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
  return (state != pressed);
}

/////////////////////////////////////////////////////////////////

boolean Button2::isPressedRaw() const {
  return (digitalRead(pin) == pressed);
}

/////////////////////////////////////////////////////////////////

byte Button2::getNumberOfClicks() const {
    return click_count;
}

/////////////////////////////////////////////////////////////////

byte Button2::getClickType() const {
    return last_click_type;
}

/////////////////////////////////////////////////////////////////

void Button2::loop() {
  prev_state = state;
  if (!capacitive) {
    state = digitalRead(pin);
  } else {
    #if defined(ARDUINO_ARCH_ESP32)
      int capa = touchRead(pin);
      state = capa <  CAPACITIVE_TOUCH_THRESHOLD ? LOW : HIGH;
    #endif
  }
  // is button pressed?
  if (prev_state != pressed && state == pressed) {
    down_ms = millis();
    pressed_triggered = false;
    click_ms = down_ms;

  // is the button released?
  } else if (prev_state == pressed && state != pressed) {
    down_time_ms = millis() - down_ms;
    // is it beyond debounce time?
    if (down_time_ms >= debounce_time_ms) {
      // trigger release        
      if (change_cb != NULL) change_cb (*this);
      if (released_cb != NULL) released_cb (*this);
      // trigger tap
      if (tap_cb != NULL) tap_cb (*this);        
      // was it a longclick? (preceeds single / double / triple clicks)
      if (down_time_ms >= LONGCLICK_MS) {
        longclick_detected = true;
      }
    }

  // trigger pressed event (after debounce has passed)
  } else if (state == pressed && !pressed_triggered && (millis() - down_ms >= debounce_time_ms)) {
    pressed_triggered = true;
    click_count++;
    if (change_cb != NULL) change_cb (*this);      
    if (pressed_cb != NULL) pressed_cb (*this);
  
  // is the button released and the time has passed for multiple clicks?
  } else if (state != pressed && millis() - click_ms > DOUBLECLICK_MS) {
    // was there a longclick?
    if (longclick_detected) {
      // was it part of a combination?
      if (click_count == 1) {
        last_click_type = LONG_CLICK;
        if (long_cb != NULL) long_cb (*this);
      }
      longclick_detected = false;      
    // determine the number of single clicks
    } else if (click_count > 0) {
      switch (click_count) {
        case 1: 
          last_click_type = SINGLE_CLICK;
          if (click_cb != NULL) click_cb (*this);
          break;
         case 2: 
          last_click_type = DOUBLE_CLICK;
          if (double_cb != NULL) double_cb (*this);
          break;
         case 3: 
          last_click_type = TRIPLE_CLICK;
          if (triple_cb != NULL) triple_cb (*this);
          break;
      }
    }
    click_count = 0;
    click_ms = 0;
  }
  //  yield();
}

/////////////////////////////////////////////////////////////////

void Button2::reset() {
  click_count = 0;
  last_click_type = 0;
  down_time_ms = 0;
  pressed_triggered = false;
  longclick_detected = false;
	
  pressed_cb = NULL;
  released_cb = NULL;
  change_cb = NULL;
  tap_cb = NULL;
  click_cb = NULL;
  long_cb = NULL;
  double_cb = NULL;
  triple_cb = NULL;
}

/////////////////////////////////////////////////////////////////
