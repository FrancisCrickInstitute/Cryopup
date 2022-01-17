#include <Arduino.h>
#include <analogWrite.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//Encoder and encoder switch variables

Encoder knob(34, 35);
const int encoder_SW = 14;

//OLED display

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   19
#define MAXCS  16 // CS pin Thermocouple 1
#define MAXCLK  5
#define MAXCS2  17 // CS pin Thermocouple 2

// initialize the Thermocouple 1
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// initialize the Thermocouple 2
Adafruit_MAX31855 thermocouple2(MAXCLK, MAXCS2, MAXDO);

// L9958XP driver pins
int DIR = 27;
int PWM = 33;
//int DI = 15; // Disable pin connected to GND
int EN = 32;

//Temperaure variables
double c;
double c2;

double sp;

int FET = 12;
int FET2 = 13;


void setup()
{
  ledcSetup(PWM, 5, 8);
  
  pinMode(DIR, OUTPUT);
  pinMode(PWM, OUTPUT);
  //pinMode(DI, OUTPUT);
  pinMode(EN, OUTPUT);

  digitalWrite(EN, HIGH);
  //digitalWrite(DI, LOW);

  pinMode(FET, OUTPUT);
  pinMode(FET2, OUTPUT);

  digitalWrite(FET, LOW);
  digitalWrite(FET2, LOW);
  
  pinMode(encoder_SW, INPUT_PULLUP);

  Serial.begin(56000);

    Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }

  
    Serial.print("Initializing sensor 2...");
  if (!thermocouple2.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
  Serial.println("DONE.");
  Serial.println("CLEARDATA");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(1000); 
  display.clearDisplay();
  
  delay(2000);
}

void loop()
{
  
  sp = knob.read()*0.125;
  c = thermocouple.readCelsius();
  c2 = thermocouple2.readCelsius();
  OLED_display();
//  Serial.print("DATA,TIME,");
//  Serial.print(millis());
//  Serial.print(",");
  //Serial.println(c2);
    
  if (sp <= c2 && sp < 19) // If the temperature is higher than the setpoint Heat extraction ON and Peltier ON
  {
      sp = knob.read()*0.125;
      c = thermocouple.readCelsius();
      c2 = thermocouple2.readCelsius();
      OLED_display();
      cool();
//      Serial.print("DATA,TIME,");
//      Serial.print(millis());
//      Serial.print(",");
      //Serial.println(c2);
  }

  if (sp > c2 && sp < 19) // If the temperature is lower than the setpoint Heat extraction ON and Peltier OFF
  {
      sp = knob.read()*0.125;
      c = thermocouple.readCelsius();
      c2 = thermocouple2.readCelsius();
      OLED_display();
      stop_cooling();
//      Serial.print("DATA,TIME,");
//      Serial.print(millis());
//      Serial.print(",");
      //Serial.println(c2);
  }

  if (sp <= c2 && sp > 19)
  {
      sp = knob.read()*0.125;
      c = thermocouple.readCelsius();
      c2 = thermocouple2.readCelsius();
      OLED_display();
      stop_heating();
//      Serial.print("DATA,TIME,");
//      Serial.print(millis());
//      Serial.print(",");
      //Serial.println(c2);
  }
  
  if (sp > c2 && sp > 19)
  {
      sp = knob.read()*0.125;
      c = thermocouple.readCelsius();
      c2 = thermocouple2.readCelsius();
      OLED_display();
      heat(24);//23-25 is the safe zone
//      Serial.print("DATA,TIME,");
//      Serial.print(millis());
//      Serial.print(",");
      //Serial.println(c2);
  }


}

void OLED_display()
{   
    display.clearDisplay();
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print(F("C/H bed"));
    display.setTextSize(2);
    display.println();
//    display.println();
    display.print(F("T1:"));
//    display.println();
    display.setTextSize(2);
    display.print(c);
    display.setTextSize(2);
    display.println  (F(" C"));
//    display.println();
    display.print(F("T2:"));
//    display.println();
    display.setTextSize(2);
    display.print(c2);
    display.setTextSize(2);
    display.println(F(" C"));
    display.print(F("DT:"));
    display.print(sp);
    display.println(F(" C"));
    //display.print(digitalRead(encoder_SW));
    display.display(); 
}

void cool()
{
    digitalWrite(FET, HIGH);
    digitalWrite(FET2, HIGH);
}

void heat(int k)
{
  analogWrite(FET, k);
  digitalWrite(FET2, LOW);
}

void stop_cooling()
{
    digitalWrite(FET, LOW);
    digitalWrite(FET2, HIGH);
}

void stop_heating()
{
    analogWrite(FET, 0);
    digitalWrite(FET2, LOW);
}
