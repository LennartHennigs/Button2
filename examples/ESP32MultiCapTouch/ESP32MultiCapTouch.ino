/////////////////////////////////////////////////////////////////

#if !defined(ESP32)
  #error This sketch needs an ESP32
#else

/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////

Button2 btn1, btn2 ;
uint8_t pin1 = 4;
uint8_t pin2 = 5;

/////////////////////////////////////////////////////////////////

byte capStateHandler(const Button2& btn) {
    int capa = touchRead(btn.getID());
    return capa < btn.getDebounceTime() ? LOW : HIGH;
}

/////////////////////////////////////////////////////////////////

void click(Button2& btn) {
    Serial.println("click\n");
}

/////////////////////////////////////////////////////////////////

void setup() {
    Serial.begin(9600);
    delay(50);
    Serial.println("\n\nCapacitive Touch Demo");
    
    btn1.setID(pin1);
    btn1.setDebounceTime(35);
    btn1.setButtonStateFunction(capStateHandler);
    btn1.setClickHandler(click);
    btn1.begin(BTN_VIRTUAL_PIN);
    
    btn2.setID(pin2);
    btn2.setDebounceTime(35);
    btn2.setButtonStateFunction(capStateHandler);
    btn2.setClickHandler(click);
    btn2.begin(BTN_VIRTUAL_PIN);

}

/////////////////////////////////////////////////////////////////

void loop() {
    btn1.loop();
    btn2.loop();
}

/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
