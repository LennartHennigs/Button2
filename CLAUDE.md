# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Button2 Library - AI Assistant Reference Guide

## Overview

Button2 is an Arduino/ESP library for handling physical buttons with advanced click detection, debouncing, and callback support. It's designed for embedded systems and IoT devices, providing robust button handling with support for single, double, triple, and long clicks.

**Compatibility**: This library is designed to work with both **Arduino IDE** and **PlatformIO**, maintaining compatibility with standard Arduino C++ (based on C++11/C++14) without advanced STL features or modern C++ constructs that might not be available in embedded environments.

## Development Commands

### Testing & Compilation

```bash
# Test compilation across all supported platforms
./test/compile_examples.sh                    # Run from repository root
cd test && ./compile_examples.sh             # Run from test directory

# Test specific configurations
./test/compile_examples.sh --verbose         # Verbose output
./test/compile_examples.sh --help            # Show available options
```

### PlatformIO Commands

```bash
# Build for default environment (Wemos D1 Mini)
pio run

# Build for specific platforms
pio run -e Wemos_D1_Mini                    # ESP8266 Wemos D1 Mini
pio run -e M5_ESP32                          # ESP32 M5Stack Core
pio run -e Arduino_Nano                      # Arduino Nano

# Upload to device
pio run -t upload -e Wemos_D1_Mini

# Monitor serial output
pio device monitor -p /dev/ttyUSB0 -b 115200
```

### Prerequisites for Testing

- **arduino-cli** must be installed and in PATH (for Arduino CLI testing)
- **PlatformIO** must be installed (for PlatformIO testing)
- Required Arduino cores:
  - `esp8266:esp8266` (for ESP8266 platforms)
  - `esp32:esp32` (for ESP32 platforms)  
  - `arduino:avr` (for Arduino Nano/Uno)
- Install cores: `arduino-cli core install esp8266:esp8266 esp32:esp32 arduino:avr`

## Target Platforms

- **Arduino**: Uno, Nano, Mega, etc.
- **ESP8266**: WiFi-enabled microcontrollers (Wemos D1 Mini, NodeMCU)
- **ESP32**: Dual-core WiFi/Bluetooth microcontrollers (M5Stack, DevKit)
- **Compatible**: Any Arduino-compatible board with digital I/O

## Arduino/PlatformIO Compatibility

### Development Environment Support

- **Arduino IDE**: Full compatibility with Arduino IDE 1.x and 2.x
- **PlatformIO**: Complete support in PlatformIO framework
- **Hardware abstraction**: Uses simulated pin state via `setButtonStateFunction()` for testing without physical buttons

### C++ Language Constraints

This library is designed for Arduino's embedded C++ environment, which means:

- **No STL**: No use of `std::vector`, `std::string`, `std::unique_ptr`, etc.
- **Basic C++**: Uses C++11/C++14 features available in Arduino (constexpr, nullptr)
- **Arduino String**: Uses Arduino's `String` class instead of `std::string`
- **Manual memory management**: Uses `new`/`delete` when needed
- **Function pointers**: Uses traditional function pointers and lambda functions
- **No exceptions**: Error handling via return codes and boolean checks

### What This Means for AI Assistants

When generating code for this library:

- ❌ Don't suggest `std::` anything
- ❌ Don't use complex `auto` type deduction
- ❌ Don't suggest advanced C++ features beyond C++14
- ❌ Don't use `std::shared_ptr` or smart pointers
- ✅ Use Arduino `String` class
- ✅ Use traditional C-style arrays or simple classes
- ✅ Use function pointers: `void (*callback)(Button2& btn)`
- ✅ Use lambda functions: `[](Button2& btn) { ... }`
- ✅ Use Arduino timing functions: `millis()`, `delay()`

## Architecture Overview

### Core Class Structure

- **Button2**: Main button handling class
  - Manages button state, debouncing, and click detection
  - Provides callback system for different button events
  - Supports both polling and interrupt-driven operation
  - Hardware abstraction through configurable I/O functions

