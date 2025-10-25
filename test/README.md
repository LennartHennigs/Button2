# Button2 Library Testing

This directory contains comprehensive testing infrastructure for the Button2 library, including unit tests, compilation tests, and testing utilities.

## Quick Start

### Running Unit Tests

```bash
# Fast native testing (recommended for development)
pio test -e epoxy-esp8266 -v    # ESP8266 emulation (~2-3 seconds)
pio test -e epoxy-esp32 -v      # ESP32 emulation (~2-3 seconds)

# Run specific test suites
pio test -e test_basics -v          # Basic functionality tests
pio test -e test_clicks -v          # Click detection tests
pio test -e test_callbacks -v       # Callback handler tests
pio test -e test_states -v          # State management tests
pio test -e test_configuration -v   # Configuration tests
pio test -e test_multiple -v        # Multiple button tests
```

### Running Compilation Tests

```bash
# Test all examples on all platforms
cd test && ./compile_examples.sh

# Test specific platform
./compile_examples.sh esp32

# Use Arduino CLI instead of PlatformIO
./compile_examples.sh --tool=arduino-cli

# Get help
./compile_examples.sh --help
```

## Test Suite Overview

The Button2 test suite is organized into comprehensive test categories:

### Test Suites (68 Total Tests)

#### 1. test_basics/ (6 tests)
- Button initialization and configuration
- Default value validation
- Pin configuration and properties
- Button equality operator
- String conversion utilities

#### 2. test_clicks/ (12 tests)
- **Single Click**: Basic click detection and timing
- **Double Click**: Rapid double-click within timeout window
- **Triple Click**: Triple-click detection
- **Long Click**: Long press detection and callback handling
- **More than 3 Clicks**: Overflow behavior (4+ clicks)
- **Not a Click**: Sub-debounce press rejection
- **Reset**: Click state reset functionality

#### 3. test_callbacks/ (12 tests)
- **Pressed Handler**: Button press event callbacks
- **Released Handler**: Button release event callbacks
- **Tap Handler**: Tap event notifications
- **Changed Handler**: State change callbacks (press + release)
- **Click Handler**: Single click callbacks
- **Double/Triple Click Handlers**: Multi-click callbacks
- **Long Click Handler**: Long press callbacks (on release)
- **Long Click Detected Handler**: Long press detection (while pressed)
- **Retriggerable Long Click**: Multiple long click triggers
- **Multiple Handlers**: Combined callback scenarios
- **Button Reference Validation**: Callback receives correct button

#### 4. test_states/ (19 tests)
- **State Queries**: `isPressed()`, `isPressedRaw()`, `wasPressed()`
- **Click Counting**: `getNumberOfClicks()`, `resetClickCount()`
- **Click Types**: `getType()` for all click types
- **Press Duration**: `wasPressedFor()` timing measurement
- **State Transitions**: Press/release state machine behavior
- **Active HIGH/LOW**: Both button polarity configurations
- **Long Click Counter**: `getLongClickCount()` tracking
- **Timing Edge Cases**:
  - Clicks at debounce boundary
  - Sub-debounce rejection
  - Double-click timeout boundaries
  - Long click threshold timing
  - Very fast/rapid clicks
  - Slow clicks becoming separate events

#### 5. test_configuration/ (7 tests)
- **Runtime Settings**: Debounce time, double-click time, long-click time
- **Button IDs**: Auto-assignment and custom ID setting
- **State Management**: `resetPressedState()` functionality
- **Handler Management**: Setting and replacing handlers

#### 6. test_multiple/ (12 tests)
- **Unique Button IDs**: Automatic ID assignment
- **Custom IDs**: Manual ID configuration
- **Shared Handlers**: Multiple buttons with single handler
- **Unique Handlers**: Independent handlers per button
- **Simultaneous Presses**: Concurrent button operations
- **Independent Counting**: Separate click counters
- **Different Configurations**: Per-button timing settings
- **Array Management**: Managing buttons in arrays
- **Button Equality**: Comparison operator
- **Independent States**: Isolated state management
- **Alternating Clicks**: Interleaved button operations

