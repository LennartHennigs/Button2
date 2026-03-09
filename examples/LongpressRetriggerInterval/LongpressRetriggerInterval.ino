/////////////////////////////////////////////////////////////////
//
// LongpressRetriggerInterval.ino
//
// Example showing how to use a custom interval for continuous
// long press events after the initial detection.
//
/////////////////////////////////////////////////////////////////

#include "Button2.h"

#define BUTTON_PIN  2 // Modify this to match your hardware

Button2 button;

void onLongClickDetected(Button2& btn) {
  Serial.print("Long click detected! Consecutive count: ");
  Serial.println(btn.getLongClickCount());
}

void onReleased(Button2& btn) {
  Serial.print("Button released. Total time held: ");
  Serial.print(btn.wasPressedFor());
  Serial.println(" ms");
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(20);
  }
  Serial.println("\n\nLongpress Retrigger Interval Example");
  Serial.println("Press and hold the button for more than 3 seconds.");

  button.begin(BUTTON_PIN);

  // Set the initial time required to detect a long press (e.g. 3000ms / 3s)
  button.setLongClickTime(3000);

  // Enable continuous trigger while holding
  button.setLongClickDetectedRetriggerable(true);

  // Set a faster interval for subsequent triggers (e.g. 500ms / 0.5s)
  // If not set, it defaults to the value of getLongClickTime().
  button.setLongClickInterval(500);

  // Assign standard callbacks instead of lambdas for better readability
  button.setLongClickDetectedHandler(onLongClickDetected);
  button.setReleasedHandler(onReleased);
}

void loop() {
  button.loop();
}
