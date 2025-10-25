# Button2 Test Suite - AI Assistant Guidelines

This file provides specific guidance for AI assistants (especially Claude Code) when working with the Button2 test suite.

## Test Suite Overview

The Button2 test suite consists of **68 comprehensive tests** organized into 6 test suites:

1. **test_basics** (6 tests) - Button initialization, configuration, and fundamental operations
2. **test_clicks** (12 tests) - Click detection (single, double, triple, long, patterns)
3. **test_callbacks** (12 tests) - All event handler callbacks
4. **test_states** (19 tests) - State management, queries, and timing edge cases
5. **test_configuration** (7 tests) - Settings management and property validation
6. **test_multiple** (12 tests) - Multiple button scenarios and interactions

## Testing Architecture

### Simulated Pin State Approach

**DO NOT use MockHardware** - it was removed from the library. Instead, use the **simulated pin state** approach:

```cpp
// Global simulated pin state
static uint8_t simulatedPinState = HIGH;

// Custom state function
uint8_t getSimulatedPinState() {
  return simulatedPinState;
}

// CRITICAL: Initialize in correct order
Button2 createTestButton() {
  Button2 button;

  // 1. Set simulated state FIRST
  simulatedPinState = HIGH;

  // 2. Set state function SECOND
  button.setButtonStateFunction(getSimulatedPinState);

  // 3. Initialize button LAST
  button.begin(BUTTON_PIN, INPUT_PULLUP, true);

  return button;
}
```

### Why This Order Matters

❌ **WRONG** - Will cause phantom clicks and state issues:
```cpp
button.begin(PIN, MODE, true);              // Reads undefined state
button.setButtonStateFunction(func);        // Too late!
simulatedPinState = HIGH;                   // Way too late!
```

✅ **CORRECT** - Proper initialization:
```cpp
simulatedPinState = HIGH;                   // 1. Set state first
button.setButtonStateFunction(func);        // 2. Inject reader
button.begin(PIN, MODE, true);              // 3. Now reads correct state
```

## Critical Testing Rules

### 1. Test Isolation (MANDATORY)

Each test MUST be completely independent:

```cpp
test(test_suite, test_name) {
    resetHandlerVars(); // ALWAYS reset global state first

    // Create LOCAL button instance
    Button2 button = createTestButton();
    button.resetPressedState();

    // Test logic here
}
```

**Why**: Global state pollution causes test interference and false failures.

### 2. Button Initialization Pattern (MANDATORY)

**ALWAYS** use this exact pattern for test button creation:

```cpp
Button2 createTestButton() {
  Button2 button;

  // CRITICAL ORDER:
  simulatedPinState = !BUTTON_ACTIVE;              // 1. State first
  button.setButtonStateFunction(getSimulatedPinState);  // 2. Function second
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW);  // 3. Init last

  return button;
}
```

### 3. Button Simulation Pattern

Use helper functions for button interactions:

```cpp
void click(Button2& button, unsigned long duration) {
  // Press
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();

  // Hold with periodic loop calls
  unsigned long startTime = millis();
  while (millis() < startTime + duration) {
    button.loop();
    delay(1);
  }

  // Release
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
  delay(5);
  button.loop();
}
```

**Critical**: Call `button.loop()` repeatedly while button is pressed to allow debouncing and click detection.

### 4. Callback Testing Pattern

```cpp
// Global test flags
static bool g_clicked = false;
static bool g_pressed = false;

void resetHandlerVars() {
  g_clicked = false;
  g_pressed = false;
  // ... reset all flags
}

test(callbacks, click_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setClickHandler([](Button2& btn) {
    g_clicked = true;
  });

  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);  // Wait for click to be reported
  button.loop();               // Process the timeout

  assertTrue(g_clicked);
}
```

## Test Suite Patterns

### test_basics/

Basic functionality and initialization tests:

```cpp
test(basics, equal_operator) {
  Button2 button1 = createTestButton();
  Button2 button2 = createTestButton();

  assertTrue(button1 == button1);
  assertFalse(button1 == button2);
}
```

### test_clicks/

Click detection tests:

