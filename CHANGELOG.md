# Changelog

**Note:** Unreleased changes are checked in but not part of an official release (available through the Arduino IDE or PlatfomIO) yet. This allows you to test WiP features and give feedback to them.

## Unreleased

- nothing, so far

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