## Testing Infrastructure

### Test Architecture

The Button2 test suite uses a **simulated pin state** approach for hardware-independent testing:

```cpp
// Simulated pin state (global or per-test)
static uint8_t simulatedPinState = HIGH;

// Custom state function
uint8_t getSimulatedPinState() {
  return simulatedPinState;
}

// Button initialization
Button2 button;
simulatedPinState = HIGH; // Set initial state
button.setButtonStateFunction(getSimulatedPinState); // Inject state reader
button.begin(BUTTON_PIN, INPUT_PULLUP, true); // Initialize button

// Simulate button press
simulatedPinState = LOW; // Change to pressed state
button.loop(); // Process state change
```

This approach:
- ✅ **No hardware required** - Works in emulated environments
- ✅ **Deterministic** - Predictable, repeatable test conditions
- ✅ **Fast** - No I/O delays
- ✅ **Simple** - No complex mocking frameworks needed
- ✅ **Flexible** - Easy to simulate any pin state sequence

### Helper Functions

Each test suite includes helper functions for common operations:

```cpp
// Create a test button with simulated state
Button2 createTestButton() {
  Button2 button;
  simulatedPinState = HIGH; // Released
  button.setButtonStateFunction(getSimulatedPinState);
  button.begin(BUTTON_PIN, INPUT_PULLUP, true);
  return button;
}

// Simulate a button click with proper timing
void click(Button2& button, unsigned long duration) {
  simulatedPinState = LOW; // Press
  button.loop();

  // Hold for duration with periodic loop calls
  unsigned long startTime = millis();
  while (millis() < startTime + duration) {
    button.loop();
    delay(1);
  }

  simulatedPinState = HIGH; // Release
  button.loop();
}
```

### Test Frameworks

#### AUnit Framework
- Arduino-compatible unit testing framework
- Supports assertions, test organization, and reporting
- Works with both native emulation and real hardware
- Provides: `assertTrue()`, `assertFalse()`, `assertEqual()`, etc.

#### EpoxyDuino
- Arduino API emulation for native testing
- Provides `millis()`, `delay()`, `pinMode()`, `digitalRead()`, etc.
- Enables fast testing without hardware
- Platform-specific defines: `EPOXY_DUINO`, `ARDUINO_ESP8266`, etc.

### Test Environments

#### Native Testing Environments (Recommended)
- **epoxy-esp8266**: ESP8266 emulation (~2-3s per suite)
- **epoxy-esp32**: ESP32 emulation (~2-3s per suite)
- **epoxy-nano**: Arduino Nano emulation (~2-3s per suite)
- **Benefits**: Fast, deterministic, no hardware required

#### Dedicated Test Environments
- **test_basics**: Basic functionality tests only
- **test_clicks**: Click detection tests only
- **test_callbacks**: Callback handler tests only
- **test_states**: State management tests only
- **test_configuration**: Configuration tests only
- **test_multiple**: Multiple button tests only

## Running Tests

### Prerequisites

```bash
# Install PlatformIO (recommended)
pip install platformio

# All dependencies are automatically managed
```

### Unit Test Execution

#### Run All Tests
```bash
# All test suites on ESP8266 emulator
pio test -e epoxy-esp8266 -v

# All test suites on all emulators
pio test -e epoxy-esp8266 -e epoxy-esp32 -e epoxy-nano -v
```

#### Run Specific Test Suites
```bash
pio test -e test_basics -v         # Basic functionality
pio test -e test_clicks -v         # Click detection
pio test -e test_callbacks -v      # Callback handlers
pio test -e test_states -v         # State management
pio test -e test_configuration -v  # Configuration
pio test -e test_multiple -v       # Multiple buttons
```

#### Development Workflow
```bash
# Quick feedback during development
pio test -e epoxy-esp8266 -f test_clicks -v

# Test specific feature area
pio test -e test_callbacks -v

# Full validation before commit
pio test -e epoxy-esp8266 -e epoxy-esp32 -v
```

### Compilation Test Execution