```cpp
test(clicks, double_click) {
  Button2 button = createTestButton();
  button.resetPressedState();

  // Two quick clicks
  click(button, DEBOUNCE_MS);
  click(button, DEBOUNCE_MS);

  // Wait for double-click to be reported
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertEqual(button.getType(), double_click);
  assertEqual(button.getNumberOfClicks(), 2);
}
```

### test_callbacks/

Callback handler tests:

```cpp
test(callbacks, long_click_detected_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  button.setLongClickDetectedHandler([](Button2& b) {
    g_long_detected = true;
  });

  // Press and hold past threshold
  simulatedPinState = BUTTON_ACTIVE;
  button.loop();

  unsigned long startTime = millis();
  while (millis() - startTime < (BTN_LONGCLICK_MS + 100)) {
    button.loop();
    delay(1);
  }

  // Handler called WHILE still pressed
  assertTrue(g_long_detected);

  // Release
  simulatedPinState = !BUTTON_ACTIVE;
  button.loop();
}
```

### test_states/

State management and timing tests:

```cpp
test(states, is_pressed_raw) {
  Button2 button = createTestButton();

  // isPressedRaw reflects immediate pin state
  simulatedPinState = BUTTON_ACTIVE;
  assertTrue(button.isPressedRaw());

  simulatedPinState = !BUTTON_ACTIVE;
  assertFalse(button.isPressedRaw());
}
```

### test_configuration/

Configuration management tests:

```cpp
test(settings, longclick_time) {
  Button2 button = createTestButton();

  button.setLongClickTime(BTN_LONGCLICK_MS + 1);
  assertEqual(button.getLongClickTime(), (unsigned int)(BTN_LONGCLICK_MS + 1));
}
```

### test_multiple/

Multiple button interaction tests:

```cpp
test(multiple, shared_handler_different_buttons) {
  resetTestState();

  Button2 button1 = createTestButton(PIN1, func1, &state1);
  Button2 button2 = createTestButton(PIN2, func2, &state2);

  button1.setID(1);
  button2.setID(2);

  // Shared handler tracks which button was clicked
  auto sharedHandler = [](Button2& b) {
    g_last_button_id = b.getID();
  };

  button1.setClickHandler(sharedHandler);
  button2.setClickHandler(sharedHandler);

  click(button1, &state1, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button1.loop();

  assertEqual(g_last_button_id, 1);
}
```

## Common Pitfalls to Avoid

### 1. Initialization Order Error

❌ **WRONG**:
```cpp
Button2 button;
button.begin(PIN, MODE, true);
button.setButtonStateFunction(func);  // Too late - state already read!
```

✅ **CORRECT**:
```cpp
simulatedPinState = HIGH;
button.setButtonStateFunction(func);
button.begin(PIN, MODE, true);
```

### 2. Insufficient loop() Calls

❌ **WRONG**:
```cpp
simulatedPinState = LOW;
button.loop();
delay(100);  // Button state not processed!
simulatedPinState = HIGH;
button.loop();
```

✅ **CORRECT**:
```cpp
simulatedPinState = LOW;
button.loop();

while (millis() < endTime) {
  button.loop();  // Keep calling loop while pressed
  delay(1);
}

simulatedPinState = HIGH;
button.loop();
```

### 3. Missing Click Finalization

❌ **WRONG**:
```cpp
click(button, DEBOUNCE_MS);
// Check immediately - click not yet reported!
assertTrue(button.wasPressed());  // FAILS!
```

✅ **CORRECT**:
```cpp
click(button, DEBOUNCE_MS);
delay(BTN_DOUBLECLICK_MS);  // Wait for timeout
button.loop();               // Process timeout
assertTrue(button.wasPressed());  // NOW it works
```

### 4. Type Cast for assertEqual()

❌ **WRONG**:
```cpp
assertEqual(button.getDebounceTime(), 100);  // Ambiguous function call
```

✅ **CORRECT**:
```cpp
assertEqual(button.getDebounceTime(), (unsigned int)100);  // Clear type
```

### 5. Active HIGH/LOW Confusion

The `activeLow` parameter in `begin()` determines what pin state means "pressed":

```cpp
// Active LOW (pressed when pin is LOW)
button.begin(PIN, INPUT_PULLUP, true);   // activeLow=true → pressed=LOW
_pressedState = LOW;

// Active HIGH (pressed when pin is HIGH)
button.begin(PIN, INPUT, false);         // activeLow=false → pressed=HIGH
_pressedState = HIGH;
```

