/////////////////////////////////////////////////////////////////

#if !defined(ESP32)
  #error This sketch needs an ESP32 Classic
#else

/////////////////////////////////////////////////////////////////

#include "Button2.h"

#define TOUCH_PIN T0 // Must declare the touch assignment, not the pin. For example, T0 is GPIO4 and T3 is GPIO 15. 
// You can look up the touch assignment in the datasheet: https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf

int threshold = 40;   // ESP32
bool touchActive = false;
bool lastTouchActive = false;
bool testingLower = true; 
byte buttonState = HIGH;// HIGH is for unpressed, pressed = LOW
/////////////////////////////////////////////////////////////////

Button2 button;

/////////////////////////////////////////////////////////////////
void gotTouchEvent(){
  if (lastTouchActive != testingLower) {
    touchActive = !touchActive;
    testingLower = !testingLower;
    // Touch ISR will be inverted: Lower <--> Higher than the Threshold after ISR event is noticed
    touchInterruptSetThresholdDirection(testingLower);
  }
}


byte capStateHandler() {
    return buttonState;
}

/////////////////////////////////////////////////////////////////

void setup() {
    Serial.begin(115200);
    delay(50);
    Serial.println("\n\nCapacitive Touch Demo");
    touchAttachInterrupt(TOUCH_PIN, gotTouchEvent, threshold);

    // Touch ISR will be activated when touchRead is lower than the Threshold
    touchInterruptSetThresholdDirection(testingLower);
    button.setDebounceTime(35);
    button.setButtonStateFunction(capStateHandler);
    button.setClickHandler(click);
    button.begin(VIRTUAL_PIN);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
  if(lastTouchActive != touchActive){
    lastTouchActive = touchActive;
    if (touchActive) {
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
