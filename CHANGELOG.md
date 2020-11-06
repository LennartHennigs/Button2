# Changelog

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.4.0] - 2020-11-06
- Updated LongpressHandler example - changed variable name to from ```buttonA``` to ```button```
- toggled ```pressed``` and ```released``` (as suggesed by [TommyC81] (https://github.com/TommyC81) in [#16] (https://github.com/LennartHennigs/Button2/issues/16))
- added debug function ```isPressedRaw()``` (as suggesed by [TommyC81] (https://github.com/TommyC81) in [#16] (https://github.com/LennartHennigs/Button2/issues/16))
- fixed bug with ```click_count``` (as suggesed by [TommyC81] (https://github.com/TommyC81) in [#16] (https://github.com/LennartHennigs/Button2/issues/16))

## [1.3.0] - 2020-11-06
- Added capacitive touch sensor capabilties (for ESP32) (as suggested by [qubolino](https://github.com/qubolino) in [#11] (https://github.com/LennartHennigs/Button2/issues/11)) 
- Removed deprecated entry in the library.properties file (as suggested by [https://github.com/SNL5943]) in [#15](https://github.com/LennartHennigs/Button2/issues/15)
- Added ```const```  modifier to functions (as suggested by [Anton-V-K](https://github.com/Anton-V-K) in [#13] (https://github.com/LennartHennigs/Button2/issues/13)) 


## [1.2.0] - 2020-04-16
- Added possibility to define your own timeouts for clicks (as suggested by [cmeldas](https://github.com/cmeldas) in [#10](https://github.com/LennartHennigs/Button2/issues/10))
- Removed ```yield()``` in main ```loop()``` since it caused some problems
- Created and added CHANGELOG.md 


## [1.1.0] - 2020-03-27
- Changed the private functions to protected (as suggested by [Nagymadar](https://github.com/Nagymadar) in [#9](https://github.com/LennartHennigs/Button2/issues/9))
- Added support for active high buttons (as suggested by [Nagymadar](https://github.com/Nagymadar) in [#8](https://github.com/LennartHennigs/Button2/issues/8))
- Added ```reset()``` function to unset all functions (as suggested by [Nagymadar](https://github.com/Nagymadar) in [#7](https://github.com/LennartHennigs/Button2/issues/7))
- Added a ```yield()``` command to the main ```loop()```
- Changed the times for double and triple click
- Fixed error in ```SingleButton.ino``` (as suggested by [alexthe-red](https://github.com/alexthe-red) in [#3](https://github.com/LennartHennigs/Button2/issues/3))
- Added the library to the Arduino IDE


## [1.0.0] - 2017-11-09
- initial release