- **Hardware Interface**: Abstraction layer
  - Default: Uses Arduino `digitalRead()` and `pinMode()`
  - Testing: Uses `setButtonStateFunction()` to inject custom state reader
  - Custom: Allows custom hardware interfaces

- **Click Types**: Enumerated button events
  - `single_click`: Single button press
  - `double_click`: Two rapid presses
  - `triple_click`: Three rapid presses
  - `long_click`: Extended press duration

### Memory Management Architecture

The library uses minimal dynamic allocation and provides safety mechanisms:

- **Stack-based operation**: Most data structures are stack-allocated
- **Configurable timeouts**: Debounce, double-click, and long-click timing
- **Callback system**: Function pointers and lambdas for event handling
- **Hardware abstraction**: Testable without physical hardware

## Core Concepts

### 1. Button Initialization

Basic button setup with pin configuration:

```cpp
#define BUTTON_PIN 2
Button2 button;

void setup() {
    // Initialize button on pin 2, INPUT_PULLUP mode, active LOW
    button.begin(BUTTON_PIN, INPUT_PULLUP, false);
    
    // Or with custom hardware interface
    button.begin(BUTTON_PIN, INPUT_PULLUP, false, &customHardware);
}
```

**Key Points for AI:**

- Pin mode can be `INPUT`, `INPUT_PULLUP`, or `INPUT_PULLDOWN`
- `inverted` parameter: `true` for active HIGH, `false` for active LOW
- Hardware interface is optional (defaults to Arduino digital I/O)

### 2. Callback System

Button2 uses a comprehensive callback system for different events:

```cpp
void setup() {
    button.begin(BUTTON_PIN);
    
    // Basic events
    button.setPressedHandler([](Button2& btn) {
        Serial.println("Button pressed");
    });
    
    button.setReleasedHandler([](Button2& btn) {
        Serial.println("Button released");
    });
    
    // Click events
    button.setClickHandler([](Button2& btn) {
        Serial.println("Single click");
    });
    
    button.setDoubleClickHandler([](Button2& btn) {
        Serial.println("Double click");
    });
    
    button.setTripleClickHandler([](Button2& btn) {
        Serial.println("Triple click");
    });
    
    button.setLongClickHandler([](Button2& btn) {
        Serial.println("Long click");
    });
    
    // Long click detection (while still pressed)
    button.setLongClickDetectedHandler([](Button2& btn) {
        Serial.println("Long click detected");
    });
}
```

**Key Points for AI:**

- Callbacks receive `Button2&` reference for accessing button properties
- Lambda functions are preferred for inline handlers
- Function pointers work for traditional C-style callbacks
- Multiple handlers can be set for different events

### 3. Button State and Properties

Access button state and configure behavior:

```cpp
void loop() {
    button.loop(); // Must be called regularly
    
    // State queries
    if (button.isPressed()) {
        // Button is currently pressed
    }
    
    if (button.wasPressed()) {
        // Button was pressed since last check
        Serial.print("Pressed for: ");
        Serial.print(button.wasPressedFor());
        Serial.println("ms");
        
        Serial.print("Click type: ");
        Serial.println(button.clickToString(button.getType()));
        
        Serial.print("Number of clicks: ");
        Serial.println(button.getNumberOfClicks());
    }
    
    // Reset pressed state
    button.resetPressedState();
}
```

### 4. Timing Configuration

Customize button timing behavior:

```cpp
void setup() {
    button.begin(BUTTON_PIN);
    
    // Configure timing (all in milliseconds)
    button.setDebounceTime(50);        // Debounce delay
    button.setDoubleClickTime(400);    // Max time between double clicks
    button.setLongClickTime(1000);     // Min time for long click
    
    // Query current settings
    Serial.print("Debounce: ");
    Serial.println(button.getDebounceTime());
}
```

### 5. Button Identification

Useful for managing multiple buttons:

