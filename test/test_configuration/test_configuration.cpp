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
#include <Button2.h>

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

// Global test state variables
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
  TestRunner::list();
}

/////////////////////////////////////////////////////////////////
// helper functions
/////////////////////////////////////////////////////////////////

// Global state variable for button simulation
static uint8_t simulatedPinState = HIGH; // Start in released state (HIGH for INPUT_PULLUP)

// Custom state function for testing
uint8_t getSimulatedPinState() {
  return simulatedPinState;
}

// pressing the button using simulated pin state
void press(Button2& button) {
  // Set the simulated pin to pressed state
  simulatedPinState = BUTTON_ACTIVE;
  delay(5); // Allow state to settle
  button.loop();
}

// letting it go
void release(Button2& button) {
  // Set the simulated pin to released state
  simulatedPinState = !BUTTON_ACTIVE;
  delay(5); // Allow state to settle
  button.loop();
}

// emulate a button click with proper timing
void click(Button2& button, unsigned long duration) {
  // Press the button
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();
  
  // Hold for the specified duration
  delay(duration);
  button.loop();
  
  // Release the button
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
  
  // Small delay for state to settle
  delay(5);
  button.loop();
}

// Helper to initialize button with EpoxyDuino
Button2 createTestButton() {
  Button2 button;
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW);
  
  // Set up custom state function for testing
  button.setButtonStateFunction(getSimulatedPinState);
  
  // Initialize simulated state to released
  simulatedPinState = !BUTTON_ACTIVE;
  return button;
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
  delay(1000);
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
