//
// This is for the Tennsy.
// Library version:1.1
#include <i2c_t3.h> // This is the tennsy Wire lib.
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
 #define WIRE_PINS   I2C_PINS_18_19 // Define where the port is...
void setup()
{
  // If you already have pull ups you may need to change this to I2C_PULLUP_EXT
  Wire.begin(I2C_MASTER, 0x00, WIRE_PINS, I2C_PULLUP_INT, 400000); 
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
  lcd.setCursor(2,1);
  lcd.print("Teensy Arduino!");
   lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
   lcd.setCursor(2,3);
  lcd.print("Teensy version");
}


void loop()
{
}
