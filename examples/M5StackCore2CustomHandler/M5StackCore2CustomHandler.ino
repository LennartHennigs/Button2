#include "M5Core2.h"
#include "Button2.h"

/////////////////////////////////////////////////////////////////

Button2 button;
bool cleared = false;

/////////////////////////////////////////////////////////////////

byte myButtonStateHandler() {
  return !(M5.BtnA.isPressed());
}

/////////////////////////////////////////////////////////////////

void setup() {
  M5.begin(); 

  button.setDoubleClickTime(300);

//  button.setChangedHandler(changed);
//  button.setPressedHandler(pressed);
  button.setReleasedHandler(released);

//  button.setTapHandler(tap);
  button.setClickHandler(click);
  button.setLongClickDetectedHandler(longClickDetected);
//  button.setLongClickHandler(longClick);
  
  button.setDoubleClickHandler(doubleClick);
  button.setTripleClickHandler(tripleClick);

  button.setButtonStateFunction(myButtonStateHandler);
  button.begin(VIRTUAL_PIN);

  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextWrap(true, true);
  M5.Lcd.print("Button A Test");
  M5.Lcd.print("\n");
  M5.Lcd.print("\n");
}

void loop() {
  button.loop();
  // clear screen if wrap happened
  if (M5.Lcd.getCursorY() <= 16) {
    if (!cleared) {
      M5.Lcd.clear();
      Serial.println("now");
      cleared = true;
    }
  } else {
    cleared = false;
  } 
  M5.update(); 
}

/////////////////////////////////////////////////////////////////

void pressed(Button2& btn) {
    M5.Lcd.print("pressed");
    M5.Lcd.print("\n");
}
void released(Button2& btn) {
    M5.Lcd.print("released: ");
    M5.Lcd.print(btn.wasPressedFor());
    M5.Lcd.print("\n");
}
void changed(Button2& btn) {
    M5.Lcd.print("changed");
    M5.Lcd.print("\n");
}
void click(Button2& btn) {
    M5.Lcd.print("click");
    M5.Lcd.print("\n");
}
void longClickDetected(Button2& btn) {
    M5.Lcd.print("long click detected");
    M5.Lcd.print("\n");
}
void longClick(Button2& btn) {
    M5.Lcd.print("long click");
    M5.Lcd.print("\n");
}
void doubleClick(Button2& btn) {
    M5.Lcd.print("double click");
    M5.Lcd.print("\n");
}
void tripleClick(Button2& btn) {
    M5.Lcd.print("triple click\n");
    M5.Lcd.print("\n");
}
void tap(Button2& btn) {
    M5.Lcd.print("tap");
    M5.Lcd.print("\n");
}