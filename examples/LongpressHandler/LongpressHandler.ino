/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN  2

/////////////////////////////////////////////////////////////////

Button2 button;

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(20);
  }
  Serial.println("\n\nLongpress Handler Demo");
  button.begin(BUTTON_PIN);

  // button.setLongClickDetectedRetriggerable(true);

  button.setLongClickHandler(longClick);
  button.setLongClickDetectedHandler(longClickDetected);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
}

/////////////////////////////////////////////////////////////////

void longClick(Button2& btn) {
    unsigned int time = btn.wasPressedFor();
    Serial.print("You clicked ");
    if (time > 1500) {
        Serial.print("a really really long time.");
    } else if (time > 1000) {
        Serial.print("a really long time.");
    } else if (time > 500) {
        Serial.print("a long time.");        
    } else {
        Serial.print("long.");        
    }
    Serial.print(" (");        
    Serial.print(time);        
    Serial.println(" ms)");
    }

/////////////////////////////////////////////////////////////////

void longClickDetected(Button2& btn) {
    Serial.print("long click #");
    Serial.print(btn.getLongClickCount());
    Serial.println(" detected");
}

/////////////////////////////////////////////////////////////////