```cpp
Button2 button1, button2;

void setup() {
    button1.begin(2);
    button1.setID(1);
    
    button2.begin(3);
    button2.setID(2);
    
    // Common handler for multiple buttons
    auto handler = [](Button2& btn) {
        Serial.print("Button ");
        Serial.print(btn.getID());
        Serial.println(" clicked");
    };
    
    button1.setClickHandler(handler);
    button2.setClickHandler(handler);
}
```

## Platform-Specific Features

### ESP32 Capacitive Touch

ESP32 supports capacitive touch sensing:

```cpp
// ESP32-specific capacitive touch
#ifdef ESP32
    button.begin(TOUCH_PIN, INPUT, true); // Capacitive touch pin
    button.setLongClickTime(2000); // Longer for touch sensitivity
#endif
```

### ESP32 Timer Interrupts

For precise timing without polling:

```cpp
#ifdef ESP32
    // Use hardware timer for button sampling
    hw_timer_t* timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &buttonInterrupt, true);
    timerAlarmWrite(timer, 1000, true); // 1ms sampling
    timerAlarmEnable(timer);
#endif
```

### Arduino Nano Considerations

Limited resources require careful configuration:

```cpp
// Optimize for limited memory
button.setDebounceTime(20);        // Shorter debounce
button.setDoubleClickTime(300);    // Faster double-click
```

## API Reference

### Core Methods

```cpp
// Initialization
Button2();
Button2(uint8_t pin, uint8_t mode = INPUT_PULLUP, bool inverted = true, HardwareInterface* hardware = nullptr);

// Setup
void begin(uint8_t pin, uint8_t mode = INPUT_PULLUP, bool inverted = true, HardwareInterface* hardware = nullptr);

// Runtime (must be called regularly)
void loop();

// State queries
bool isPressed() const;
bool wasPressed();
bool isPressedRaw() const;
int wasPressedFor() const;
clickType getType() const;
int getNumberOfClicks() const;

// Configuration
void setDebounceTime(unsigned int ms);
void setDoubleClickTime(unsigned int ms);
void setLongClickTime(unsigned int ms);
void setID(int id);

// State management
void resetPressedState();
void resetClickCount();

// Properties
unsigned int getDebounceTime() const;
unsigned int getDoubleClickTime() const;
unsigned int getLongClickTime() const;
int getID() const;
uint8_t getPin() const;

// Utility
String clickToString(clickType type) const;
bool operator==(Button2& other) const;
```

### Callback Methods

```cpp
// Event handlers
void setPressedHandler(CallbackFunction f);
void setReleasedHandler(CallbackFunction f);
void setChangedHandler(CallbackFunction f);
void setClickHandler(CallbackFunction f);
void setDoubleClickHandler(CallbackFunction f);
void setTripleClickHandler(CallbackFunction f);
void setLongClickHandler(CallbackFunction f);
void setLongClickDetectedHandler(CallbackFunction f);

// Callback function types
typedef std::function<void(Button2& btn)> CallbackFunction;
// Or traditional: typedef void (*CallbackFunction)(Button2& btn);
```

## Common Patterns

### 1. Simple Button

**See**: `examples/SingleButton/SingleButton.ino`

- Basic button setup with single click detection
- Demonstrates callback usage
- Hardware configuration examples

### 2. Multiple Click Types

**See**: `examples/MultiHandler/MultiHandler.ino`

- Single, double, triple, and long click detection
- Different actions for different click types
- Timing configuration examples

### 3. Multiple Buttons

**See**: `examples/MultipleButtons/MultipleButtons.ino`

- Managing multiple buttons with unique IDs
- Shared handlers with button identification
- Efficient polling patterns

### 4. Hardware Abstraction

**See**: `examples/CustomButtonStateHandler/CustomButtonStateHandler.ino`

- Custom hardware interfaces
- Alternative input methods
- Non-standard button configurations

### 5. ESP32 Capacitive Touch

**See**: `examples/ESP32CapacitiveTouch/ESP32CapacitiveTouch.ino`

- Capacitive touch button implementation
- ESP32-specific configuration
- Touch sensitivity adjustment

### 6. Long Press with Detection

