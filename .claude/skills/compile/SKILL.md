---
name: compile
description: Compile Button2 examples across Arduino platforms using arduino-cli and PlatformIO. Pass a platform name (esp8266, esp32, nano, all) or leave blank to compile all.
---

Compile Button2 examples to verify they build cleanly across target platforms.

Parse $ARGUMENTS:
- If a specific platform is named (esp8266, esp32, nano), run `pio run -e <matching env>` for that platform
- If "examples" or nothing is given, run `./test/compile_examples.sh` to compile all examples across all platforms
- If a specific example name is given, find the matching `.ino` file under `examples/` and compile it with `arduino-cli compile` for all three cores (esp8266:esp8266:d1_mini, esp32:esp32:m5stack_core2, arduino:avr:nano)

Report any compiler errors clearly, quoting the relevant error lines. If compilation is clean, confirm which platforms and examples passed.