```bash
cd test

# Test all platforms (default: PlatformIO)
./compile_examples.sh

# Test specific platform
./compile_examples.sh esp32
./compile_examples.sh Wemos
./compile_examples.sh nano

# Use Arduino CLI
./compile_examples.sh --tool=arduino-cli

# Verbose output for debugging
./compile_examples.sh --verbose
```

## Test Development Guidelines

### Adding New Tests

#### 1. Follow Established Patterns
```cpp
test(test_category, descriptive_name) {
  resetHandlerVars(); // Reset global test state

  Button2 button = createTestButton(); // Local instance
  button.resetPressedState();

  // Test logic
  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  // Assertions
  assertTrue(button.wasPressed());
  assertEqual(button.getType(), single_click);
}
```

#### 2. Test Initialization Pattern
```cpp
// Helper to create test button
Button2 createTestButton() {
  Button2 button;

  // IMPORTANT: Set state BEFORE begin()
  simulatedPinState = !BUTTON_ACTIVE;
  button.setButtonStateFunction(getSimulatedPinState);
  button.begin(BUTTON_PIN, BUTTON_MODE, BUTTON_ACTIVE == LOW);

  return button;
}
```

**Critical**: Initialize `simulatedPinState` and set the state function BEFORE calling `begin()` to avoid initialization issues.

#### 3. Test Categories
- **basics**: Fundamental functionality
- **clicks**: Click detection and patterns
- **callbacks**: Event handlers and callbacks
- **states**: State queries and transitions
- **timing**: Timing-related behavior
- **settings/configuration**: Configuration management
- **multiple**: Multiple button scenarios

### Best Practices

#### Test Isolation
- Each test MUST be completely independent
- Always reset global state (use `resetHandlerVars()` or equivalent)
- Use local instances, never global Button2 objects
- Don't rely on test execution order

#### Button State Simulation
```cpp
// CORRECT: Proper initialization order
simulatedPinState = HIGH;
button.setButtonStateFunction(getSimulatedPinState);
button.begin(PIN, MODE, true);

// WRONG: State function set after begin()
button.begin(PIN, MODE, true);
button.setButtonStateFunction(getSimulatedPinState); // Too late!
```

#### Timing Considerations
```cpp
// Use button's actual configuration
delay(button.getDoubleClickTime() + 10);

// Or use helper function
click(button, DEBOUNCE_MS);

// For long clicks
click(button, BTN_LONGCLICK_MS + 50);
```

#### Callback Testing
```cpp
// Global test flags
static bool g_clicked = false;

void resetHandlerVars() {
  g_clicked = false;
  // Reset other flags...
}

// In test
test(callbacks, click_handler) {
  resetHandlerVars();
  Button2 button = createTestButton();

  button.setClickHandler([](Button2& btn) {
    g_clicked = true;
  });

  click(button, DEBOUNCE_MS);
  delay(BTN_DOUBLECLICK_MS);
  button.loop();

  assertTrue(g_clicked);
}
```

### Common Patterns

#### Multiple Clicks
```cpp
// Double click
click(button, DEBOUNCE_MS);
click(button, DEBOUNCE_MS);
delay(BTN_DOUBLECLICK_MS);
button.loop();

// Triple click
click(button, DEBOUNCE_MS);
click(button, DEBOUNCE_MS);
click(button, DEBOUNCE_MS);
delay(BTN_DOUBLECLICK_MS);
button.loop();
```

#### Long Click Detection (While Pressed)
```cpp
button.setLongClickDetectedHandler([](Button2& b) {
  g_long_detected = true;
});

simulatedPinState = LOW; // Press
button.loop();

// Keep calling loop past threshold
unsigned long startTime = millis();
while (millis() - startTime < (BTN_LONGCLICK_MS + 100)) {
  button.loop();
  delay(1);
}

assertTrue(g_long_detected); // Called while still pressed
```

## Troubleshooting

### Common Problems

