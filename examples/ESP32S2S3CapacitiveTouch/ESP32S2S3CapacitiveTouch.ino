/////////////////////////////////////////////////////////////////
// see https://github.com/LennartHennigs/Button2/pull/57
/////////////////////////////////////////////////////////////////
#if !defined(ESP32)
  #error This sketch needs an ESP32 S2 or S3
#else

/////////////////////////////////////////////////////////////////

#include "Button2.h"

#define TOUCH_PIN T5 // Must declare the touch assignment, not the pin.

int threshold = 1500;   // ESP32S2 
bool touchdetected = false; 
byte buttonState = HIGH;// HIGH is for unpressed, pressed = LOW
/////////////////////////////////////////////////////////////////

Button2 button;

/////////////////////////////////////////////////////////////////
void gotTouch() {
  touchdetected = true;
}


byte capStateHandler() {
    return buttonState;
}

/////////////////////////////////////////////////////////////////

void setup() {
    Serial.begin(115200);
    delay(50);
    Serial.println("\n\nCapacitive Touch Demo");
    touchAttachInterrupt(TOUCH_PIN, gotTouch, threshold); 
    button.setDebounceTime(35);
    button.setButtonStateFunction(capStateHandler);
    button.setClickHandler(click);
    button.begin(BTN_VIRTUAL_PIN);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
  if (touchdetected) {
    touchdetected = false;
    if (touchInterruptGetLastStatus(TOUCH_PIN)) {
      buttonState = LOW;
    } else {
      buttonState = HIGH;
    }
  }
}

/////////////////////////////////////////////////////////////////

void click(Button2& btn) {
    Serial.println("click\n");
}

/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
