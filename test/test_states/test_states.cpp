/////////////////////////////////////////////////////////////////
/*
  State management tests for Button2 library.
  Tests button state queries, transitions, and edge cases.

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

// Global state variable for button simulation
static uint8_t simulatedPinState = HIGH;

/////////////////////////////////////////////////////////////////

void setup_test_runner() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
}

/////////////////////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////////////////////

// Custom state function for testing
uint8_t getSimulatedPinState() {
  return simulatedPinState;
}

// Helper to initialize button with EpoxyDuino
Button2 createTestButton() {
  Button2 button;
  simulatedPinState = !BUTTON_ACTIVE;
  button.setButtonStateFunction(getSimulatedPinState);
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW);
  return button;
}

// Emulate a button click with proper timing
void click(Button2& button, unsigned long duration) {
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();

  unsigned long startTime = millis();
  unsigned long endTime = startTime + duration;

  while (millis() < endTime) {
    button.loop();
    delay(1);
  }

  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
  delay(5);
  button.loop();
}

/////////////////////////////////////////////////////////////////
// STATE MANAGEMENT TESTS
/////////////////////////////////////////////////////////////////

test(states, initial_state_released) {
  Button2 button = createTestButton();

  assertFalse(button.isPressed());
  assertFalse(button.isPressedRaw());
  assertFalse(button.wasPressed());
}

/////////////////////////////////////////////////////////////////

test(states, is_pressed_during_press) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // Press button
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();
  delay(DEBOUNCE_MS);
  button.loop();

  // Should be pressed now
  assertTrue(button.isPressed());
}

/////////////////////////////////////////////////////////////////

test(states, is_pressed_raw) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // isPressedRaw should reflect immediate pin state, even before debounce
  simulatedPinState = BUTTON_ACTIVE;
  assertTrue(button.isPressedRaw());

  simulatedPinState = !BUTTON_ACTIVE;
  assertFalse(button.isPressedRaw());
}

/////////////////////////////////////////////////////////////////

test(states, was_pressed_after_click) {
  Button2 button = createTestButton();
  button.resetPressedState();

  assertFalse(button.wasPressed());

  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertTrue(button.wasPressed());
}

/////////////////////////////////////////////////////////////////

test(states, was_pressed_for_duration) {
  Button2 button = createTestButton();
  button.resetPressedState();

  unsigned long pressDuration = 100;
  click(button, pressDuration);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  // Should be approximately the press duration
  unsigned long measuredDuration = button.wasPressedFor();

  // Allow some tolerance for timing variations in EpoxyDuino
  assertTrue(measuredDuration > 0);
  assertTrue(measuredDuration >= pressDuration - 20);
  assertTrue(measuredDuration <= pressDuration + 100);
}

/////////////////////////////////////////////////////////////////

test(states, get_number_of_clicks) {
  Button2 button = createTestButton();
  button.resetPressedState();

  assertEqual(button.getNumberOfClicks(), 0);

  // Single click
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertEqual(button.getNumberOfClicks(), 1);

  // Reset and double click
  button.resetPressedState();
  click(button, DEBOUNCE_MS);
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertEqual(button.getNumberOfClicks(), 2);
}

/////////////////////////////////////////////////////////////////

test(states, get_type_after_clicks) {
  Button2 button = createTestButton();

  // Single click
  button.resetPressedState();
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();
  assertEqual(button.getType(), single_click);

  // Double click
  button.resetPressedState();
  click(button, DEBOUNCE_MS);
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();
  assertEqual(button.getType(), double_click);

  // Triple click
  button.resetPressedState();
  click(button, DEBOUNCE_MS);
  click(button, DEBOUNCE_MS);
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();
  assertEqual(button.getType(), triple_click);

  // Long click
  button.resetPressedState();
  click(button, BTN_LONGCLICK_MS + 50);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();
  assertEqual(button.getType(), long_click);
}

/////////////////////////////////////////////////////////////////

test(states, reset_click_count) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // Double click
  click(button, DEBOUNCE_MS);
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertEqual(button.getNumberOfClicks(), 2);

  // Reset click count
  uint8_t count = button.resetClickCount();
  assertEqual(count, 2); // Returns old count
  assertEqual(button.getNumberOfClicks(), 0); // Now reset
}

/////////////////////////////////////////////////////////////////

test(states, state_transition_press_release) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // Initial: released
  assertFalse(button.isPressed());

  // Press
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();
  delay(DEBOUNCE_MS);
  button.loop();

  // Now: pressed
  assertTrue(button.isPressed());

  // Release
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
  delay(5);
  button.loop();

  // Now: released again
  assertFalse(button.isPressed());
}

/////////////////////////////////////////////////////////////////

test(states, multiple_presses_without_reset) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // First click
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();
  assertTrue(button.wasPressed());

  // wasPressed() should remain true until reset
  assertTrue(button.wasPressed());

  // Second click without reset
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  // Still true
  assertTrue(button.wasPressed());

  // Reset
  button.resetPressedState();
  assertFalse(button.wasPressed());
}

/////////////////////////////////////////////////////////////////

test(states, active_high_configuration) {
  Button2 button;
  simulatedPinState = LOW; // Released for active HIGH
  button.setButtonStateFunction(getSimulatedPinState);
  button.begin(BUTTON_PIN, INPUT, false); // activeLow=false means active HIGH (pressed when HIGH)

  button.resetPressedState();

  // Initial state should be released (pin is LOW, pressedState is HIGH)
  assertFalse(button.isPressed());

  // Press (set to HIGH for active HIGH)
  simulatedPinState = HIGH;
  button.loop();

  // Keep calling loop past debounce time
  unsigned long startTime = millis();
  while (millis() - startTime < (DEBOUNCE_MS + 10)) {
    button.loop();
    delay(1);
  }

  // Should be pressed now
  assertTrue(button.isPressed());
}

/////////////////////////////////////////////////////////////////

test(states, active_low_configuration) {
  Button2 button;
  simulatedPinState = HIGH; // Released for active LOW
  button.setButtonStateFunction(getSimulatedPinState);
  button.begin(BUTTON_PIN, INPUT_PULLUP, true); // activeLow=true means active LOW (pressed when LOW)

  button.resetPressedState();

  // Initial state should be released (pin is HIGH, pressedState is LOW)
  assertFalse(button.isPressed());

  // Press (set to LOW for active LOW)
  simulatedPinState = LOW;
  button.loop();

  // Keep calling loop past debounce time
  unsigned long startTime = millis();
  while (millis() - startTime < (DEBOUNCE_MS + 10)) {
    button.loop();
    delay(1);
  }

  // Should be pressed now
  assertTrue(button.isPressed());
}

/////////////////////////////////////////////////////////////////

test(states, long_click_counter) {
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setLongClickDetectedRetriggerable(true);
  button.setLongClickDetectedHandler([](Button2& b) {
    // Handler needed for long click detection
  });

  // Press and hold for 3x long click time
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();

  unsigned long startTime = millis();
  while (millis() - startTime < (BTN_LONGCLICK_MS * 3 + 100)) {
    button.loop();
    delay(1);
  }

  // Check counter
  int count = button.getLongClickCount();
  assertTrue(count >= 3);

  // Release
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
}

/////////////////////////////////////////////////////////////////
// TIMING EDGE CASE TESTS
/////////////////////////////////////////////////////////////////

test(timing, click_at_debounce_boundary) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // Click for slightly more than debounce time to account for timing variations
  click(button, BTN_DEBOUNCE_MS + 5);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  // Should register as a valid click
  assertTrue(button.wasPressed());
  assertEqual(button.getNumberOfClicks(), 1);
}

/////////////////////////////////////////////////////////////////

test(timing, click_just_under_debounce) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // Click for just under debounce time
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();
  delay(BTN_DEBOUNCE_MS - 10);
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();

  delay(BTN_DOUBLECLICK_MS + 50);
  button.loop();

  // Should NOT register as a valid click
  assertFalse(button.wasPressed());
}

/////////////////////////////////////////////////////////////////

test(timing, double_click_at_timeout_boundary) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // First click
  click(button, DEBOUNCE_MS);

  // Wait almost to the double-click timeout
  delay(BTN_DOUBLECLICK_MS - 50);

  // Second click just before timeout
  click(button, DEBOUNCE_MS);

  // Wait for final timeout
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  // Should register as double click
  assertEqual(button.getType(), double_click);
  assertEqual(button.getNumberOfClicks(), 2);
}

/////////////////////////////////////////////////////////////////

test(timing, long_click_at_threshold) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // Click for EXACTLY long click time
  click(button, BTN_LONGCLICK_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  // Should register as long click
  assertEqual(button.getType(), long_click);
}

/////////////////////////////////////////////////////////////////

test(timing, very_fast_clicks) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // Rapid triple click with minimal gaps
  click(button, DEBOUNCE_MS);
  delay(10); // Very short gap
  click(button, DEBOUNCE_MS);
  delay(10); // Very short gap
  click(button, DEBOUNCE_MS);

  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  // Should still register as triple click
  assertEqual(button.getType(), triple_click);
  assertEqual(button.getNumberOfClicks(), 3);
}

/////////////////////////////////////////////////////////////////

test(timing, slow_clicks_become_separate) {
  Button2 button = createTestButton();
  button.resetPressedState();

  bool firstClick = false;
  button.setClickHandler([&firstClick](Button2& b) {
    firstClick = true;
  });

  // First click
  click(button, DEBOUNCE_MS);

  // Wait beyond double-click timeout
  delay(BTN_DOUBLECLICK_MS + 100);
  button.loop();

  // Should have registered first click
  assertTrue(firstClick);
  assertEqual(button.getNumberOfClicks(), 1);

  // Reset and second click
  button.resetPressedState();
  firstClick = false;

  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS + 100);
  button.loop();

  // Should register as separate single click, not a double click
  assertTrue(firstClick);
  assertEqual(button.getNumberOfClicks(), 1);
}

/////////////////////////////////////////////////////////////////

void setup() {
  setup_test_runner();

  // Reduced delay for faster native testing (was 1000ms)
  delay(100);
  Serial.begin(SERIAL_SPEED);
  while(!Serial) {}
  Serial.println(F("\n\nButton2 State Management & Timing Tests"));
  Serial.println(F("Using EpoxyDuino + PlatformIO"));
}

/////////////////////////////////////////////////////////////////

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
