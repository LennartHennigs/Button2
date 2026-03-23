/////////////////////////////////////////////////////////////////
/*
  Shared test helpers for Button2 test suites.
  Provides a common button simulation infrastructure used by all suites.

  Correct initialization order (must be followed in all suites):
    1. simulatedPinState = !BUTTON_ACTIVE
    2. button.setButtonStateFunction(getSimulatedPinState)
    3. button.begin(...)
*/
/////////////////////////////////////////////////////////////////

#pragma once

#include <Arduino.h>
#include <Button2.h>

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN    37
#define BUTTON_MODE   INPUT_PULLUP
#define BUTTON_ACTIVE LOW

#define DEBOUNCE_MS   (BTN_DEBOUNCE_MS + 5)

/////////////////////////////////////////////////////////////////

static uint8_t simulatedPinState = HIGH;

uint8_t getSimulatedPinState() {
  return simulatedPinState;
}

/////////////////////////////////////////////////////////////////

// Correct order: set state → setButtonStateFunction → begin
inline Button2 createTestButton() {
  Button2 button;
  simulatedPinState = !BUTTON_ACTIVE;
  button.setButtonStateFunction(getSimulatedPinState);
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW);
  return button;
}

/////////////////////////////////////////////////////////////////

// Press and hold for `duration` ms (looping), without releasing.
inline void pressAndHold(Button2& button, unsigned long duration) {
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();

  unsigned long end = millis() + duration;
  while (millis() < end) {
    button.loop();
    delay(1);
  }
}

// Emulate a button click: press, hold for `duration` ms (looping), then release.
inline void click(Button2& button, unsigned long duration) {
  pressAndHold(button, duration);
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
  delay(5);
  button.loop();
}

inline void press(Button2& button) {
  simulatedPinState = BUTTON_ACTIVE;
  delay(5);
  button.loop();
}

inline void release(Button2& button) {
  simulatedPinState = !BUTTON_ACTIVE;
  delay(5);
  button.loop();
}

/////////////////////////////////////////////////////////////////
