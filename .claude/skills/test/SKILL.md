---
name: test
description: Run Button2 test suites via PlatformIO. Optionally pass a suite name (basics, clicks, callbacks, states, configuration, multiple) or platform (esp8266, esp32, nano). Defaults to all suites on ESP8266.
---

Run the Button2 test suite using PlatformIO + EpoxyDuino (no hardware required).

Parse $ARGUMENTS:
- If a specific suite is named (basics, clicks, callbacks, states, configuration, multiple), run `pio test -e test_<suite>`
- If a platform is named (esp8266, esp32, nano), run `pio test -e epoxy-<platform>`
- If "all" or nothing is given, run `pio test -e epoxy-esp8266 -e epoxy-esp32 -e epoxy-nano`

Show the filtered output (`grep -E '(PASSED|FAILED|ERRORED|summary)'`) and report how many suites passed vs failed.

If any suite fails, show the full verbose output for the failing suite so the failure is easy to diagnose.
