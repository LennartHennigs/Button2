/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN  39

/////////////////////////////////////////////////////////////////

Button2 button;

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n\nMulti Handler Demo");
  
  button.begin(BUTTON_PIN);
  button.setClickHandler(handler);
  //button.setLongClickHandler(handler);       // this will only be called upon release
  button.setLongClickDetectedHandler(handler);  // this will only be called upon detection
  button.setDoubleClickHandler(handler);
  button.setTripleClickHandler(handler);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
}

/////////////////////////////////////////////////////////////////

void handler(Button2& btn) {
    switch (btn.getType()) {
        case single_click:
            break;
        case double_click:
            Serial.print("double ");
            break;
        case triple_click:
            Serial.print("triple ");
            break;
        case long_click:
            Serial.print("long");
            break;
          case empty:
            return;
    }
    Serial.print("click");
    Serial.print(" (");
    Serial.print(btn.getNumberOfClicks());    
    Serial.println(")");
}
/////////////////////////////////////////////////////////////////