**See**: `examples/LongpressHandler/LongpressHandler.ino`

- Long press detection while button is held
- Retriggerable long press events
- Timing-sensitive applications

### 7. Button Loop Alternative

**See**: `examples/ButtonLoop/ButtonLoop.ino`

- Blocking wait functions for simple applications
- `waitForClick()`, `waitForDouble()`, `waitForTriple()`
- Sequential button interactions

## Testing Framework

### Unit Testing

The library includes comprehensive unit tests using the AUnit framework:

**Dependencies:**
- **AUnit** (v1.7.1+) - Arduino unit testing framework by Brian T. Park
- **EpoxyDuino** (v1.6.0) - Arduino API emulation for native testing
- Automatically installed via PlatformIO `lib_deps`

**Running Tests:**

```bash
# PlatformIO - Native tests using EpoxyDuino (no hardware required)
pio test -e epoxy-esp8266        # Emulates ESP8266 (~2-3s)
pio test -e epoxy-esp32          # Emulates ESP32 (~2-3s)

# PlatformIO - Hardware tests on specific platform
pio test -e Wemos_D1_Mini_test   # Real ESP8266 hardware
pio test -e M5_ESP32_test        # Real ESP32 hardware  
pio test -e Arduino_Nano_test    # Real Arduino Nano hardware
```

**Test Coverage (68 tests across 6 suites):**

- **test_basics**: Initialization, configuration, default values (6 tests)
- **test_clicks**: Click detection - single, double, triple, long (12 tests)
- **test_callbacks**: All event handler callbacks (12 tests)
- **test_states**: State management, queries, timing edge cases (19 tests)
- **test_configuration**: Runtime settings and configuration (7 tests)
- **test_multiple**: Multiple button interactions (12 tests)

### Integration Testing

- Test all button types across different platforms
- Verify timing accuracy on different hardware
- Test hardware-specific features (capacitive touch, interrupts)
- Validate debouncing behavior

### Compilation Testing

Use the provided test script to verify compatibility:

```bash
./test/compile_examples.sh  # Tests all examples on all platforms
```

## Performance Notes

### Timing Characteristics

- Button state checking: ~5μs typical
- Debouncing overhead: ~1μs per check
- Callback execution: Depends on user code

### Memory Usage

- Per Button2 instance: ~100 bytes
- Stack usage during callbacks: ~50 bytes
- No dynamic allocation during normal operation

### Polling vs Interrupts

**Polling (Default)**:
- Call `button.loop()` regularly (every 1-10ms recommended)
- Simple implementation, no interrupt conflicts
- Suitable for most applications

**Interrupt-Driven**:
- More responsive, especially for fast clicks
- Requires careful interrupt handling
- Platform-specific implementation needed

## Hardware Considerations

### Pull-up/Pull-down Configuration

```cpp
// Internal pull-up (most common)
button.begin(PIN, INPUT_PULLUP, false); // Active LOW

// External pull-down
button.begin(PIN, INPUT, true);          // Active HIGH

// No pull resistor (external required)
button.begin(PIN, INPUT, false);         // Active LOW with external pull-up
```

### Debouncing

- Default debounce time: 50ms (suitable for most mechanical switches)
- Adjust based on switch quality and application needs
- Capacitive touch may need longer debounce times

### Power Considerations

- INPUT_PULLUP mode draws ~50μA through internal pull-up
- Sleep-friendly: can wake from deep sleep on pin change
- Consider external pull-up for ultra-low power applications

## Common Issues & Solutions

### 1. Missed Clicks

**Problem**: Button clicks not detected
**Solutions**:
- Ensure `button.loop()` is called frequently enough
- Check wiring and pull-up/pull-down configuration
- Adjust debounce time for switch characteristics
- Verify active HIGH/LOW setting matches hardware

### 2. False Triggers

**Problem**: Button triggers without being pressed
**Solutions**:
- Add external pull-up/pull-down resistor
- Increase debounce time
- Check for electrical noise sources
- Shield button wires if necessary

