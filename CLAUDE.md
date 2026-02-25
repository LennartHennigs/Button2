# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Button2 is an Arduino/ESP library for handling physical buttons with advanced click detection, debouncing, and callback support. Targets Arduino Uno/Nano (AVR), ESP8266, and ESP32.

## Development Commands

```bash
# Fast native tests (no hardware required) — recommended
pio test -e epoxy-esp8266        # Emulates ESP8266
pio test -e epoxy-esp32          # Emulates ESP32
pio test -e epoxy-nano           # Emulates Arduino Nano

# Specific test suites
pio test -e test_basics
pio test -e test_clicks
pio test -e test_callbacks
pio test -e test_states
pio test -e test_configuration
pio test -e test_multiple

# Compile all examples across all platforms
./test/compile_examples.sh

# Build / upload
pio run -e Wemos_D1_Mini
pio run -t upload -e Wemos_D1_Mini
```

Prerequisites: `arduino-cli`, PlatformIO, and cores `esp8266:esp8266 esp32:esp32 arduino:avr`.

## Code Generation Rules

This is an **embedded Arduino library**. When writing or modifying code:

### Never use

- `std::vector`, `std::string`, `std::unique_ptr`, `std::shared_ptr`, or any STL containers
- Smart pointers of any kind
- Exceptions or `try`/`catch`
- `std::size_t` — use `uint8_t` / `unsigned int` instead
- Complex `auto` type deduction
- C++ features beyond C++14

### Always use

- `uint8_t`, `unsigned int`, `unsigned long` for sizes and times — **never `long` for timing** (would break after 24.8 days on AVR/ESP; use `unsigned long` to match `millis()`)
- Arduino `String` class (not `std::string`)
- C-style arrays, not containers
- `millis()`, `delay()` for timing
- `clickType::empty` with explicit scope (avoids conflict with C++17 `std::empty()`)
- `activeLow = true` means button pressed when pin reads **LOW** (the default, for INPUT_PULLUP)
- `activeLow = false` means button pressed when pin reads **HIGH**

### Callbacks — platform-dependent

On **ESP32/ESP8266** (C++11+, non-AVR): `std::function` and lambdas are available:

```cpp
button.setClickHandler([](Button2& btn) { Serial.println("click"); });
```

On **AVR (Uno/Nano/Mega)**: plain function pointers only:

```cpp
void onClick(Button2& btn) { Serial.println("click"); }
button.setClickHandler(onClick);
```

### Testing pattern

**Order matters**: set state function and initial state *before* `begin()`, so the initial state read in `begin()` uses the injected function.

```cpp
static uint8_t pinState = HIGH;
uint8_t getState() { return pinState; }

pinState = HIGH;                                        // 1. set initial state
button.setButtonStateFunction(getState);               // 2. inject function
button.begin(BTN_VIRTUAL_PIN, INPUT_PULLUP, true);     // 3. begin() reads state
```

All six test suites share helpers via `test/shared/test_helpers.h` (`createTestButton()`, `click()`, `press()`, `release()`). Add new tests there, not inline in a single suite.

## Architecture

`Button2` is a single class (no inheritance hierarchy). Key internal flow:

- `loop()` reads pin state, dispatches to `_handlePress()` or `_handleRelease()`
- Press is only counted after `debounce_time_ms` of continuous press (`pressed_triggered` flag)
- Release shorter than `debounce_time_ms` is ignored (bounce rejection)
- After release, `_reportClicks()` fires after `doubleclick_time_ms` timeout
- Long-click fires via `_checkForLongClick()` only when `click_count == 1` (prevents ambiguity with multi-click sequences)
- Memory layout in `Button2.h` is ordered by size to minimize padding on AVR

## Important Constants

```cpp
const unsigned int BTN_DEBOUNCE_MS    = 50;   // debounce window
const unsigned int BTN_LONGCLICK_MS   = 200;  // long-click threshold
const unsigned int BTN_DOUBLECLICK_MS = 300;  // multi-click timeout

const unsigned int BTN_UNDEFINED_PIN  = 255;  // uninitialized
const unsigned int BTN_VIRTUAL_PIN    = 254;  // no GPIO (I2C, touch, etc.)

enum clickType { single_click, double_click, triple_click, long_click, empty };
// Use clickType::empty (scoped) to avoid conflict with std::empty()
```

