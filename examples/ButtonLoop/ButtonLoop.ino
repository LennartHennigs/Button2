/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN  2

/////////////////////////////////////////////////////////////////

Button2 button;

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n\nButton Loop Demo");

  button.begin(BUTTON_PIN);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();

  
  Serial.println(button.clickToString(button.wait()));
  Serial.println(button.getNumberOfClicks());

/* 
 *  or replace the above line with (keep the loop):
  
  if (button.wasPressed()) {
      switch(button.read()) {
          case single_click: 
            Serial.println("single");
            break;
          case double_click: 
            Serial.println("double");
            break;
          case triple_click: 
            Serial.println("triple");
            break;
          case long_click: 
            Serial.println("looong");
            break;
      }
  }
*/
  
}
/////////////////////////////////////////////////////////////////