#### 1. Test Initialization Issues
**Problem**: Button shows incorrect initial state or phantom clicks
**Solution**: Ensure proper initialization order:
```cpp
// CORRECT ORDER:
simulatedPinState = HIGH;              // 1. Set state first
button.setButtonStateFunction(func);   // 2. Set state function
button.begin(PIN, MODE, true);         // 3. Initialize button
```

#### 2. Timing-Related Failures
**Problem**: Tests fail intermittently due to timing
**Solution**: Use button's configuration values and add small buffers:
```cpp
delay(button.getDebounceTime() + 10);  // Add buffer for timing variations
```

#### 3. Click Detection Issues
**Problem**: Clicks not detected or wrong type detected
**Solution**: Ensure `button.loop()` is called during press and after release:
```cpp
simulatedPinState = LOW;
button.loop(); // Process press

// Keep calling loop while pressed
while (millis() < endTime) {
  button.loop();
  delay(1);
}

simulatedPinState = HIGH;
button.loop(); // Process release
```

#### 4. Callback Not Triggered
**Problem**: Handler callback never called
**Solution**: Wait for click to be finalized:
```cpp
click(button, DEBOUNCE_MS);
delay(BTN_DOUBLECLICK_MS); // Wait for click to be reported
button.loop();             // Process the timeout
// Now callback has been called
```

## Performance Metrics

### Test Execution Times

| Test Suite | Tests | Duration (epoxy) |
|------------|-------|------------------|
| test_basics | 6 | ~2s |
| test_clicks | 12 | ~5s |
| test_callbacks | 12 | ~6s |
| test_states | 19 | ~11s |
| test_configuration | 7 | ~2s |
| test_multiple | 12 | ~8s |
| **Total** | **68** | **~34s** |

### Compilation Test Times

| Platform | Examples | Duration |
|----------|----------|----------|
| ESP8266 | All | ~1-2min |
| ESP32 | All | ~1-2min |
| Arduino Nano | Most | ~1-2min |
| **Total** | ~20 examples | ~3-5min |

## File Structure

```
test/
├── README.md                          # This file
├── CLAUDE.md                          # AI assistant guidelines
├── compile_examples.sh                # Compilation testing script
├── _utils/                            # Testing utilities
│   ├── Button2TestHelper.h            # Test helper (optional/unused)
│   └── Button2TestHelper.cpp          # Test helper (optional/unused)
├── test_basics/                       # Basic functionality tests
│   └── test_basics.cpp                # 6 tests
├── test_clicks/                       # Click detection tests
│   └── test_clicks.cpp                # 12 tests
├── test_callbacks/                    # Callback handler tests
│   └── test_callbacks.cpp             # 12 tests
├── test_states/                       # State management tests
│   └── test_states.cpp                # 19 tests
├── test_configuration/                # Configuration tests
│   └── test_configuration.cpp         # 7 tests
└── test_multiple/                     # Multiple button tests
    └── test_multiple.cpp              # 12 tests
```

## Continuous Integration

### GitHub Actions Example

```yaml
name: Button2 Tests

on: [push, pull_request]

jobs:
  unit-tests:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.x'
    - name: Install PlatformIO
      run: pip install platformio
    - name: Run all test suites
      run: pio test -e epoxy-esp8266 -e epoxy-esp32

  compilation-tests:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.x'
    - name: Install PlatformIO
      run: pip install platformio
    - name: Test compilation
      run: cd test && ./compile_examples.sh
```

### Local CI Simulation

```bash
# Full CI pipeline
pio test -e epoxy-esp8266 -e epoxy-esp32 && cd test && ./compile_examples.sh

# Quick validation
pio test -e epoxy-esp8266 && cd test && ./compile_examples.sh esp32
```

## Summary

This comprehensive testing infrastructure ensures the Button2 library is:
- ✅ **Robust**: 68 tests covering all functionality
- ✅ **Reliable**: Deterministic, hardware-independent tests
- ✅ **Fast**: Native testing provides quick feedback (~34s for all tests)
- ✅ **Maintainable**: Clear patterns and isolated tests
- ✅ **Cross-platform**: Works on ESP8266, ESP32, and Arduino
- ✅ **Well-documented**: Clear guidelines and examples
