/////////////////////////////////////////////////////////////////
/*
  Callback handler tests for Button2 library.
  Tests all event callbacks: pressed, released, changed, tap, click types.

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
static bool g_pressed = false;
static bool g_released = false;
static bool g_tap = false;
static bool g_changed = false;
static bool g_click = false;
static bool g_double_click = false;
static bool g_triple_click = false;
static bool g_long_click = false;
static bool g_long_detected = false;
static int g_changed_count = 0;
static int g_long_detected_count = 0;
static Button2* g_callback_button = nullptr;

/////////////////////////////////////////////////////////////////

void setup_test_runner() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
}

/////////////////////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////////////////////

// Global state variable for button simulation
static uint8_t simulatedPinState = HIGH;

// Custom state function for testing
uint8_t getSimulatedPinState() {
  return simulatedPinState;
}

// Helper to initialize button with EpoxyDuino
Button2 createTestButton() {
  Button2 button;

  // Initialize simulated state to released BEFORE begin()
  simulatedPinState = !BUTTON_ACTIVE;

  // Set up custom state function for testing BEFORE begin()
  button.setButtonStateFunction(getSimulatedPinState);

  // Now initialize the button - it will read the correct state
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW);

  return button;
}

// Emulate a button click with proper timing
void click(Button2& button, unsigned long duration) {
  // Press the button
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();

  // Hold for the specified duration with periodic loop calls
  unsigned long startTime = millis();
  unsigned long endTime = startTime + duration;

  // Keep calling loop until we reach the target duration
  while (millis() < endTime) {
    button.loop();
    delay(1);
  }

  // Release the button
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();

  // Small delay for state to settle
  delay(5);
  button.loop();
}

// Reset all handler variables
void resetHandlerVars() {
  g_pressed = false;
  g_released = false;
  g_tap = false;
  g_changed = false;
  g_click = false;
  g_double_click = false;
  g_triple_click = false;
  g_long_click = false;
  g_long_detected = false;
  g_changed_count = 0;
  g_long_detected_count = 0;
  g_callback_button = nullptr;
}

/////////////////////////////////////////////////////////////////
// CALLBACK HANDLER TESTS
/////////////////////////////////////////////////////////////////

test(callbacks, pressed_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setPressedHandler([](Button2& b) {
    g_pressed = true;
    g_callback_button = &b;
  });

  // Press button
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();
  delay(DEBOUNCE_MS);
  button.loop();

  assertTrue(g_pressed);
  assertEqual(g_callback_button, &button);
}

/////////////////////////////////////////////////////////////////

test(callbacks, released_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setReleasedHandler([](Button2& b) {
    g_released = true;
    g_callback_button = &b;
  });

  // Press and release
  click(button, DEBOUNCE_MS);

  assertTrue(g_released);
  assertEqual(g_callback_button, &button);
}

/////////////////////////////////////////////////////////////////

test(callbacks, tap_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setTapHandler([](Button2& b) {
    g_tap = true;
  });

  // Tap the button
  click(button, DEBOUNCE_MS);

  assertTrue(g_tap);
}

/////////////////////////////////////////////////////////////////

test(callbacks, changed_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setChangedHandler([](Button2& b) {
    g_changed = true;
    g_changed_count++;
  });

  // Press (should trigger changed)
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();
  delay(DEBOUNCE_MS);
  button.loop();

  assertTrue(g_changed);
  assertEqual(g_changed_count, 1);

  // Release (should trigger changed again)
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
  delay(5);
  button.loop();

  assertEqual(g_changed_count, 2);
}

/////////////////////////////////////////////////////////////////

test(callbacks, click_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setClickHandler([](Button2& b) {
    g_click = true;
  });

  // Single click
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertTrue(g_click);
}

/////////////////////////////////////////////////////////////////

test(callbacks, double_click_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setDoubleClickHandler([](Button2& b) {
    g_double_click = true;
  });

  // Double click
  click(button, DEBOUNCE_MS);
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertTrue(g_double_click);
}

/////////////////////////////////////////////////////////////////

test(callbacks, triple_click_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setTripleClickHandler([](Button2& b) {
    g_triple_click = true;
  });

  // Triple click
  click(button, DEBOUNCE_MS);
  click(button, DEBOUNCE_MS);
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertTrue(g_triple_click);
}

/////////////////////////////////////////////////////////////////

test(callbacks, long_click_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setLongClickHandler([](Button2& b) {
    g_long_click = true;
  });

  // Long click
  click(button, BTN_LONGCLICK_MS + 50);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertTrue(g_long_click);
}

/////////////////////////////////////////////////////////////////

test(callbacks, long_click_detected_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setLongClickDetectedHandler([](Button2& b) {
    g_long_detected = true;
    g_long_detected_count++;
  });

  // Press and hold past long click threshold
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();

  // Keep calling loop past the long click threshold
  unsigned long startTime = millis();
  while (millis() - startTime < (BTN_LONGCLICK_MS + 100)) {
    button.loop();
    delay(1);
  }

  // Long click detected should have been called WHILE still pressed
  assertTrue(g_long_detected);
  assertEqual(g_long_detected_count, 1);

  // Release
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
}

/////////////////////////////////////////////////////////////////

test(callbacks, long_click_detected_retriggerable) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setLongClickDetectedRetriggerable(true);
  button.setLongClickDetectedHandler([](Button2& b) {
    g_long_detected_count++;
  });

  // Press and hold for 2x long click time
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();

  unsigned long startTime = millis();
  while (millis() - startTime < (BTN_LONGCLICK_MS * 2 + 100)) {
    button.loop();
    delay(1);
  }

  // Should have been called twice (retriggerable)
  assertTrue(g_long_detected_count >= 2);

  // Release
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
}

/////////////////////////////////////////////////////////////////

test(callbacks, multiple_handlers_on_click) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  // Set multiple handlers
  button.setPressedHandler([](Button2& b) { g_pressed = true; });
  button.setReleasedHandler([](Button2& b) { g_released = true; });
  button.setTapHandler([](Button2& b) { g_tap = true; });
  button.setClickHandler([](Button2& b) { g_click = true; });
  button.setChangedHandler([](Button2& b) { g_changed_count++; });

  // Single click
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  // All handlers should have been called
  assertTrue(g_pressed);
  assertTrue(g_released);
  assertTrue(g_tap);
  assertTrue(g_click);
  assertEqual(g_changed_count, 2); // Press + Release
}

/////////////////////////////////////////////////////////////////

test(callbacks, handler_gets_correct_button_reference) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  int buttonID = 42;
  button.setID(buttonID);

  int receivedID = -1;
  button.setClickHandler([&receivedID](Button2& b) {
    receivedID = b.getID();
  });

  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertEqual(receivedID, buttonID);
}

/////////////////////////////////////////////////////////////////

void setup() {
  setup_test_runner();

  delay(100);  // Reduced for faster native testing
  Serial.begin(SERIAL_SPEED);
  while(!Serial) {}
  Serial.println(F("\n\nButton2 Callback Tests"));
  Serial.println(F("Using EpoxyDuino + PlatformIO"));
}

/////////////////////////////////////////////////////////////////

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
