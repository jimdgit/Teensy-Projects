/*
* ARDUINO TESTCODE FOR AD9833 Micro Waveform Generator
* https://www.changpuak.ch/electronics/AD9833-MicroWaveformGenerator.php
* Software Version 1.0, 
* 25.04.2016, Alexander C. Frank
*/

//#include "Wire.h" 
//#define MAX5381 0x64 
// Suffix "L" : 0x60
// Suffix "M" : 0x62
// Suffix "N" : 0x64
// Suffix "P" : 0x66
#include "TimerOne.h"
int TimerUpdateRate = 1000;

//const int SOURCE = A5; // Reset
const int FSYNC = 11; 
const int SDATA = 12; 
const int SCLK = 13; 

const float CRYSTAL = 75000000.0 ;
const int SINE = 0x2000; 
const int SQUARE = 0x2020; 
const int TRIANGLE = 0x2002; 
unsigned long FREQ = 1000; 

char Command ; 
byte byteRead ;


// ///////////////////////////////////////////////////////////// 
//! Updates the DDS registers
// ///////////////////////////////////////////////////////////// 
void UpdateDDS(unsigned int data){
  unsigned int pointer = 0x8000;
  // Serial.print(data,HEX);Serial.print(" ");
  digitalWrite(FSYNC, LOW); // AND NOW : WAIT 5 ns
  
  for(int i=0; i<16; i++){
      if((data & pointer) > 0) { 
        digitalWrite(SDATA, HIGH); 
      }
      else { 
        digitalWrite(SDATA, LOW); 
      }
      digitalWrite(SCLK, LOW);
      digitalWrite(SCLK, HIGH);
      pointer = pointer >> 1 ;
   }
  digitalWrite(FSYNC, HIGH);
}

// ///////////////////////////////////////////////////////////// 
//! Updates the Freq registers
// ///////////////////////////////////////////////////////////// 
void UpdateFreq(long FREQ, int WAVE){

    long FTW = (FREQ * pow(2, 28)) / CRYSTAL;
    if (WAVE == SQUARE) FTW = FTW << 1;
  
    unsigned int MSB = (int)((FTW & 0xFFFC000) >> 14); 
    unsigned int LSB = (int)(FTW & 0x3FFF);
    LSB |= 0x4000;
    MSB |= 0x4000; 
    UpdateDDS(0x2100); 
    UpdateDDS(LSB); 
    UpdateDDS(MSB); 
    UpdateDDS(0xC000); 
    UpdateDDS(WAVE);

}

// ///////////////////////////////////////////////////////////// 
//! Reads User Input from Serial
// ///////////////////////////////////////////////////////////// 
void ReadUserInput()
{
  boolean ende = false ;
  FREQ = 0;
  while ( ( Serial.available() ) || ( ende == false ) ) 
  {
    byteRead = Serial.read();
    if ( byteRead == 10 ){ ende = true ; }
    
    if (( byteRead > 64 ) & (byteRead < 91)) { 
      Command = byteRead ; 
    } // A..Z
    
    if (( byteRead > 47 ) & (byteRead < 58)) { 
      FREQ = FREQ * 10;
      FREQ = FREQ + byteRead - 48 ; 
    } // 0..9
  }
}


// ///////////////////////////////////////////////////////////// 

void setup() {
//    pinMode(SOURCE, OUTPUT);
    pinMode(FSYNC, OUTPUT);
    pinMode(SDATA, OUTPUT);
    pinMode(SCLK, OUTPUT);
//    digitalWrite(SOURCE, LOW);
    digitalWrite(FSYNC, HIGH);
    digitalWrite(SDATA, LOW);
    digitalWrite(SCLK, HIGH);
    
//    Wire.begin();
    Serial.begin(9600);
    // Micro Hack fails ....
    // while (!Serial) { delay(10); // wait for serial port to connect. }
    delay(6999);
    Serial.println("AD9833 Micro Waveform Generator V1.0");
    Serial.println("-------------------------------------------------------");
    Serial.println("COMMAND : [WAVEFORM]+[FREQUENCY IN HERTZ]");
    Serial.println("WAVEFORM : S-SINE, T-TRIANGLE, R-RECTANGLE, A-ARBITRARY");
    Serial.println("E.G:: S1000 OUTPUTS A SINE AT 1000 Hz");
    Serial.println("-------------------------------------------------------");
    // INIT DDS, AS DESCRIBED IN AN-1070, "Programming the AD9833/AD9834"
    // BY LIAM RIORDAN (ANALOG DEVICES)
    UpdateDDS(0x2100); 
    UpdateDDS(0x50C7); 
    UpdateDDS(0x4000); 
    UpdateDDS(0xC000); 
    UpdateDDS(0x2000); 
    // SWITCH TO SOURCE = DDS 
//    digitalWrite(SOURCE, LOW);
    // NOW THERE SHOULD BE A 
    // 384 Hz SIGNAL AT THE OUTPUT 
    delay(999);
}

void loop() {

ReadUserInput();

  switch (Command)
  {
  
      case 82 :
        // THAT WAS AN "R" FOR "DAC data MSB" 
        Serial.print("\nRECTANGLE, FREQ = ");
        Serial.print(FREQ,DEC);Serial.println(" Hz");
        UpdateFreq(FREQ, SQUARE);
 //       digitalWrite(SOURCE, LOW);
        Serial.println("\nO.K.");
      break;
    
      case 83 :
        // THAT WAS AN "S" FOR "Sinusoid" 
        Serial.print("\nSINE, FREQ = ");
        Serial.print(FREQ,DEC);Serial.println(" Hz");
        UpdateFreq(FREQ, SINE);
 //       digitalWrite(SOURCE, LOW);
        Serial.println("\nO.K.");
      break;
      
      case 84 :
        // THAT WAS AN "T" FOR "Triangle" 
        Serial.print("\nTRIANGLE, FREQ = ");
        Serial.print(FREQ,DEC);Serial.println(" Hz");
        UpdateFreq(FREQ, TRIANGLE);
//        digitalWrite(SOURCE, LOW);
        Serial.println("\nO.K.");
      break;
      
      default:
        Serial.println("\nUnknown Command.");
        Serial.println("Try again :-)");
        delay(100);
      break;
    } 
}

// ///////////////////////////////////////////////////////////// 
// END OF FILE.
// ///////////////////////////////////////////////////////////// 