### 3. Double-Click Issues

**Problem**: Double-clicks not detected or false double-clicks
**Solutions**:
- Adjust double-click timeout (`setDoubleClickTime()`)
- Check mechanical switch bounce characteristics
- Ensure consistent pressing technique
- Consider switch quality upgrade

### 4. Long-Click Timing

**Problem**: Long-click detection too sensitive or insensitive
**Solutions**:
- Adjust long-click time (`setLongClickTime()`)
- Use `setLongClickDetectedHandler()` for immediate feedback
- Consider user experience and application requirements

## Arduino-Specific Considerations

### Setup Pattern

```cpp
#define BUTTON_PIN 2
Button2 button;

void setup() {
    Serial.begin(115200);
    
    // Initialize button
    button.begin(BUTTON_PIN, INPUT_PULLUP, false);
    
    // Set up handlers
    button.setClickHandler([](Button2& btn) {
        Serial.println("Click!");
    });
    
    Serial.println("Button initialized");
}

void loop() {
    button.loop(); // Essential - must be called regularly
    
    // Other application code
}
```

### Multiple Buttons Management

```cpp
Button2 buttons[4];
const uint8_t buttonPins[] = {2, 3, 4, 5};

void setup() {
    for (int i = 0; i < 4; i++) {
        buttons[i].begin(buttonPins[i]);
        buttons[i].setID(i);
        buttons[i].setClickHandler(commonHandler);
    }
}

void loop() {
    for (int i = 0; i < 4; i++) {
        buttons[i].loop();
    }
}

void commonHandler(Button2& btn) {
    Serial.print("Button ");
    Serial.print(btn.getID());
    Serial.println(" clicked");
}
```

### Non-blocking Operation

```cpp
void loop() {
    button.loop();
    
    // Other non-blocking tasks
    handleSensors();
    handleNetwork();
    handleDisplay();
    
    // Small delay to prevent overwhelming the system
    delay(1);
}
```

## Best Practices for AI Assistants

### Arduino/PlatformIO Compliance

1. **Stick to Arduino C++** - Use Arduino-compatible C++ features
2. **Use Arduino libraries** - `String`, `Serial`, `millis()`, etc.
3. **Function pointers and lambdas** - Both are supported in modern Arduino
4. **Memory awareness** - Consider embedded constraints

### Button2 Implementation

1. **Always call `button.loop()`** regularly in the main loop
2. **Use appropriate timing** for debounce, double-click, and long-click
3. **Choose correct pin mode** based on hardware configuration
4. **Set meaningful button IDs** for multiple button applications
5. **Use callbacks** for clean event-driven code
6. **Test with simulated pin state** using `setButtonStateFunction()` for reliable unit testing
7. **Consider platform differences** (ESP32 vs Arduino vs ESP8266)
8. **Handle edge cases** in timing-sensitive applications

## Debugging Tips

### Common Debug Patterns

```cpp
void setup() {
    Serial.begin(115200);
    
    button.setChangedHandler([](Button2& btn) {
        Serial.print("Button state changed: ");
        Serial.println(btn.isPressed() ? "PRESSED" : "RELEASED");
    });
    
    button.setClickHandler([](Button2& btn) {
        Serial.print("Click detected - Type: ");
        Serial.print(btn.clickToString(btn.getType()));
        Serial.print(", Duration: ");
        Serial.print(btn.wasPressedFor());
        Serial.print("ms, Count: ");
        Serial.println(btn.getNumberOfClicks());
    });
}
```

### Hardware Debugging

```cpp
void loop() {
    button.loop();
    
    // Debug raw pin state
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 1000) {
        Serial.print("Raw pin state: ");
        Serial.print(button.isPressedRaw() ? "HIGH" : "LOW");
        Serial.print(", Debounced: ");
        Serial.println(button.isPressed() ? "PRESSED" : "RELEASED");
        lastDebug = millis();
    }
}
```

This reference should help AI assistants understand and work with the Button2 library effectively, considering its Arduino/embedded context and specific button handling requirements.
