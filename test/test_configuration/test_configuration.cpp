/////////////////////////////////////////////////////////////////
/*
  Configuration tests for Button2 library.
  Tests settings, parameters, IDs, and configuration management.

  Created by Lennart Hennigs
  Migrated to PlatformIO + EpoxyDuino for fast native testing
*/
/////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <AUnitVerbose.h>
#include "../shared/test_helpers.h"

using namespace aunit;

/////////////////////////////////////////////////////////////////

#define SERIAL_SPEED 115200

/////////////////////////////////////////////////////////////////

// Test state variables
bool pressed = false;
bool released = false;
bool tap = false;
bool longclick = false;
bool long_detected = false;
int  long_detected_count = 0;
bool changed = false;

void setup_test_runner() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
}

void resetHandlerVars() {
  pressed = false;
  released = false;
  tap = false;
  longclick = false;
  long_detected = false;
  changed = false;
  long_detected_count = 0;
}

/////////////////////////////////////////////////////////////////
// CONFIGURATION TESTS
/////////////////////////////////////////////////////////////////

test(settings, id) {
  Button2 button = createTestButton();
  int id = 10;
  button.setID(id);
  // run the tests
  assertTrue(button.getID() == id);
  // clean up
  button.setID(0);
}

/////////////////////////////////////////////////////////////////

test(settings, additional_ids) {
  Button2 button1 = createTestButton();
  Button2 button2;
  button2.begin(BUTTON_PIN + 1, BUTTON_MODE, BUTTON_ACTIVE == LOW);
  // Set up custom state function for the second button too
  button2.setButtonStateFunction(getSimulatedPinState);
  assertNotEqual(button2.getID(), button1.getID());
}

/////////////////////////////////////////////////////////////////

test(settings, doubleclick_time) {
  Button2 button = createTestButton();
  unsigned int dc_time = BTN_DOUBLECLICK_MS + 1;
  button.setDoubleClickTime(dc_time);
  // run the tests
  assertEqual(button.getDoubleClickTime(), dc_time);
  // clean up
  button.setDoubleClickTime(BTN_DOUBLECLICK_MS);
}

/////////////////////////////////////////////////////////////////

test(settings, longclick_time) {
  Button2 button = createTestButton();
  unsigned int lc_time = BTN_LONGCLICK_MS + 1;
  button.setLongClickTime(lc_time);
  // run the tests
  assertEqual(button.getLongClickTime(), lc_time);
  // clean up
  button.setLongClickTime(BTN_LONGCLICK_MS);
}

/////////////////////////////////////////////////////////////////

test(settings, debounce_time) {
  Button2 button = createTestButton();
  unsigned int debounce_time = BTN_DEBOUNCE_MS + 1;
  button.setDebounceTime(debounce_time);
  // run the tests
  assertEqual(button.getDebounceTime(), debounce_time);
  // clean up
  button.setDebounceTime(BTN_DEBOUNCE_MS);
}

/////////////////////////////////////////////////////////////////

test(settings, resetPressedState) {
  Button2 button = createTestButton();
  button.resetPressedState();
  // single click
  int time = DEBOUNCE_MS;
  click(button, time);
  // wait out the double click time
  delay(BTN_DOUBLECLICK_MS);
  button.loop();
  // run the tests
  assertTrue(button.wasPressed());
  assertEqual(button.getNumberOfClicks(), 1);
  // now reset the "click memory"
  button.resetPressedState();
  assertFalse(button.wasPressed());
  assertEqual(button.getNumberOfClicks(), 0);
}

/////////////////////////////////////////////////////////////////

test(settings, longclick_interval_default) {
  Button2 button = createTestButton();
  // Default: no custom interval (sentinel 0)
  assertEqual(button.getLongClickInterval(), (unsigned int)0);
}

/////////////////////////////////////////////////////////////////

test(settings, longclick_interval_set) {
  Button2 button = createTestButton();
  button.setLongClickDetectedRetriggerable(true, 500);
  assertEqual(button.getLongClickInterval(), (unsigned int)500);
}

/////////////////////////////////////////////////////////////////

test(settings, longclick_interval_reset_by_bool_overload) {
  Button2 button = createTestButton();
  button.setLongClickDetectedRetriggerable(true, 500);
  // Calling the bool-only overload should clear the interval
  button.setLongClickDetectedRetriggerable(true);
  assertEqual(button.getLongClickInterval(), (unsigned int)0);
}

/////////////////////////////////////////////////////////////////

test(settings, context_default_null) {
  Button2 button = createTestButton();
  assertEqual(button.getContext(), (void*)nullptr);
}

/////////////////////////////////////////////////////////////////

test(settings, context_set_get) {
  Button2 button = createTestButton();
  int value = 42;
  button.setContext(&value);
  assertEqual(button.getContext(), (void*)&value);
}

/////////////////////////////////////////////////////////////////

test(settings, context_in_callback) {
  Button2 button = createTestButton();
  button.resetPressedState();

  struct Ctx { int result; };
  Ctx ctx = { 0 };
  button.setContext(&ctx);

  button.setClickHandler([](Button2& btn) {
    Ctx* c = (Ctx*)btn.getContext();
    c->result = 99;
  });

  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS + 10);
  button.loop();

  assertEqual(ctx.result, 99);
}

/////////////////////////////////////////////////////////////////

test(settings, context_survives_reset) {
  Button2 button = createTestButton();
  int value = 7;
  button.setContext(&value);
  button.reset();
  assertEqual(button.getContext(), (void*)&value);
}

/////////////////////////////////////////////////////////////////

test(settings, set_pressed_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  
  int testVal = 0;
  button.setPressedHandler([](Button2& b) {
    // Empty handler
  });
  button.setPressedHandler([&testVal](Button2& b) {
    testVal = 101;
  });
  release(button);
  button.resetPressedState();
  press(button);
  delay(DEBOUNCE_MS);
  button.loop();
  assertEqual(testVal, 101);
  
  // clean up
  button.setPressedHandler([](Button2& b) {
    released = false;
    pressed = true;
  });
  release(button);
  button.resetPressedState();
  pressed = false;
}

/////////////////////////////////////////////////////////////////

void setup() {
  setup_test_runner();

  // setup serial
  delay(100);  // Reduced for faster native testing
  Serial.begin(SERIAL_SPEED);
  while(!Serial) {}
  Serial.println(F("\n\nButton2 Configuration Tests"));
  Serial.println(F("Using EpoxyDuino + PlatformIO"));
}

/////////////////////////////////////////////////////////////////

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
