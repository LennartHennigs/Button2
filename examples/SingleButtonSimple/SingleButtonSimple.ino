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
  Serial.println("\n\nButton Demo");
  
  button.begin(BUTTON_PIN);
  button.setChangedHandler(changed);
  //button.setPressedHandler(pressed);
  //button.setReleasedHandler(released);

  // setTapHandler() is called by any type of click, longpress or shortpress
  button.setTapHandler(tap);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
}

/////////////////////////////////////////////////////////////////

void pressed(Button2& btn) {
    Serial.println("pressed");
}
void released(Button2& btn) {
    Serial.print("released: ");
    Serial.println(btn.wasPressedFor());
}
void changed(Button2& btn) {
    Serial.println("changed");
}
void tap(Button2& btn) {
    Serial.println("tap");
}
/////////////////////////////////////////////////////////////////

