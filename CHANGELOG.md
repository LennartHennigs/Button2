# Changelog
-

## Unreleased

## [1.6.5] - 2021-09-12
- Fixed problem with `std::function` as found by [ItsFlo](https://github.com/ItsFlo) in pull request [#29](https://github.com/LennartHennigs/Button2/pull/29)

## [1.6.4] - 2021-09-12
- Use `std::function` to allow C++ 11 lambda functions as suggested by [jacobdekeizer](https://github.com/jacobdekeizer) in pull request [#29](https://github.com/LennartHennigs/Button2/pull/29)

## [1.6.3] - 2021-09-12
- added two new examples: `[MultiHandlerTwoButtons.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/MultiHandlerTwoButtons/MultiHandlerTwoButtons.ino)` and `[TrackDualButtonClick.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/TrackDualButtonClick/TrackDualButtonClick.ino.ino)`
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
