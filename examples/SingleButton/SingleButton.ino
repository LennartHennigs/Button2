/////////////////////////////////////////////////////////////////

#include "Button2.h";

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN  2

/////////////////////////////////////////////////////////////////

Button2 button = Button2(BUTTON_PIN);

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  delay(50);
  Serial.println("\n\nButton Demo");
  
  // button.setChangedHandler(changed);
  // button.setPressedHandler(pressed);
  button.setReleasedHandler(released);

  // button.setTapHandler(tap);
  button.setClickHandler(click);
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
