#line 2 "Button2Test.ino"
/////////////////////////////////////////////////////////////////
/*
  Unit tests for Button2 library.
  Arduino Library to simplify working with buttons.

  Created by Lennart Hennigs
  Tested on Wemos D1 Mini, M5Stack ESP32, Arduino UNO
*/
/////////////////////////////////////////////////////////////////
/*
  To add: 
  - check retriggerable long-clicks (setLongClickDetectedRetriggerable getLongClickCount())
  - check loop functions (read(), wait(), waitForClick(), waitForDouble(), waitForTriple())
  - check custom handler 
*/
/////////////////////////////////////////////////////////////////

 #include "AUnitVerbose.h"
// #include "AUnit.h"
#include "Button2.h"

using namespace aunit;

/////////////////////////////////////////////////////////////////

#define SERIAL_SPEED 115200

#define VERBOSE_CHANGED         false
#define VERBOSE_PRESS_RELEASE   false
#define VERBOSE_MAIN_EVENTS     false // clicks, long presses

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN      37
#define BUTTON_MODE     INPUT_PULLUP
#define BUTTON_ACTIVE   LOW

#define DEBOUNCE_MS     BTN_DEBOUNCE_MS + 5

/////////////////////////////////////////////////////////////////

MockHardware hw;
Button2 button;

bool pressed = false;
bool released = false;
bool tap = false;
bool longclick = false;
bool long_detected = false;
int  long_detected_count = 0;
bool changed = false;

/////////////////////////////////////////////////////////////////

void setup_test_runner() {
  TestRunner::setVerbosity(Verbosity::kDefault);
//  TestRunner::setVerbosity(Verbosity::kAssertionFailed);
//  TestRunner::setVerbosityVerbosity::kAll);

  TestRunner::list();
/*
  TestRunner::exclude("*");
  // TestRunner::include("basics_*");
  // TestRunner::include("defaults_*");
  // TestRunner::include("settings_*");
  // TestRunner::include("clicks_*");
  // TestRunner::include("other_*");
*/
}

/////////////////////////////////////////////////////////////////
// helper functions
/////////////////////////////////////////////////////////////////

// pressing the button
void press() {
  hw.digitalWrite(BUTTON_PIN, BUTTON_ACTIVE);
  button.loop(); 
}

// letting it go
void release() {
  hw.digitalWrite(BUTTON_PIN, !BUTTON_ACTIVE);
  button.loop(); 
}

// emulate a button click
void clickFor(unsigned long duration) {
  press();
  delay(duration);
  button.loop(); 
  release();
}

// resets all handler vars
void resetHandlerVars() {
  pressed = false;
  released = false;
  tap = false;
  longclick = false;
  long_detected = false;
  changed = false;
  long_detected_count = 0;
}

// executes a single click
int singleClick(bool waitItOut = true) {
  int time = DEBOUNCE_MS + 10;
  clickFor(time);
  // wait out the double click time
  if (waitItOut) {
    delay(BTN_DOUBLECLICK_MS);
    button.loop();  
  }
  return time;
}

// executes a long click
int longClick() {
  int time = BTN_LONGCLICK_MS + 10;
  clickFor(time);
   // wait out the double click time
  delay(BTN_DOUBLECLICK_MS);
  button.loop();  
  return time;
}


/////////////////////////////////////////////////////////////////
// BASICS
/////////////////////////////////////////////////////////////////

test(basics, loop) {
  resetHandlerVars();
  button.resetPressedState();
  // press the button down
  hw.digitalWrite(BUTTON_PIN, BUTTON_ACTIVE);
  button.loop();
  // wait a while
  delay(DEBOUNCE_MS);
  // is there an update without the loop?
  assertFalse(pressed);
  // there should be one now
  button.loop();
  assertTrue(pressed);

  // release the button 
  hw.digitalWrite(BUTTON_PIN, !BUTTON_ACTIVE);
  // wait a while
  delay(DEBOUNCE_MS);
  // is there an update without the loop?
  assertFalse(released);
  // there should be one now
  button.loop();
  assertTrue(released);
}

/////////////////////////////////////////////////////////////////

test(basics, equal_operator) {
  Button2 b;
  assertTrue(button == button);
  assertTrue(b == b);
  assertFalse(button == b);
}

/////////////////////////////////////////////////////////////////
// CLICKS
/////////////////////////////////////////////////////////////////


test(clicks, changed_handler) {
  button.resetPressedState();
  changed = false;

  press();
  // wait a bit
  delay(DEBOUNCE_MS);
  button.loop(); 
  // test
  assertTrue(changed);
  changed = false;
  // let go
  release();
  // test
  assertTrue(changed);
  // clean up
  changed = false;
}

/////////////////////////////////////////////////////////////////

