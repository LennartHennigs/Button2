/////////////////////////////////////////////////////////////////
/*
  CallbackContext.ino - Button2 Library Example

  Demonstrates setContext() / getContext() to pass caller-owned data
  into button handlers without global variables.

  Two buttons share the same handler functions. Each button has a
  ButtonCtx struct attached via setContext(), so the handler knows
  which LED to control and what label to print.

  Works on all platforms including AVR (no lambda captures required).

  Created by Lennart Hennigs
*/
/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////

#define BUTTON_A_PIN  2
#define BUTTON_B_PIN  3
#define LED_A_PIN    12
#define LED_B_PIN    13

/////////////////////////////////////////////////////////////////

struct ButtonCtx {
  uint8_t     ledPin;
  const char* label;
};

ButtonCtx ctxA = { LED_A_PIN, "Button A" };
ButtonCtx ctxB = { LED_B_PIN, "Button B" };

Button2 buttonA, buttonB;

/////////////////////////////////////////////////////////////////

void onClick(Button2& btn) {
  ButtonCtx* ctx = (ButtonCtx*)btn.getContext();
  Serial.print(ctx->label);
  Serial.println(": click — toggling LED");
  digitalWrite(ctx->ledPin, !digitalRead(ctx->ledPin));
}

void onDoubleClick(Button2& btn) {
  ButtonCtx* ctx = (ButtonCtx*)btn.getContext();
  Serial.print(ctx->label);
  Serial.println(": double click");
}

void onLongClick(Button2& btn) {
  ButtonCtx* ctx = (ButtonCtx*)btn.getContext();
  Serial.print(ctx->label);
  Serial.println(": long press — LED off");
  digitalWrite(ctx->ledPin, LOW);
}

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n\nCallbackContext Example");

  pinMode(LED_A_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);

  buttonA.begin(BUTTON_A_PIN);
  buttonA.setContext(&ctxA);
  buttonA.setClickHandler(onClick);
  buttonA.setDoubleClickHandler(onDoubleClick);
  buttonA.setLongClickHandler(onLongClick);

  buttonB.begin(BUTTON_B_PIN);
  buttonB.setContext(&ctxB);
  buttonB.setClickHandler(onClick);
  buttonB.setDoubleClickHandler(onDoubleClick);
  buttonB.setLongClickHandler(onLongClick);
}

/////////////////////////////////////////////////////////////////

void loop() {
  buttonA.loop();
  buttonB.loop();
}

/////////////////////////////////////////////////////////////////
