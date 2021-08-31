/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN  2

/////////////////////////////////////////////////////////////////

Button2 button;

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  delay(50);
  Serial.println("\n\nButton Demo");

  button.begin(BUTTON_PIN);
  // button.setLongClickTime(1000);
  // button.setDoubleClickTime(400);

  Serial.println(" Longpress Time: " + String(button.getLongClickTime()) + "ms");
  Serial.println(" DoubleClick Time: " + String(button.getDoubleClickTime()) + "ms");

  // button.setChangedHandler(changed);
  // button.setPressedHandler(pressed);
  button.setReleasedHandler(released);

  // button.setTapHandler(tap);
  button.setClickHandler(click);
  button.setLongClickDetectedHandler(longClickDetected);
  button.setLongClickHandler(longClick);
  
  button.setDoubleClickHandler(doubleClick);
  button.setTripleClickHandler(tripleClick);
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
void click(Button2& btn) {
    Serial.println("click\n");
}
void longClickDetected(Button2& btn) {
    Serial.println("long click detected\n");
}
void longClick(Button2& btn) {
    Serial.println("long click\n");
}
void doubleClick(Button2& btn) {
    Serial.println("double click\n");
}
void tripleClick(Button2& btn) {
    Serial.println("triple click\n");
}
void tap(Button2& btn) {
    Serial.println("tap");
}

/////////////////////////////////////////////////////////////////