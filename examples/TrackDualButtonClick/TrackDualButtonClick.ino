/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN_1  37
#define BUTTON_PIN_2  38

/////////////////////////////////////////////////////////////////

Button2 button_1, button_2;

unsigned long now = 0;
byte counter = 0;

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n\nTrack dual button press & release");

  button_1.begin(BUTTON_PIN_1);
  button_1.setPressedHandler(pressed);
  button_1.setReleasedHandler(released);

  button_2.begin(BUTTON_PIN_2);
  button_2.setPressedHandler(pressed);
  button_2.setReleasedHandler(released);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button_1.loop();
  button_2.loop();
}

/////////////////////////////////////////////////////////////////

void pressed(Button2& btn) {
  counter++;
  if (counter == 2) {
    now = millis();
    Serial.println("now!");
  }
}

/////////////////////////////////////////////////////////////////

void released(Button2& btn) {
  counter--;
  if (counter == 0) {
    if (now != 0) {
      Serial.print("Pressed for: ");
      Serial.print(millis() - now);
      Serial.println("ms");
    }
    now = 0;
  }
}

/////////////////////////////////////////////////////////////////