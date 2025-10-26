# Changelog

**Note:** Unreleased changes are checked in but not part of an official release (available through the Arduino IDE or PlatfomIO) yet. This allows you to test WiP features and give feedback to them.

## Unreleased

## [2.5.0] - 2025-10-25

This release focuses on code quality improvements, bug fixes, comprehensive testing infrastructure, and enhanced support for I2C port expanders and virtual buttons.

### Fixed

- **CRITICAL**: Fixed integer overflow in long click counter calculation on AVR platforms (Arduino Nano/Uno). The multiplication `longclick_time_ms * (longclick_counter + 1)` now uses proper casting to prevent 16-bit overflow that could cause incorrect long click timing [Button2.cpp:415]
- **Issue #82**: Fixed ambiguous reference to `empty` enum value when compiling with libraries that use `using namespace std;`. The library now uses qualified references (`clickType::empty`) internally to avoid conflicts with `std::empty()` from C++17 standard library. This fix maintains 100% backward compatibility - user code does not need to change [Button2.h:117, Button2.cpp:225]
- Fixed uninitialized member variables (`click_ms`, `down_ms`, `state`, `prev_state`) in constructor that could cause phantom button presses and incorrect click detection on startup [Button2.h:69-76]
- Fixed `resetPressedState()` to properly reset all timing variables (`click_ms`, `down_ms`, `last_click_count`) for complete state reset [Button2.cpp:223-235]
- Fixed `operator==` const correctness - now properly uses `const Button2&` parameter and `const` method qualifier [Button2.h:177, Button2.cpp:98]
- Fixed test initialization order bug where `setButtonStateFunction()` was called after `begin()`, causing phantom press/release transitions
- Fixed compile_examples.sh: Removed unnecessary AUnit dependency from example compilation
- Fixed compile_examples.sh: Corrected M5StackCore2CustomHandler exclusion logic - now properly runs on M5Stack platform
- Fixed compile_examples.sh: Improved platformio.ini generation for M5Stack dependencies

### Added

- **Issue #69**: Added optional initialization callback parameter to `begin()` method for virtual buttons. This allows hardware initialization (I2C/SPI expanders, touch sensors, etc.) to be encapsulated within the button setup. The callback is invoked immediately when `begin()` is called, ensuring hardware is ready before button polling starts. Example: `button.begin(BTN_VIRTUAL_PIN, INPUT, true, initCallback);` [Button2.h:141, Button2.cpp:34-43]
- **Issue #70**: Added `I2CPortExpanderButtons.ino` example demonstrating the efficient caching pattern for multiple buttons on I2C port expanders (PCF8574, MCP23017). The example shows how to read the entire port once per loop cycle and use bit masking in state handlers, reducing I2C bus traffic from N transactions to just 1 per cycle. This pattern scales efficiently to 8+ buttons while minimizing I2C overhead
- **Comprehensive Test Suite**: Added 70 tests across 6 test suites using AUnit framework:
  - **test_basics** (10 tests): Initialization, configuration, default values, init callback
  - **test_clicks** (11 tests): Click detection - single, double, triple, long
  - **test_callbacks** (12 tests): All event handler callbacks
  - **test_states** (19 tests): State management, queries, timing edge cases
  - **test_configuration** (7 tests): Runtime settings and configuration
  - **test_multiple** (11 tests): Multiple button interactions
- Added EpoxyDuino-based native testing (no hardware required) - tests run on host machine emulating ESP8266/ESP32
- Added automated compilation testing script (`test/compile_examples.sh`) that tests all examples across multiple platforms (ESP8266, ESP32, AVR)
- Added comprehensive test documentation in `test/README.md` and `test/CLAUDE.md`
- Added extensive inline code documentation explaining:
  - Asymmetric debouncing strategy (press vs release edge handling)
  - Long click detection design (only on first click to avoid multi-click ambiguity)
  - Critical importance of calling `loop()` regularly (recommended 1-10ms frequency)
  - Virtual button usage and I2C port expander patterns
