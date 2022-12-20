#include <SpacebrewYun.h>



//
// tennsy 3.5 Hardware tests
//

#include <LiquidCrystal.h>
#include <i2c_t3.h>
#include <Encoder.h>
#include <ADC.h>
#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
// Set I2C pins.
 #define WIRE_PINS   I2C_PINS_18_19 // Define where the port is...
 
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  
LiquidCrystal_I2C lcdi2c(0x27,20,4); // address 0x27, 4x20 
Encoder myEnc(3, 2);
File myFile;

SPISettings settingsSD(SPI_FULL_SPEED, MSBFIRST, SPI_MODE0); 
const int chipSelect = BUILTIN_SDCARD;
const int switch_values[]={
  1023, // None pushed
  881,  // select button
  688,  // left button
  244,  // up button
  464,  // down button
  0,    // right button
  -1
};
const char* bnames[] = {
"None",
"Select",
"Left",
"Up",
"Down",
"Right",
"Error",
0
};

int test_button()
{
  int sensorValue = analogRead(A0);
  int i;
 
  for( i = 0 ; switch_values[i] != -1 ; ++i){
    if( sensorValue >= (switch_values[i]-10) && sensorValue <= (switch_values[i]+10)){ 
      return i;
    }
  }
 
  return i;
}
int testSD()
{
  myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
  // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    return 1;
  }
//
  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  return 0;
}
void setup() 
{
  pinMode(LED_BUILTIN,OUTPUT);
   Serial.begin(9600);
  // Set to use internal pullup.
  Wire.begin(I2C_MASTER, 0x00, WIRE_PINS, I2C_PULLUP_INT, 400000); 
  lcdi2c.init();  
  lcdi2c.backlight();
  lcdi2c.setCursor(3,0);
  lcdi2c.print("Hello World!");
  lcdi2c.setCursor(3,3);
  lcdi2c.print("Line 4");
  Serial.print("Initializing SD card...");
  SPISettings(SPI_FULL_SPEED, MSBFIRST, SPI_MODE0);
  if (!SD.begin(chipSelect)) {
    Serial.println("SD initialization failed!");

  } else {
  Serial.println(" SD initialization done.");
  }
 
  
  lcd.begin(16,2);
  lcd.clear();
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}
double frequncy = 10.0E6;
long oldPosition  = -999;
char printBuffer[20];
double multiplier = 1;
double incvalue = 0;
int ignore=0;
int scroll_counter = 0;
void loop() {
  
  long newPosition = myEnc.readAndReset();
  
  // Read which button is pressed
  int b = test_button();
  if(b > 0 && ignore == 0){
    if(b == 3 && multiplier < 1E6) multiplier *= 10;
    if(b == 4 && multiplier > 1) multiplier /= 10;
    ignore = 6;  
  }
  if( ignore > 0) --ignore;
  if (newPosition != oldPosition) {
    if( newPosition > 0) incvalue = -1;
    if( newPosition < 0) incvalue = 1;
    int step = abs(newPosition);
    multiplier = .1;
    if( step > 20 ) multiplier *=10;
    if( step > 50 ) multiplier *=10;
    if( step > 100 ) multiplier *=10;
    if( step > 200 ) multiplier *=10;
    if( step > 500 ) multiplier *=10;
    oldPosition = newPosition;
    Serial.println(newPosition);
    if( frequncy >= 0)
      frequncy +=(double) incvalue * multiplier;
      if( frequncy < 0) frequncy = 0;
  }
  String SerialData="";
  int sensorValue = analogRead(A0);
 
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(bnames[b]);
  lcd.print("  ");
  lcd.print(sensorValue);
  lcd.print("  ");
  lcd.print(newPosition);
  SerialData = String(frequncy,4);
  dtostrf(frequncy/1E6, 5, 6, printBuffer);
  
  lcd.setCursor(0, 0); 
  
  lcd.print(" ");
  lcd.print(printBuffer);
  lcd.print(" Mhz ");

  delay(100);

}
