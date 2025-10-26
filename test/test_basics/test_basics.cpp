/////////////////////////////////////////////////////////////////
/*
  Basic functionality tests for Button2 library.
  Tests core button operations, state management, and fundamental features.

  Created by Lennart Hennigs
  Migrated to PlatformIO + EpoxyDuino for fast native testing
*/
/////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <AUnitVerbose.h>
#include <Button2.h>

using namespace aunit;

/////////////////////////////////////////////////////////////////

#define SERIAL_SPEED 115200

#define BUTTON_PIN      37
#define BUTTON_MODE     INPUT_PULLUP
#define BUTTON_ACTIVE   LOW

#define DEBOUNCE_MS     BTN_DEBOUNCE_MS + 5

/////////////////////////////////////////////////////////////////

// Global test state variables
bool pressed = false;
bool released = false;
bool tap = false;
bool changed = false;

/////////////////////////////////////////////////////////////////

void setup_test_runner() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
}

/////////////////////////////////////////////////////////////////
// helper functions
/////////////////////////////////////////////////////////////////

// Helper to initialize button with EpoxyDuino
Button2 createTestButton() {
  Button2 button;
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW);
  // Initialize the pin state to released using EpoxyDuino
  pinMode(BUTTON_PIN, BUTTON_MODE);
  digitalWrite(BUTTON_PIN, !BUTTON_ACTIVE);
  return button;
}

// resets all handler vars
void resetHandlerVars() {
  pressed = false;
  released = false;
  tap = false;
  changed = false;
}

/////////////////////////////////////////////////////////////////
// BASIC FUNCTIONALITY TESTS
/////////////////////////////////////////////////////////////////

test(basics, equal_operator) {
  Button2 button1 = createTestButton();
  Button2 button2;
  button2.begin(BUTTON_PIN + 1, BUTTON_MODE, BUTTON_ACTIVE == LOW);
  // Initialize second pin
  pinMode(BUTTON_PIN + 1, BUTTON_MODE);
  digitalWrite(BUTTON_PIN + 1, !BUTTON_ACTIVE);
  
  assertTrue(button1 == button1);
  assertTrue(button2 == button2);
  assertFalse(button1 == button2);
}

/////////////////////////////////////////////////////////////////

test(defaults, doubleclick_time) {
  Button2 button = createTestButton();
  assertEqual(button.getDoubleClickTime(), BTN_DOUBLECLICK_MS);
}

/////////////////////////////////////////////////////////////////

test(defaults, debounce_time) {
  Button2 button = createTestButton();
  assertEqual(button.getDebounceTime(), BTN_DEBOUNCE_MS);
}

/////////////////////////////////////////////////////////////////

test(defaults, longclick_time) {
  Button2 button = createTestButton();
  assertEqual(button.getLongClickTime(), BTN_LONGCLICK_MS);
}

/////////////////////////////////////////////////////////////////

test(defaults, id) {
  Button2 button = createTestButton();
  // Button2 assigns unique IDs automatically, just check it's a valid ID
  assertTrue(button.getID() >= 0);
}

/////////////////////////////////////////////////////////////////

test(defaults, pin) {
  Button2 button = createTestButton();
  assertTrue(button.getPin() == BUTTON_PIN);
}

/////////////////////////////////////////////////////////////////

test(other, to_string) {
  Button2 button = createTestButton();

  assertStringCaseEqual(button.clickToString(clickType::single_click), String("single click"));
  assertStringCaseEqual(button.clickToString(clickType::double_click), String("double click"));
  assertStringCaseEqual(button.clickToString(clickType::triple_click), String("triple click"));
  assertStringCaseEqual(button.clickToString(clickType::long_click),  String("long click"));
}

/////////////////////////////////////////////////////////////////
// INIT CALLBACK TESTS (Issue #69)
/////////////////////////////////////////////////////////////////

// Global flag to track if init callback was called
bool initCallbackCalled = false;

void testInitCallback() {
  initCallbackCalled = true;
}

test(init_callback, callback_is_called) {
  // Reset flag
  initCallbackCalled = false;

  // Create button with init callback
  Button2 button = createTestButton(); // Use helper to avoid pin issues

  // Re-init with callback
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW, testInitCallback);

  // Verify callback was called
  assertTrue(initCallbackCalled);
}

/////////////////////////////////////////////////////////////////

test(init_callback, callback_not_required) {
  // Test backward compatibility - begin() works without callback
  Button2 button = createTestButton();

  // Button should be functional
  assertEqual(button.getPin(), BUTTON_PIN);
}

/////////////////////////////////////////////////////////////////

test(init_callback, lambda_callback) {
  bool lambdaCalled = false;

  // Create button with lambda callback
  Button2 button = createTestButton();

  // Re-init with lambda
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW, [&lambdaCalled]() {
    lambdaCalled = true;
  });

  // Verify lambda was called
  assertTrue(lambdaCalled);
}

/////////////////////////////////////////////////////////////////

void setup() {
  setup_test_runner();

  // setup serial (reduced delay for faster native testing)
  delay(100);
  Serial.begin(SERIAL_SPEED);
  while(!Serial) {}
  Serial.println(F("\n\nButton2 Basic Functionality Tests"));
  Serial.println(F("Using EpoxyDuino + PlatformIO"));
}

/////////////////////////////////////////////////////////////////

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
