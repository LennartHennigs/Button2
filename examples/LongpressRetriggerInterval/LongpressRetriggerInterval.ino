/////////////////////////////////////////////////////////////////
/*
  LongpressRetriggerInterval.ino - Demo for decoupled retrigger interval.

  Shows how to set a long-press threshold and a separate (shorter)
  retrigger interval so the callback fires quickly after the initial
  threshold is met.

  - Hold button: first callback fires after 3000 ms
  - Every 500 ms after that: callback fires again (while still held)
  - On release: long click handler fires

  Works on ESP32, ESP8266, and AVR (with plain function pointers on AVR).
*/
/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN  2

/////////////////////////////////////////////////////////////////

Button2 button;

/////////////////////////////////////////////////////////////////

void longClick(Button2& btn) {
  Serial.print("long click released after ");
  Serial.print(btn.wasPressedFor());
  Serial.println(" ms");
}

/////////////////////////////////////////////////////////////////

void longClickDetected(Button2& btn) {
  Serial.print("long click detected #");
  Serial.print(btn.getLongClickCount());
  Serial.println();
}

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(20);
  }
  Serial.println("\n\nLongpress Retrigger Interval Demo");

  button.begin(BUTTON_PIN);
  button.setLongClickTime(3000);
  button.setLongClickDetectedRetriggerable(true, 500);

  button.setLongClickHandler(longClick);
  button.setLongClickDetectedHandler(longClickDetected);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
}

/////////////////////////////////////////////////////////////////
