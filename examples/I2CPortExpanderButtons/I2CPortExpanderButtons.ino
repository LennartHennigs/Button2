/////////////////////////////////////////////////////////////////
/*
  I2C Port Expander Buttons Example

  This example demonstrates the EFFICIENT way to use multiple Button2
  instances with I2C port expanders (PCF8574, MCP23017, etc.).

  PROBLEM: Reading I2C separately for each button creates redundant bus traffic.
  SOLUTION: Read the entire port ONCE per loop, cache it, then have each button
            read from the cached value using bit masking.

  This pattern minimizes I2C transactions and is ideal for:
  - PCF8574 (8-bit I2C expander)
  - MCP23017 (16-bit I2C expander)
  - Any I2C GPIO expander with multiple buttons

  Hardware Setup:
  - Connect PCF8574 to I2C bus (SDA, SCL)
  - Connect buttons to PCF8574 pins P0-P3
  - Each button connects pin to GND (active LOW with pull-up)

  This addresses issue #70:
  https://github.com/LennartHennigs/Button2/issues/70
*/
/////////////////////////////////////////////////////////////////

#include <Wire.h>
#include "Button2.h"

/////////////////////////////////////////////////////////////////
// I2C Configuration

#define PCF8574_ADDRESS 0x20  // Default PCF8574 address (adjust if needed)

// Button pin assignments on the PCF8574
#define BTN0_BIT 0  // P0
#define BTN1_BIT 1  // P1
#define BTN2_BIT 2  // P2
#define BTN3_BIT 3  // P3

/////////////////////////////////////////////////////////////////
// Global state cache - updated ONCE per loop cycle

uint8_t cachedPortState = 0xFF;  // 0xFF = all pins HIGH (not pressed)

/////////////////////////////////////////////////////////////////
// Button instances

Button2 button0;
Button2 button1;
Button2 button2;
Button2 button3;

/////////////////////////////////////////////////////////////////
// State handler functions - read from CACHE, not I2C

uint8_t button0StateHandler() {
  // Extract bit 0 from cached state
  return (cachedPortState & (1 << BTN0_BIT)) ? HIGH : LOW;
}

uint8_t button1StateHandler() {
  // Extract bit 1 from cached state
  return (cachedPortState & (1 << BTN1_BIT)) ? HIGH : LOW;
}

uint8_t button2StateHandler() {
  // Extract bit 2 from cached state
  return (cachedPortState & (1 << BTN2_BIT)) ? HIGH : LOW;
}

uint8_t button3StateHandler() {
  // Extract bit 3 from cached state
  return (cachedPortState & (1 << BTN3_BIT)) ? HIGH : LOW;
}

/////////////////////////////////////////////////////////////////
// I2C Functions

// Read entire port state from PCF8574 (single I2C transaction)
uint8_t readPortExpander() {
  Wire.requestFrom(PCF8574_ADDRESS, 1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0xFF;  // Default: all HIGH (not pressed)
}

// Initialize PCF8574 - set all pins as inputs with pull-ups
void initPortExpander() {
  Wire.begin();

  // Write 0xFF to enable pull-ups on all pins
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(0xFF);
  if (Wire.endTransmission() != 0) {
    Serial.println("ERROR: PCF8574 not found at address 0x20");
    Serial.println("Check wiring and I2C address!");
  } else {
    Serial.println("PCF8574 initialized successfully");
  }
}

/////////////////////////////////////////////////////////////////
// Event Handlers

void handleButton(Button2& btn) {
  switch (btn.getType()) {
    case clickType::single_click:
      Serial.print("Button ");
      Serial.print(btn.getID());
      Serial.println(" - Single click");
      break;
    case clickType::double_click:
      Serial.print("Button ");
      Serial.print(btn.getID());
      Serial.println(" - Double click");
      break;
    case clickType::triple_click:
      Serial.print("Button ");
      Serial.print(btn.getID());
      Serial.println(" - Triple click");
      break;
    case clickType::long_click:
      Serial.print("Button ");
      Serial.print(btn.getID());
      Serial.print(" - Long click (");
      Serial.print(btn.wasPressedFor());
      Serial.println("ms)");
      break;
  }
}

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\n=================================");
  Serial.println("I2C Port Expander Buttons Example");
  Serial.println("=================================\n");

  // Initialize I2C port expander with initialization callback
  button0.begin(BTN_VIRTUAL_PIN, INPUT, false, initPortExpander);
  button0.setID(0);
  button0.setButtonStateFunction(button0StateHandler);
  button0.setClickHandler(handleButton);
  button0.setLongClickHandler(handleButton);

  // Remaining buttons don't need init callback (already initialized)
  button1.begin(BTN_VIRTUAL_PIN, INPUT, false);
  button1.setID(1);
  button1.setButtonStateFunction(button1StateHandler);
  button1.setClickHandler(handleButton);
  button1.setLongClickHandler(handleButton);

  button2.begin(BTN_VIRTUAL_PIN, INPUT, false);
  button2.setID(2);
  button2.setButtonStateFunction(button2StateHandler);
  button2.setClickHandler(handleButton);
  button2.setLongClickHandler(handleButton);

  button3.begin(BTN_VIRTUAL_PIN, INPUT, false);
  button3.setID(3);
  button3.setButtonStateFunction(button3StateHandler);
  button3.setClickHandler(handleButton);
  button3.setLongClickHandler(handleButton);

  Serial.println("Ready! Press buttons on PCF8574 pins P0-P3\n");
  Serial.println("Key Efficiency Feature:");
  Serial.println("- I2C read happens ONCE per loop cycle");
  Serial.println("- All 4 buttons read from cached state");
  Serial.println("- Minimal I2C bus traffic\n");
}

/////////////////////////////////////////////////////////////////

void loop() {
  // CRITICAL: Update cache with single I2C read BEFORE button loops
  // This is the KEY to efficiency - one I2C transaction per cycle
  cachedPortState = readPortExpander();

  // All buttons now read from the cached state (no I2C calls here!)
  button0.loop();
  button1.loop();
  button2.loop();
  button3.loop();

  // Optional: Add a small delay if needed
  // delay(1);
}

/////////////////////////////////////////////////////////////////
/*
  PERFORMANCE NOTES:

  WITHOUT caching (inefficient):
  - loop() calls: button0.loop() -> I2C read
  - loop() calls: button1.loop() -> I2C read
  - loop() calls: button2.loop() -> I2C read
  - loop() calls: button3.loop() -> I2C read
  - Total: 4 I2C transactions per cycle

  WITH caching (this example):
  - loop() reads I2C once -> updates cache
  - button0.loop() -> reads from cache (fast!)
  - button1.loop() -> reads from cache (fast!)
  - button2.loop() -> reads from cache (fast!)
  - button3.loop() -> reads from cache (fast!)
  - Total: 1 I2C transaction per cycle

  SCALABILITY:
  - Add more buttons? Just add more bit-mask handlers
  - 8 buttons on PCF8574? Still 1 I2C read per cycle
  - 16 buttons on MCP23017? Still 1-2 I2C reads per cycle

  ADAPTATION FOR OTHER EXPANDERS:

  For MCP23017 (16-bit):
  - Use uint16_t for cachedPortState
  - Read both GPIOA and GPIOB registers
  - Create 16 state handlers (one per bit)

  For TCA9555 or similar:
  - Same pattern, adjust I2C read commands
  - Cache the full port state
  - Bit-mask for each button
*/
/////////////////////////////////////////////////////////////////
