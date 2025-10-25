/////////////////////////////////////////////////////////////////
/*
  Multiple button tests for Button2 library.
  Tests managing multiple buttons, unique IDs, and simultaneous operations.

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

#define BUTTON1_PIN     37
#define BUTTON2_PIN     38
#define BUTTON3_PIN     39
#define BUTTON_MODE     INPUT_PULLUP
#define BUTTON_ACTIVE   LOW

#define DEBOUNCE_MS     BTN_DEBOUNCE_MS + 5

/////////////////////////////////////////////////////////////////

// Simulated pin states for multiple buttons
static uint8_t simulatedPin37State = HIGH;
static uint8_t simulatedPin38State = HIGH;
static uint8_t simulatedPin39State = HIGH;

// Global test state
static int g_button1_clicks = 0;
static int g_button2_clicks = 0;
static int g_button3_clicks = 0;
static int g_last_button_id = -1;

/////////////////////////////////////////////////////////////////

void setup_test_runner() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
}

/////////////////////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////////////////////

// Custom state functions for each pin
uint8_t getSimulatedPin37State() {
  return simulatedPin37State;
}

uint8_t getSimulatedPin38State() {
  return simulatedPin38State;
}

uint8_t getSimulatedPin39State() {
  return simulatedPin39State;
}

// Helper to initialize button with specific pin and state function
Button2 createTestButton(uint8_t pin, uint8_t (*stateFunction)(), uint8_t* stateVar) {
  Button2 button;
  *stateVar = !BUTTON_ACTIVE;
  button.setButtonStateFunction(stateFunction);
  button.begin(pin, BUTTON_MODE, BUTTON_ACTIVE == LOW);
  return button;
}

// Emulate a button click with proper timing
void click(Button2& button, uint8_t* stateVar, unsigned long duration) {
  *stateVar = BUTTON_ACTIVE;
  button.loop();

  unsigned long startTime = millis();
  unsigned long endTime = startTime + duration;

  while (millis() < endTime) {
    button.loop();
    delay(1);
  }

  *stateVar = !BUTTON_ACTIVE;
  button.loop();
  delay(5);
  button.loop();
}

// Reset all test state
void resetTestState() {
  g_button1_clicks = 0;
  g_button2_clicks = 0;
  g_button3_clicks = 0;
  g_last_button_id = -1;
  simulatedPin37State = HIGH;
  simulatedPin38State = HIGH;
  simulatedPin39State = HIGH;
}

/////////////////////////////////////////////////////////////////
// MULTIPLE BUTTON TESTS
/////////////////////////////////////////////////////////////////

test(multiple, unique_button_ids) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);
  Button2 button3 = createTestButton(BUTTON3_PIN, getSimulatedPin39State, &simulatedPin39State);

  int id1 = button1.getID();
  int id2 = button2.getID();
  int id3 = button3.getID();

  // All IDs should be different
  assertNotEqual(id1, id2);
  assertNotEqual(id2, id3);
  assertNotEqual(id1, id3);

  // All IDs should be non-negative
  assertTrue(id1 >= 0);
  assertTrue(id2 >= 0);
  assertTrue(id3 >= 0);
}

/////////////////////////////////////////////////////////////////

test(multiple, custom_button_ids) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);

  button1.setID(100);
  button2.setID(200);

  assertEqual(button1.getID(), 100);
  assertEqual(button2.getID(), 200);
}

/////////////////////////////////////////////////////////////////

test(multiple, shared_handler_different_buttons) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);

  button1.setID(1);
  button2.setID(2);
  button1.resetPressedState();
  button2.resetPressedState();

  // Shared handler that tracks which button was clicked
  auto sharedHandler = [](Button2& b) {
    g_last_button_id = b.getID();
  };

  button1.setClickHandler(sharedHandler);
  button2.setClickHandler(sharedHandler);

  // Click button1
  click(button1, &simulatedPin37State, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button1.loop();

  assertEqual(g_last_button_id, 1);

  // Click button2
  click(button2, &simulatedPin38State, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button2.loop();

  assertEqual(g_last_button_id, 2);
}

/////////////////////////////////////////////////////////////////

test(multiple, unique_handlers_per_button) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);

  button1.resetPressedState();
  button2.resetPressedState();

  button1.setClickHandler([](Button2& b) {
    g_button1_clicks++;
  });

  button2.setClickHandler([](Button2& b) {
    g_button2_clicks++;
  });

  // Click button1
  click(button1, &simulatedPin37State, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button1.loop();

  assertEqual(g_button1_clicks, 1);
  assertEqual(g_button2_clicks, 0);

  // Click button2
  click(button2, &simulatedPin38State, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button2.loop();

  assertEqual(g_button1_clicks, 1);
  assertEqual(g_button2_clicks, 1);
}

/////////////////////////////////////////////////////////////////

test(multiple, simultaneous_button_presses) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);

  button1.resetPressedState();
  button2.resetPressedState();

  // Press both buttons simultaneously
  simulatedPin37State = BUTTON_ACTIVE;
  simulatedPin38State = BUTTON_ACTIVE;

  button1.loop();
  button2.loop();

  delay(DEBOUNCE_MS);

  button1.loop();
  button2.loop();

  // Both should be pressed
  assertTrue(button1.isPressed());
  assertTrue(button2.isPressed());

  // Release both
  simulatedPin37State = !BUTTON_ACTIVE;
  simulatedPin38State = !BUTTON_ACTIVE;

  button1.loop();
  button2.loop();

  // Both should be released
  assertFalse(button1.isPressed());
  assertFalse(button2.isPressed());
}

/////////////////////////////////////////////////////////////////

test(multiple, independent_click_counting) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);

  button1.resetPressedState();
  button2.resetPressedState();

  // Single click button1
  click(button1, &simulatedPin37State, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button1.loop();

  assertEqual(button1.getNumberOfClicks(), 1);
  assertEqual(button2.getNumberOfClicks(), 0);

  // Double click button2
  click(button2, &simulatedPin38State, DEBOUNCE_MS);
  click(button2, &simulatedPin38State, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button2.loop();

  assertEqual(button1.getNumberOfClicks(), 1);
  assertEqual(button2.getNumberOfClicks(), 2);
}

/////////////////////////////////////////////////////////////////

test(multiple, different_timing_configurations) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);

  // Configure button1 with longer timeouts
  button1.setDebounceTime(100);
  button1.setLongClickTime(500);

  // Configure button2 with shorter timeouts
  button2.setDebounceTime(30);
  button2.setLongClickTime(150);

  // Verify independent configuration
  assertEqual(button1.getDebounceTime(), (unsigned int)100);
  assertEqual(button1.getLongClickTime(), (unsigned int)500);
  assertEqual(button2.getDebounceTime(), (unsigned int)30);
  assertEqual(button2.getLongClickTime(), (unsigned int)150);
}

/////////////////////////////////////////////////////////////////

test(multiple, array_of_buttons) {
  resetTestState();

  const int NUM_BUTTONS = 3;
  Button2 buttons[NUM_BUTTONS];
  uint8_t pins[NUM_BUTTONS] = {BUTTON1_PIN, BUTTON2_PIN, BUTTON3_PIN};
  uint8_t (*stateFuncs[NUM_BUTTONS])() = {
    getSimulatedPin37State,
    getSimulatedPin38State,
    getSimulatedPin39State
  };
  uint8_t* stateVars[NUM_BUTTONS] = {
    &simulatedPin37State,
    &simulatedPin38State,
    &simulatedPin39State
  };

  // Initialize all buttons
  for (int i = 0; i < NUM_BUTTONS; i++) {
    *stateVars[i] = !BUTTON_ACTIVE;
    buttons[i].setButtonStateFunction(stateFuncs[i]);
    buttons[i].begin(pins[i], BUTTON_MODE, BUTTON_ACTIVE == LOW);
    buttons[i].setID(i);
    buttons[i].resetPressedState();
  }

  // Shared handler
  auto handler = [](Button2& b) {
    g_last_button_id = b.getID();
  };

  // Set handler for all buttons
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].setClickHandler(handler);
  }

  // Click each button in sequence
  for (int i = 0; i < NUM_BUTTONS; i++) {
    click(buttons[i], stateVars[i], DEBOUNCE_MS);
    delay(BTN_DOUBLECLICK_MS);
    buttons[i].loop();

    assertEqual(g_last_button_id, i);
    assertEqual(buttons[i].getNumberOfClicks(), 1);
  }
}

/////////////////////////////////////////////////////////////////

test(multiple, button_equality_operator) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);

  // Button should equal itself
  assertTrue(button1 == button1);
  assertTrue(button2 == button2);

  // Different buttons should not be equal
  assertFalse(button1 == button2);
}

/////////////////////////////////////////////////////////////////

test(multiple, independent_state_management) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);

  button1.resetPressedState();
  button2.resetPressedState();

  // Click button1
  click(button1, &simulatedPin37State, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button1.loop();

  // button1 should show wasPressed, button2 should not
  assertTrue(button1.wasPressed());
  assertFalse(button2.wasPressed());

  // Reset button1
  button1.resetPressedState();

  assertFalse(button1.wasPressed());
  assertFalse(button2.wasPressed());

  // Click button2
  click(button2, &simulatedPin38State, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button2.loop();

  // button2 should show wasPressed, button1 should not
  assertFalse(button1.wasPressed());
  assertTrue(button2.wasPressed());
}

/////////////////////////////////////////////////////////////////

test(multiple, alternating_button_clicks) {
  resetTestState();

  Button2 button1 = createTestButton(BUTTON1_PIN, getSimulatedPin37State, &simulatedPin37State);
  Button2 button2 = createTestButton(BUTTON2_PIN, getSimulatedPin38State, &simulatedPin38State);

  button1.setID(1);
  button2.setID(2);
  button1.resetPressedState();
  button2.resetPressedState();

  button1.setClickHandler([](Button2& b) { g_button1_clicks++; });
  button2.setClickHandler([](Button2& b) { g_button2_clicks++; });

  // Alternate clicks
  for (int i = 0; i < 3; i++) {
    // Click button1
    click(button1, &simulatedPin37State, DEBOUNCE_MS);
    delay(BTN_DOUBLECLICK_MS);
    button1.loop();

    // Click button2
    click(button2, &simulatedPin38State, DEBOUNCE_MS);
    delay(BTN_DOUBLECLICK_MS);
    button2.loop();
  }

  assertEqual(g_button1_clicks, 3);
  assertEqual(g_button2_clicks, 3);
}

/////////////////////////////////////////////////////////////////

void setup() {
  setup_test_runner();

  delay(1000);
  Serial.begin(SERIAL_SPEED);
  while(!Serial) {}
  Serial.println(F("\n\nButton2 Multiple Button Tests"));
  Serial.println(F("Using EpoxyDuino + PlatformIO"));
}

/////////////////////////////////////////////////////////////////

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