## Testing Dependencies

### Required Libraries

- **AUnit** (v1.7.1+) - Arduino unit testing framework
- **EpoxyDuino** (v1.6.0+) - Arduino API emulation for native testing

These are automatically installed via PlatformIO `lib_deps`.

### NO Dependencies

- ❌ **MockHardware** - Removed from library, not needed
- ❌ **Button2TestHelper** - Removed, tests use direct simulation

## Running Tests

### Quick Commands

```bash
# Run all test suites on ESP8266 emulator
pio test -e epoxy-esp8266 -v

# Run specific test suite
pio test -e test_clicks -v
pio test -e test_callbacks -v
pio test -e test_states -v

# Run all emulated platforms
pio test -e epoxy-esp8266 -e epoxy-esp32 -e epoxy-nano -v
```

### Expected Results

All 68 tests should pass:
- ✅ test_basics: 6 passed
- ✅ test_clicks: 12 passed
- ✅ test_callbacks: 12 passed
- ✅ test_states: 19 passed
- ✅ test_configuration: 7 passed
- ✅ test_multiple: 12 passed

Total execution time: ~34 seconds on epoxy-esp8266

## Adding New Tests

### Step-by-Step Process

1. **Choose appropriate test suite** based on what you're testing
2. **Copy existing test pattern** from that suite
3. **Reset state at beginning**: `resetHandlerVars()`
4. **Create local button**: `Button2 button = createTestButton()`
5. **Reset button state**: `button.resetPressedState()`
6. **Write test logic** using helper functions
7. **Add assertions** to verify behavior

### Example: Adding a New Callback Test

```cpp
test(callbacks, my_new_callback_test) {
  resetHandlerVars();                    // 1. Reset global state
  Button2 button = createTestButton();   // 2. Create local button
  button.resetPressedState();            // 3. Reset button state

  // 4. Set up callback
  button.setClickHandler([](Button2& btn) {
    g_clicked = true;
  });

  // 5. Simulate interaction
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  // 6. Verify behavior
  assertTrue(g_clicked);
}
```

## Debugging Failed Tests

### Enable Verbose Output

```bash
pio test -e epoxy-esp8266 -vv  # Very verbose
```

### Common Failure Patterns

| Failure | Cause | Solution |
|---------|-------|----------|
| Phantom clicks on init | Wrong init order | Set state before `begin()` |
| Click not detected | Not enough `loop()` calls | Call `loop()` during press |
| Wrong click type | Timing issue | Wait for timeout, call `loop()` |
| Callback not triggered | Missing finalization | Add delay + `loop()` after click |
| Intermittent failures | State pollution | Check `resetHandlerVars()` |

### Debug Pattern

```cpp
test(debug, investigate_issue) {
  resetHandlerVars();
  Button2 button = createTestButton();
  button.resetPressedState();

  Serial.print("Initial state: ");
  Serial.println(button.isPressed());

  // Your test logic here

  Serial.print("Final state: ");
  Serial.print(button.wasPressed());
  Serial.print(", type: ");
  Serial.print(button.getType());
  Serial.print(", count: ");
  Serial.println(button.getNumberOfClicks());
}
```

## Platform-Specific Notes

### EpoxyDuino Timing

EpoxyDuino uses system clock for `millis()`:
- ✅ Timing is generally accurate
- ⚠️ May have slight variations vs real hardware
- 💡 Add small timing buffers (e.g., `+10ms`) for boundary tests

### ESP8266 vs ESP32 vs Arduino

Tests are platform-agnostic:
- ✅ Same code works on all platforms
- ✅ No platform-specific features in core tests
- ✅ Platform differences handled in library, not tests

## Summary

**Remember the golden rules:**

1. ✅ Set `simulatedPinState` BEFORE `begin()`
2. ✅ Call `button.loop()` repeatedly while button is pressed
3. ✅ Wait for timeouts and call `loop()` before checking results
4. ✅ Reset all state with `resetHandlerVars()` at start of each test
5. ✅ Use local button instances, never global
6. ✅ Cast literals to `(unsigned int)` for assertEqual() with timing values

Follow these patterns and your tests will be robust, reliable, and maintainable!
