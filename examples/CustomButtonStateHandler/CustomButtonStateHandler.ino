/////////////////////////////////////////////////////////////////
#include "Button2.h"
/////////////////////////////////////////////////////////////////

Button2 button;

/////////////////////////////////////////////////////////////////

// Custom state handler - reads button state from any source
uint8_t myButtonStateHandler() {
  return digitalRead(39);
}

/////////////////////////////////////////////////////////////////

// Optional initialization callback for hardware setup
// This is called when begin() is invoked
void myInitCallback() {
  // Initialize your hardware here (I2C, SPI, touch sensors, etc.)
  pinMode(39, INPUT_PULLUP);
  Serial.println("Hardware initialized via callback");
}

/////////////////////////////////////////////////////////////////

void myTapHandler(Button2 &b) {
    Serial.println("tap");
}

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\nCustom Button State Handler Test");

  // Option 1: With initialization callback (recommended for virtual buttons)
  button.begin(BTN_VIRTUAL_PIN, INPUT, true, myInitCallback);

  // Option 2: Manual initialization (old approach, still works)
  // pinMode(39, INPUT_PULLUP);
  // button.begin(BTN_VIRTUAL_PIN);

  button.setButtonStateFunction(myButtonStateHandler);
  button.setTapHandler(myTapHandler);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
}

/////////////////////////////////////////////////////////////////