## API Reference

```cpp
// Constructors
Button2();
Button2(uint8_t attachTo, uint8_t buttonMode = INPUT_PULLUP, bool activeLow = true);

// Setup
// initCallback: optional — called once in begin() for hardware init (I2C, SPI, etc.)
void begin(uint8_t attachTo, uint8_t buttonMode = INPUT_PULLUP, bool activeLow = true,
           InitCallbackFunction initCallback = BUTTON2_NULL);
void setButtonStateFunction(StateCallbackFunction f);  // inject custom state reader
void setLongClickDetectedRetriggerable(bool retriggerable);

// Must be called regularly (every 1–10ms)
void loop();
void reset();  // resets all state and clears all callbacks (except get_state_cb)

// State queries
bool isPressed() const;              // debounced
bool isPressedRaw() const;           // immediate pin state
bool wasPressed() const;             // true after any completed click
unsigned int wasPressedFor() const;  // duration of last press in ms
clickType getType() const;
uint8_t getNumberOfClicks() const;
uint16_t getLongClickCount() const;  // retriggerable long-click count
uint8_t getPin() const;
int getID() const;

// Configuration
void setDebounceTime(unsigned int ms);
void setDoubleClickTime(unsigned int ms);
void setLongClickTime(unsigned int ms);
void setID(int newID);
unsigned int getDebounceTime() const;
unsigned int getDoubleClickTime() const;
unsigned int getLongClickTime() const;

// State management
void resetPressedState();   // resets all state flags and timing
uint8_t resetClickCount();  // returns previous click count, resets to 0

// Callbacks — all receive Button2& reference
void setChangedHandler(CallbackFunction f);
void setPressedHandler(CallbackFunction f);
void setReleasedHandler(CallbackFunction f);
void setTapHandler(CallbackFunction f);               // fires on every release
void setClickHandler(CallbackFunction f);             // single click
void setDoubleClickHandler(CallbackFunction f);
void setTripleClickHandler(CallbackFunction f);
void setLongClickHandler(CallbackFunction f);         // fires after release
void setLongClickDetectedHandler(CallbackFunction f); // fires while still held

// Blocking helpers (use in simple sequential sketches only)
clickType read(bool keepState = false);
clickType wait(bool keepState = false);
void waitForClick(bool keepState = false);
void waitForDouble(bool keepState = false);
void waitForTriple(bool keepState = false);
void waitForLong(bool keepState = false);

// Utility
const char* clickToString(clickType type) const;  // returns string literal, no heap
bool operator==(const Button2 &rhs) const;        // identity comparison (same object)
```

## Key Patterns

### Virtual button (I2C expander, capacitive touch, etc.)

Use `BTN_VIRTUAL_PIN` and inject state via `setButtonStateFunction()`. For multiple buttons on one I2C expander, read the port **once per cycle** and cache it — do not read I2C inside each button's state function.

```cpp
uint8_t cachedPort = 0xFF;

void updateCache() {
    Wire.requestFrom(I2C_ADDR, 1);
    if (Wire.available()) cachedPort = Wire.read();
}

uint8_t btn0State() { return (cachedPort & 0x01) ? HIGH : LOW; }
uint8_t btn1State() { return (cachedPort & 0x02) ? HIGH : LOW; }

void loop() {
    updateCache();      // one I2C read per cycle
    buttons[0].loop();
    buttons[1].loop();
}
```

See `examples/I2CPortExpanderButtons/` for the complete pattern.

## Testing Framework

Tests use **AUnit** + **EpoxyDuino** (native, no hardware needed). Six suites in `test/`:

- `test_basics` — defaults and initialization
- `test_clicks` — single/double/triple/long click detection
- `test_callbacks` — all event handlers
- `test_states` — state queries, transitions, timing edges
- `test_configuration` — runtime settings
- `test_multiple` — multiple button interactions

Shared infrastructure lives in `test/shared/test_helpers.h` — `createTestButton()`, `click()`, `press()`, `release()`, `BUTTON_PIN`, `BUTTON_MODE`, `BUTTON_ACTIVE`, `DEBOUNCE_MS`.