test(clicks, is_not_pressed) {
  release();
  button.resetPressedState();
  // run the tests
  assertFalse(button.isPressed());
}

/////////////////////////////////////////////////////////////////

test(clicks, is_pressed) {
  release();
  button.resetPressedState();
  press();
  // wait a bit
  delay(DEBOUNCE_MS);
  button.loop(); 
  // test
  assertTrue(button.isPressed());
  assertTrue(pressed);
  release();
  // clean up
  pressed = false;
}

/////////////////////////////////////////////////////////////////

test(clicks, is_pressed_raw) {
  button.resetPressedState();
  press();
  // wait a bit
  delay(BTN_DEBOUNCE_MS);
  button.loop(); 
  // test
  assertTrue(button.isPressedRaw());
  release();
  // clean up
  assertFalse(button.isPressedRaw());
  pressed = false;
}

/////////////////////////////////////////////////////////////////

test(clicks, not_a_click) {
  resetHandlerVars();
  button.resetPressedState();
  // click, but too short
  clickFor(BTN_DEBOUNCE_MS - 10);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();
  // run the tests
  assertFalse(button.wasPressed());  
  assertFalse(tap);
}

/////////////////////////////////////////////////////////////////

test(clicks, single_click) {
  resetHandlerVars();
  button.resetPressedState();

  int time = singleClick();
  int pressedFor = button.wasPressedFor();

  // run the tests
  assertTrue(button.wasPressed());
  assertNear(time, pressedFor, 10);
  assertEqual(button.getType(), single_click);
  assertEqual(button.getNumberOfClicks(), 1);
  assertTrue(tap);
}

/////////////////////////////////////////////////////////////////

test(clicks, long_click) {
  resetHandlerVars();
  button.resetPressedState();
  
  int time = longClick();
  int pressedFor = button.wasPressedFor();

  // run the tests
  assertTrue(button.wasPressed());
  assertNear(time, pressedFor, 10);
  assertEqual(button.getType(), long_click);
  assertEqual(button.getNumberOfClicks(), 1);
  assertTrue(long_detected);
  assertTrue(longclick);
  assertTrue(tap);
}

/////////////////////////////////////////////////////////////////

test(clicks, first_single_then_long_click) {
  resetHandlerVars();
  button.resetPressedState();

  singleClick();
  int long_time = longClick();
  int pressedFor = button.wasPressedFor();
  
  // run the tests
  assertTrue(button.wasPressed());
  assertNear(long_time, pressedFor, 10);
  assertEqual(button.getType(), long_click);
  assertEqual(button.getNumberOfClicks(), 1);
  assertTrue(long_detected);
  assertTrue(longclick);
  assertTrue(tap);
}

/////////////////////////////////////////////////////////////////

test(clicks, first_double_then_long_click) {
  resetHandlerVars();
  button.resetPressedState();

  // double click
  singleClick(false);
  singleClick(true);
  int long_time = longClick();
  int pressedFor = button.wasPressedFor();
  
  // run the tests
  assertTrue(button.wasPressed());
  assertNear(long_time, pressedFor, 10);
  assertEqual(button.getType(), long_click);
  assertEqual(button.getNumberOfClicks(), 1);
  assertTrue(long_detected);
  assertTrue(longclick);
  assertTrue(tap);
}

/////////////////////////////////////////////////////////////////

test(clicks, double_click) {
  button.resetPressedState();
  // 2x click
  singleClick(false);
  singleClick(true);
  // run the tests
  assertTrue(button.wasPressed());
  assertEqual(button.getType(), double_click);
  assertEqual(button.getNumberOfClicks(), 2);
}

/////////////////////////////////////////////////////////////////

test(clicks, triple_click) {
  button.resetPressedState();
  // 3x click
  singleClick(false);
  singleClick(false);
  singleClick(true);
  // run the tests
  assertTrue(button.wasPressed());
  assertEqual(button.getType(), triple_click);
  assertEqual(button.getNumberOfClicks(), 3);
}

/////////////////////////////////////////////////////////////////

test(clicks, more_than_3_click) {
  button.resetPressedState();
  // 4x click
  singleClick(false);
  singleClick(false);
  singleClick(false);
  singleClick(true);
  // run the tests
  assertTrue(button.wasPressed());
  assertEqual(button.getType(), triple_click);
  assertEqual(button.getNumberOfClicks(), 4);
}

/////////////////////////////////////////////////////////////////

test (clicks, reset) {
  button.resetPressedState();
  // single click
  singleClick();
  // run the tests
  assertTrue(button.wasPressed());
  assertEqual(button.getNumberOfClicks(), 1);
  // now reset the "click memory"
  button.resetPressedState();
  assertFalse(button.wasPressed());
  button.resetClickCount();
  assertEqual(button.getNumberOfClicks(), 0);
}

