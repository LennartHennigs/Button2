/////////////////////////////////////////////////////////////////
/*
  Button2.cpp - Arduino Library to simplify working with buttons.
  Created by Lennart Hennigs.
*/
/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////
// initalize static counter

int Button2::_nextID = 0;

/////////////////////////////////////////////////////////////////
//  default contructor

Button2::Button2() {  
  pin = UNDEFINED_PIN;
  id = _nextID++;
}

/////////////////////////////////////////////////////////////////
// contructor

Button2::Button2(byte attachTo, byte buttonMode /* = INPUT_PULLUP */, boolean activeLow /* = true */) {
  begin(attachTo, buttonMode, activeLow);
}

/////////////////////////////////////////////////////////////////

void Button2::begin(byte attachTo, byte buttonMode /* = INPUT_PULLUP */, boolean activeLow /* = true */) {  
  pin = attachTo;
  id = _nextID++;
#ifndef _IGNORE_LONG  
  longclick_detected_counter = 0;
  longclick_detected_retriggerable = false;
#endif
  _pressedState = activeLow ? LOW : HIGH;
  if (attachTo != VIRTUAL_PIN) {
    pinMode(attachTo, buttonMode);
  }
  //  state = activeLow ? HIGH : LOW;
  state = _getState();
  prev_state = state ;
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

bool Button2::operator == (Button2 &rhs) {
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

#ifndef _IGNORE_LONG
void Button2::setLongClickHandler(CallbackFunction f) {
  long_cb = f;
}

/////////////////////////////////////////////////////////////////

void Button2::setLongClickDetectedRetriggerable(bool retriggerable) {
  longclick_detected_retriggerable = retriggerable;
}

/////////////////////////////////////////////////////////////////

void Button2::setLongClickDetectedHandler(CallbackFunction f) {
  longclick_detected_cb = f;
}
#endif

/////////////////////////////////////////////////////////////////

#ifndef _IGNORE_DOUBLE
void Button2::setDoubleClickHandler(CallbackFunction f) {
  double_cb = f;
}
#endif

/////////////////////////////////////////////////////////////////

#ifndef _IGNORE_TRIPLE
void Button2::setTripleClickHandler(CallbackFunction f) {
  triple_cb = f;
}
#endif

/////////////////////////////////////////////////////////////////

unsigned int Button2::wasPressedFor() const {
  return down_time_ms;
}

/////////////////////////////////////////////////////////////////

boolean Button2::isPressed() const {
  return (state == _pressedState);
}

/////////////////////////////////////////////////////////////////

boolean Button2::isPressedRaw() {
  return (_getState() == _pressedState);
}

/////////////////////////////////////////////////////////////////

byte Button2::getNumberOfClicks() const {
    return click_count;
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
  if (type == single_click) return "click";
#ifndef _IGNORE_LONG
  if (type == long_click) return "long click";
#endif
#ifndef _IGNORE_DOUBLE
  if (type == double_click) return "double click";
#endif
#ifndef _IGNORE_TRIPLE
  if (type == triple_click) return "triple click";
#endif  
  return "empty";
 }

/////////////////////////////////////////////////////////////////

bool Button2::wasPressed() const {
  return was_pressed;
}

/////////////////////////////////////////////////////////////////

clickType Button2::read(bool keepState /* = false */) {
    if (!keepState) {
      clickType res = last_click_type;
      last_click_type = empty;
      was_pressed = false;
      return res;
    }
    return last_click_type;
}

/////////////////////////////////////////////////////////////////

clickType Button2::wait(bool keepState /* = false */) {
    while(!wasPressed()) {
      loop();
    }
    return read(keepState);
}

/////////////////////////////////////////////////////////////////

void Button2::waitForClick(bool keepState /* = false */) {
  do {
    while(!wasPressed()) {
      loop();
    }
  } while(read() != single_click);
}

/////////////////////////////////////////////////////////////////

#ifndef _IGNORE_DOUBLE
void Button2::waitForDouble(bool keepState  /* = false */) {
  do {
    while(!wasPressed()) {
      loop();
    }
  } while(read() != double_click);
}
#endif

/////////////////////////////////////////////////////////////////

#ifndef _IGNORE_TRIPLE
void Button2::waitForTriple(bool keepState /* = false */) {
  do {
    while(!wasPressed()) {
      loop();
    }
  } while(read() != triple_click);
}
#endif

/////////////////////////////////////////////////////////////////

#ifndef _IGNORE_LONG
void Button2::waitForLong(bool keepState  /* = false */) {
  do {
    while(!wasPressed()) {
      loop();
    }
  } while(read() != long_click);
}
#endif

/////////////////////////////////////////////////////////////////

byte Button2::_getState() const {
  if (get_state_cb != NULL) {
    return get_state_cb();
  } else {
    return digitalRead(pin);
  }
}

/////////////////////////////////////////////////////////////////

void Button2::loop() {
  if (pin != UNDEFINED_PIN) {
    unsigned long now = millis();
    prev_state = state;
    state = _getState();
    
    // is button pressed?
    if (state == _pressedState) {
      // is it pressed now?
      if (prev_state != _pressedState) {
        down_ms = now;
        pressed_triggered = false;
        click_ms = down_ms;
      // trigger pressed event (after debounce has passed)
      } else if (!pressed_triggered && (now - down_ms >= debounce_time_ms)) {
        pressed_triggered = true;
        click_count++;
        if (change_cb != NULL) change_cb (*this);      
        if (pressed_cb != NULL) pressed_cb (*this);
      }
#ifndef _IGNORE_LONG      
      // is a longpress detected callback defined?
      if (longclick_detected_cb != NULL) {
        // check to see that the longclick_ms period has been exceeded and call the appropriate callback
        bool longclick_period_detected = now - down_ms >= (longclick_time_ms * (longclick_detected_counter + 1));
        if (longclick_period_detected && !longclick_detected_reported) {
          longclick_detected_reported = true;
          longclick_detected = true;
          if (longclick_detected_retriggerable) {
            // increate the counter and reset the "reported" flag (as the counter will stop the false trigger)
            longclick_detected_counter++;
            longclick_detected_reported = false;
          }
          if (longclick_detected_cb != NULL) longclick_detected_cb(*this);
        }
      }
#endif
    // is the button released?
    } else if (state != _pressedState) {
      // is it released right now?
      if (prev_state == _pressedState) {
        down_time_ms = now - down_ms;
        // is it beyond debounce time?
        if (down_time_ms >= debounce_time_ms) {
          last_click_type = single_click;
          // trigger release        
          if (change_cb != NULL) change_cb (*this);
          if (released_cb != NULL) released_cb (*this);
          // trigger tap
          if (tap_cb != NULL) tap_cb (*this);        
#ifndef _IGNORE_LONG
          // was it a longclick? (preceeds single / double / triple clicks)
          if (down_time_ms >= longclick_time_ms) {
            longclick_detected = true;
          }
#endif
        }
      // is the button released and the time has passed for multiple clicks?
      } else if (now - click_ms > doubleclick_time_ms) {
#ifndef _IGNORE_LONG  
        // was there a longclick?
        if (longclick_detected) {
          // was it part of a combination?
          if (click_count == 1) {
            last_click_type = long_click;
            if (long_cb != NULL) long_cb (*this);
            was_pressed = true;
          }
          longclick_detected = false;
          longclick_detected_reported = false;
          longclick_detected_counter = 0;

        // determine the number of single clicks
        } else if (click_count > 0) {
#else
        // determine the number of single clicks
        if (click_count > 0) {
#endif
          switch (click_count) {
            case 1: 
              last_click_type = single_click;
              if (click_cb != NULL) click_cb (*this);
              break;
#ifndef _IGNORE_DOUBLE
#ifdef _IGNORE_TRIPLE // all multi-clicks are now identified as double
            default:
#else            
            case 2: 
#endif
              last_click_type = double_click;
              if (double_cb != NULL) double_cb (*this);
              break;
#endif
#ifndef _IGNORE_TRIPLE
            case 3: 
              last_click_type = triple_click;
              if (triple_cb != NULL) triple_cb (*this);
              break;
#endif
          }
          was_pressed = true;
        }
        // clean up
        click_count = 0;
        click_ms = 0;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////

void Button2::reset() {
  pin = UNDEFINED_PIN;
  click_count = 0;
  last_click_type = empty;
  down_time_ms = 0;
  pressed_triggered = false;
#ifndef _IGNORE_LONG
  longclick_detected = false;
  longclick_detected_reported = false;
  longclick_detected_counter = 0;
#endif
  pressed_cb = NULL;
  released_cb = NULL;
  change_cb = NULL;
  tap_cb = NULL;
  click_cb = NULL;
#ifndef _IGNORE_LONG
  long_cb = NULL;
  longclick_detected_cb = NULL;
#endif
#ifndef _IGNORE_DOUBLE
  double_cb = NULL;
#endif
#ifndef _IGNORE_TRIPLE
  triple_cb = NULL;
#endif
}

/////////////////////////////////////////////////////////////////
