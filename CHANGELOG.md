# Changelog

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased
- none, yet


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