/////////////////////////////////////////////////////////////////
// DEFAULTS
/////////////////////////////////////////////////////////////////

test(defaults, doubleclick_time) {
  assertEqual(button.getDoubleClickTime(), BTN_DOUBLECLICK_MS);
}

/////////////////////////////////////////////////////////////////

test(defaults, debounce_time) {
  assertEqual(button.getDebounceTime(), BTN_DEBOUNCE_MS);
}

/////////////////////////////////////////////////////////////////

test(defaults, longclick_time) {
  assertEqual(button.getLongClickTime(), BTN_LONGCLICK_MS);
}

/////////////////////////////////////////////////////////////////

test(defaults, id) {
  assertTrue(button.getID() == 0);
}

/////////////////////////////////////////////////////////////////

test(defaults, pin) {
  assertTrue(button.getPin() == BUTTON_PIN);
}

/////////////////////////////////////////////////////////////////
// SETTING
/////////////////////////////////////////////////////////////////

test(settings, id) {
  int id = 10;
  button.setID(id);
  // run the tests
  assertTrue(button.getID() == id); 
  // clean up
  button.setID(0);
}

/////////////////////////////////////////////////////////////////

test(settings, additional_ids) {
  Button2 b;
  assertNotEqual(b.getID(), button.getID());
}

/////////////////////////////////////////////////////////////////

test(settings, doubleclick_time) {
  unsigned int dc_time = BTN_DOUBLECLICK_MS + 1;
  button.setDoubleClickTime(dc_time);
  // run the tests
  assertEqual(button.getDoubleClickTime(), dc_time);  
  // clean up
  button.setDoubleClickTime(BTN_DOUBLECLICK_MS);
}

/////////////////////////////////////////////////////////////////

test(settings, longclick_time) {
  unsigned int lc_time = BTN_LONGCLICK_MS + 1;
  button.setLongClickTime(lc_time);
  // run the tests
  assertEqual(button.getLongClickTime(), lc_time);  
  // clean up
  button.setLongClickTime(BTN_LONGCLICK_MS);
}

/////////////////////////////////////////////////////////////////

test(settings, debounce_time) {
  unsigned int debounce_time = BTN_DEBOUNCE_MS + 1;
  button.setDebounceTime(debounce_time);
  // run the tests
  assertEqual(button.getDebounceTime(), debounce_time);  
  // clean up
  button.setDebounceTime(BTN_DEBOUNCE_MS);
}

/////////////////////////////////////////////////////////////////
// OTHER
/////////////////////////////////////////////////////////////////

test(other, to_string) {
  assertStringCaseEqual(button.clickToString(clickType::single_click), String("single click"));
  assertStringCaseEqual(button.clickToString(clickType::double_click), String("double click"));
  assertStringCaseEqual(button.clickToString(clickType::triple_click), String("triple click"));
  assertStringCaseEqual(button.clickToString(clickType::long_click),  String("long click"));
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

// ignore unused params in the lamda functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/////////////////////////////////////////////////////////////////

void setup() {
  setup_test_runner();

  // setup serial
  delay(1000); 
  Serial.begin(SERIAL_SPEED);
  while(!Serial) {}
  Serial.println(F("\n\nButton2 Unit Tests"));
  // set up button
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW, &hw);

  button.setPressedHandler([](Button2& b) {
    if (VERBOSE_PRESS_RELEASE) Serial.println(F("-- PRESSED"));
    released = false;
    pressed = true;
  });
  button.setReleasedHandler([](Button2& b) {
    if (VERBOSE_PRESS_RELEASE) Serial.println(F("-- RELEASED"));
    released = true;
    pressed = false;
    tap = true;
  });
  button.setChangedHandler([](Button2& b) {
    if (VERBOSE_CHANGED) Serial.println(F("-- CHANGED"));
    changed = true;
  });
  button.setClickHandler([](Button2& b) {
    if (VERBOSE_MAIN_EVENTS) Serial.println(F("   -- CLICK"));
  });
  button.setLongClickHandler([](Button2& b) {
    if (VERBOSE_MAIN_EVENTS) Serial.println(F("   -- LONG"));
    longclick = true;
  });
  button.setLongClickDetectedHandler([](Button2& b) {
    if (VERBOSE_MAIN_EVENTS) Serial.println("   -- LONG DETECTED");
    long_detected = true;
    long_detected_count = long_detected_count + 1;
  });
  button.setDoubleClickHandler([](Button2& b) {
    if (VERBOSE_MAIN_EVENTS) Serial.println("   -- DOUBLE");
  });
  button.setTripleClickHandler([](Button2& b) {
    if (VERBOSE_MAIN_EVENTS) Serial.println("   -- TRIPLE ");
  });
}
#pragma GCC diagnostic push

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