- Added documentation for `wasPressedFor()` behavior in multi-click scenarios (returns duration of most recent click only) [Issue #35]

### Changed

- **OPTIMIZATION**: Reordered struct members for better memory packing, reducing padding overhead on 32-bit platforms (ESP32/ESP8266) [Button2.h:66-122]
- **PERFORMANCE**: Changed `clickToString()` to return `const char*` instead of `String` to avoid heap allocation and memory fragmentation on low-memory devices (AVR) [Button2.h:172, Button2.cpp:206]
- **CODE QUALITY**: Replaced NULL with BUTTON2_NULL macro (nullptr on C++11+, NULL on AVR) for consistent modern C++ practices while maintaining Arduino compatibility
- **Breaking Change (Minor)**: Return type of `clickToString()` changed from `String` to `const char*` - most code will work unchanged, but assignments to `String` variables may need explicit casting
- Improved test reliability through proper initialization order and state management
- **TEST PERFORMANCE**: Reduced setup delays in all test suites from 1000ms to 100ms, improving native test execution time by ~5 seconds and reducing intermittent timeouts
- Enhanced README.md with:
  - Virtual buttons and custom state handlers section
  - Efficient pattern for multiple I2C buttons with caching example
  - Updated std::function platform support documentation [Issue #58]
  - Added scoped enum usage recommendation for `clickType::empty` [Issue #82]
- Updated CustomButtonStateHandler.ino example to demonstrate initialization callback usage
- Updated documentation (CLAUDE.md) to reflect current testing approach using `setButtonStateFunction()`

### Removed

- Removed Hardware.h abstraction layer (replaced with simpler `setButtonStateFunction()` approach)
- Removed unused Button2TestHelper utility class

### Documentation

- Documented current behavior of `wasPressedFor()` for multi-click scenarios [Issue #35]
- Documented std::function support now works on all C++11+ platforms except AVR [Issue #58]
- Added comprehensive virtual button documentation with I2C port expander examples
- Added notes about `clickType::empty` scoped syntax to avoid naming conflicts [Issue #82]

## [2.4.1] - 2025-07-19

- **Issue #58**: Improved detection and enabling of `std::function` support for callback handlers. Switched from platform-specific checks to language-version-based detection (`__cplusplus >= 201103L`), excluding AVR platforms. This provides broader compatibility for modern boards including ESP32, ESP8266, Teensy, ARM, RP2040, SAMD, STM32, and other C++11+ platforms.

## [2.4.0] - 2025-07-19

- Removed workaround file `main.cpp` to prevent compile warnings and undefined behavior in PlatformIO. The library now compiles correctly as a dependency in PlatformIO projects. [PR #83](https://github.com/LennartHennigs/Button2/issues/83)
- Improved callback handler performance and compatibility by using `std::move` for `std::function` assignments on supported platforms (ESP32/ESP8266). Users can now force-enable or disable `std::function` support via macros. [PR #84](https://github.com/LennartHennigs/Button2/pull/84)

## [2.3.5] - 2025-05-01

- fixed contant name in example `M5StackCore2CustomHandler.ino`
- replaced `boolean` with `bool` as requested in [#81](https://github.com/LennartHennigs/Button2/issues/81)
- replaced `byte` with `uint_8` as requested in [#79](https://github.com/LennartHennigs/Button2/issues/79)

## [2.3.4] - 2025-01-26

- added dummy file `main.cpp` to be able to compile lib in VSCode/PIO

## [2.3.3] - 2024-05-30

- fixed bug, that first long press was not properly detected, see issue [#72](https://github.com/LennartHennigs/Button2/issues/72)
- added `byte resetClickCount()` function
- click count is no longer resetted in `resetPressedState()`
- updated examples

## [2.3.2] - 2024-02-17

- expanded condition to check for API version 2.0 (for UNO R4, RP2040, ...)

## [2.3.1] - 2024-01-02

- bugfix for RP2040, as pointed out by [kilrah](https://github.com/kilrah) in issue [#60](https://github.com/LennartHennigs/Button2/issues/60)

## [2.3.0] - 2024-01-02

- renamed Button2 constants, they now start with `BTN_`  (BREAKING CHANGE)
- added `Hardware.h` – it implements hardware pin abstraction. Needed for unit testing
- added Unit Tests
- added `resetPressedState()` function
- added `ESP32S2S3CapacitiveTouch.ino` suggested by [ryancasler](https://github.com/ryancasler) in PR #57

## [2.2.4] - 2023-06-22

- `getNumberOfClicks()` now works inside a callback and after the `wait()`statement(s).
- Refactored code in `Button2.cpp`

## [2.2.3] - 2023-06-21

- Included PR for issue [#54](https://github.com/LennartHennigs/Button2/issues/54)

## [2.2.2] - 2022-12-16

- Another stab at the bug [#46](https://github.com/LennartHennigs/Button2/issues/46)

## [2.2.1] - 2022-12-16

- Fixed bug [#46](https://github.com/LennartHennigs/Button2/issues/46) that in some instances long clicks are wrongly triggered

## [2.2.0] - 2022-12-13

- Refactored the main `loop()`
- Rewrote click detection
- Cleaned up the long press handling
- Removed compiler switches – they made the code unreadable and they only saved a few bytes (could be a BREAKING CHANGE)
- Added `byte getLongClickCount()` function
- Updated the [LongpressHandler](https://github.com/LennartHennigs/Button2/blob/master/examples/LongpressHandler/LongpressHandler.ino) example
- Defaults (x>3)-clicks to triple
- Fixed bug with button ID

## [2.1.0] - 2022-11-03

- Removed the capacitive touch functionality out of main library. (BREAKING CHANGE). The constructor and `begin()` lost a parameter. Instead I provide a custom handler example for cap. touch [ESP32CapacitiveTouch.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/ESP32CapacitiveTouch/ESP32CapacitiveTouch.ino). For reasons, see [#45](https://github.com/LennartHennigs/Button2/issues/45).
- Added an ESP32 timer interrupt example [ESP32TimerInterrupt.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/ESP32TimerInterrupt/ESP32TimerInterrupt.ino) based on [#43](https://github.com/LennartHennigs/Button2/issues/43).
- Added compiler switches in `Button.h` to remove click detection code, as mentioned in [#44](https://github.com/LennartHennigs/Button2/issues/44).
- Clarified the difference between the `setLongClickHandler` and the `setLongClickDetectedHandler` in the README and the [MultiHandler](https://github.com/LennartHennigs/Button2/blob/master/examples/MultiHandler/MultiHandler.ino) example as mentioned in [#41](https://github.com/LennartHennigs/Button2/issues/41). (The handler set via `setLongClickHandler` waits until you release the button, the second one is called as soon as the defined long-click time has passed.)
- Made `byte _getState()` into a `const` function.

## [2.0.3] - 2022-05-26

- Fixed bug with the button ID as pointed out by [Jon](https://github.com/mscreations) in [#39](https://github.com/LennartHennigs/Button2/pull/39).

## [2.0.2] - 2022-05-21

- Added example for the [M5Stack Core2](https://github.com/LennartHennigs/Button2/blob/master/examples/M5StackCore2CustomHandler/M5StackCore2CustomHandler.ino) - showing how to add a custom handler for the touch buttons.

## [2.0.1] - 2022-04-22

- Fixed bug – `longclick_detected_counter` is not properly initalized as mentioned in [#37](https://github.com/LennartHennigs/Button2/pull/37).

## [2.0.0] - 2022-04-04

- House keeping
  - Refactored `loop()` - cleaned up conditions, should be easier to understand now.
  - Renamed `getAttachedPin()`to `getPin()` (BREAKING CHANGE).
  - Fixed a bug that the first click type was falsly returned by `getClickType()`.
- Possibility define your own "_getState" function for non standard buttons as suggested in [#32](https://github.com/LennartHennigs/Button2/issues/32).
  - Refactored `isPressedRaw()` to also use `_getState()`.
  - Introduced a `VIRTUAL_PIN` constant – using it in the constructor or `begin()` will skip pin initalization.
  - Added `setButtonStateFunction(StateCallbackFunction f)` to assign your own "_getState" function.
  - Added [CustomButtonStateHandler.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/CustomButtonStateHandler/CustomButtonStateHandler.ino) example.
- Improved click type handling.
  - Added `clickType`      and removed constants for determining the click type (BREAKING CHANGE)
  - Renamed `getClickType()` to `getType()` (BREAKING CHANGE)
  - Added `clickToString` function to print the `clickType` enum value
- Added IDs button instances
  - Added `getID()`, it returns an auto incremented `int` ID for the button, as suggest in [#34](https://github.com/LennartHennigs/Button2/pull/34)
  - Added `setID()`, it allows you to set your own IDs – but then you need to ensure its uniqeness yourself
- Added possibility to use the button class inside your main `loop()` function (instead of using callback handlers)
  - Added `bool wasPressed()` function
  - Added `read(bool keepState = false)`, it returns the button press type (as a `clickType` enum)
  - Added `wait(bool keepState = false)`, it combines `wasPressed()` and `read()` methods and halts execution until a button press took place
  - Added `waitForClick()`, `waitForDouble()`, `waitForTriple()` and `waitForLong()` as well
  - Added [ButtonLoop.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/ButtonLoop/ButtonLoop.ino) example to showcase the "loop" functions

## [1.6.5] - 2021-09-12

- Fixed problem with `std::function` as found by [ItsFlo](https://github.com/ItsFlo) in pull request [#29](https://github.com/LennartHennigs/Button2/pull/29)

## [1.6.4] - 2021-09-12

- Use `std::function` to allow C++ 11 lambda functions as suggested by [jacobdekeizer](https://github.com/jacobdekeizer) in pull request [#29](https://github.com/LennartHennigs/Button2/pull/29)

## [1.6.3] - 2021-09-12

- added two new examples: [MultiHandlerTwoButtons.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/MultiHandlerTwoButtons/MultiHandlerTwoButtons.ino) and [TrackDualButtonClick.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/TrackDualButtonClick/TrackDualButtonClick.ino)
- added examples to the [README.md](https://github.com/LennartHennigs/Button2/blob/master/README.md)
- initialized `pin` in `_getState()`
- a bit of variable clean up
- updated setting of inital state of `state` and `prev_state`

## [1.6.2] - 2021-06-22

- initialized `pin` property to 255 instead of -1, as pointed out by [rin67630](https://github.com/rin67630) in issue [#26](https://github.com/LennartHennigs/Button2/issues/26)

## [1.6.1] - 2021-03-22

- updated [README.md](https://github.com/LennartHennigs/Button2/blob/master/README.md)
- added `const` to getter functions

## [1.6.0] - 2021-02-10

- added getter/setter functions for debounce, longclick and doubleclick timeouts
- removed debounce timeout parameter from `contructor` and `begin()`

## [1.5.4] - 2021-02-08

- Added `getAttachPin()` function, as suggested by [madivad](https://github.com/madivad) in issue [#23](https://github.com/LennartHennigs/Button2/issues/23)

## [1.5.3] - 2021-01-26

- Fixed a bug in the constructor, as suggested by [alex-s-v](https://github.com/alex-s-v) in pull request [#22](https://github.com/LennartHennigs/Button2/pull/22)

## [1.5.2] - 2021-01-26

- Fixed a bug in the `isPressed()` function, as suggested by [zenturacp](https://github.com/zenturacp) in [#21](https://github.com/LennartHennigs/Button2/issues/21)

## [1.5.1] - 2021-01-04

- Fixed a bug in the `loop()` function

## [1.5.0] - 2021-01-03

- Added default constructor and `begin()` function
- Added pull request by [skelstar](https://github.com/skelstar) to add the `setLongClickDetectedHandler()` function which is triggered as soon as the longclick timeout has passed

## [1.4.1] - 2020-12-19

- Moved activeLow outside of isCapacitive condition (as suggested by [Wai Lin](https://github.com/w4ilun) in [#18](https://github.com/LennartHennigs/Button2/pull/18)

## [1.4.0] - 2020-11-06

- Updated LongpressHandler example - changed variable name to from `button` to `button`
- toggled `pressed` and `released` (as suggesed by [TommyC81](https://github.com/TommyC81) in [#16](https://github.com/LennartHennigs/Button2/issues/16))
- added debug function `isPressedRaw()` (as suggesed by [TommyC81](https://github.com/TommyC81) in [#16](https://github.com/LennartHennigs/Button2/issues/16))
- fixed bug with `click_count` (as suggesed by [TommyC81](https://github.com/TommyC81) in [#16](https://github.com/LennartHennigs/Button2/issues/16))
- changed return types of `getNumberOfClicks()` and  `getClickType()` to `byte`

## [1.3.0] - 2020-11-06

- Added capacitive touch sensor capabilties (for ESP32) (as suggested by [qubolino](https://github.com/qubolino) in [#11](https://github.com/LennartHennigs/Button2/issues/11))
- Removed deprecated entry in the library.properties file (as suggested by [SangLe](https://github.com/SNL5943)) in [#15](https://github.com/LennartHennigs/Button2/issues/15)
- Added `const`  modifier to functions (as suggested by [Anton-V-K](https://github.com/Anton-V-K) in [#13](https://github.com/LennartHennigs/Button2/issues/13))

## [1.2.0] - 2020-04-16

- Added possibility to define your own timeouts for clicks (as suggested by [cmeldas](https://github.com/cmeldas) in [#10](https://github.com/LennartHennigs/Button2/issues/10))
- Removed `yield()` in main `loop()` since it caused some problems
- Created and added CHANGELOG.md

## [1.1.0] - 2020-03-27

- Changed the private functions to protected (as suggested by [Nagymadar](https://github.com/Nagymadar) in [#9](https://github.com/LennartHennigs/Button2/issues/9))
- Added support for active high buttons (as suggested by [Nagymadar](https://github.com/Nagymadar) in [#8](https://github.com/LennartHennigs/Button2/issues/8))
- Added `reset()` function to unset all functions (as suggested by [Nagymadar](https://github.com/Nagymadar) in [#7](https://github.com/LennartHennigs/Button2/issues/7))
- Added a `yield()` command to the main `loop()`
- Changed the times for double and triple click
- Fixed error in `SingleButton.ino` (as suggested by [alexthe-red](https://github.com/alexthe-red) in [#3](https://github.com/LennartHennigs/Button2/issues/3))
- Added the library to the Arduino IDE

## [1.0.0] - 2017-11-09

- initial release

## Note

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
