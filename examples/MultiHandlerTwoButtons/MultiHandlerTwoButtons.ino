/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN_1  3
#define BUTTON_PIN_2  4

/////////////////////////////////////////////////////////////////

Button2 button_1, button_2;

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  delay(50);
  Serial.println("\n\nMulti Handler Demo w/ 2 buttons");

  button_1.begin(BUTTON_PIN_1);
  button_1.setClickHandler(handler);
  button_1.setDoubleClickHandler(handler);

  button_2.begin(BUTTON_PIN_2);
  button_2.setClickHandler(handler);
  button_2.setDoubleClickHandler(handler);

}

/////////////////////////////////////////////////////////////////

void loop() {
  button_1.loop();
  button_2.loop();
}

/////////////////////////////////////////////////////////////////

void handler(Button2& btn) {
    switch (btn.getType()) {
        case single_click:
            break;
        case double_click:
            Serial.print("double ");
            break;
    }
    Serial.print("click ");
    Serial.print("on button #");
    Serial.print((btn == button_1) ? "1" : "2");
    Serial.println();
}
/////////////////////////////////////////////////////////////////