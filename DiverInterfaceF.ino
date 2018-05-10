//-------------------------------------------------------------------------------------------//
//                                                                                           //
//  Diver Interface                                                                          //
//  Last Updated 10 May 2018                                                                 // 
//                                                                                           //
//  For interface with TinyScreen and Nanomodems by Jeff Neasham, Newcastle University       //             
//                                                                                           //
//  Written by Yelena Randall and Justin Lewis (URI)                                         //     
//  Contact: ymrandall@my.uri.edu, jlewis001@my.uri.edu                                      //
//                                                                                           //
//  MIT License                                                                              //
//                                                                                           //
//  Copyright (c) [2018] [Yelena Randall & Justin Lewis - University of Rhode Island]        //
//  Sponsored by: [Dr. Bridget Buxton]                                                       //
//                                                                                           //
//  Permission is hereby granted, free of charge, to any person obtaining a copy             //
//  of this software and associated documentation files, to deal                             //
//  in the Software without restriction, including without limitation the rights             //
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell                //
//  copies of the Software, and to permit persons to whom the Software is                    //
//  furnished to do so, subject to the following conditions:                                 //
//                                                                                           //
//  The above copyright notice and this permission notice shall be included in all           //
//  copies or substantial portions of the Software.                                          //
//                                                                                           //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR               //
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                 //
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE              //
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                   //
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,            //
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE            //
//  SOFTWARE.                                                                                //
//                                                                                           //
//                                                                                           //
//-------------------------------------------------------------------------------------------//


///////////////////////////////////////////Libraries///////////////////////////////////////////
#include <Wire.h>
#include <SPI.h>
#include <TinyScreen.h>
#include <TimeLib.h> // Arduino time library - https://playground.arduino.cc/Code/Time
#include <SoftwareSerial.h>
#include "BMA250.h"

///////////////////////////////////////////Variables///////////////////////////////////////////
// Tiny Screen constants
char brightnessChanged = 0; char brightness = 5; int width=5;

// Analog pins for use with thermistor 
const int aPin0 = 0; const int aPin2 = 2;

// Thermistor variables
int Vin, Vin2; float Temp, thermRes, I, Vin_f, Vin_f2, V_therm, l;

// Thermistor constants
const float R2 = 82000;                     // Resistance 
const float c = 1500;                       // m/s, sound of speed in water
const float pi = 3.141592654;               // Pi
const float A = 3.412026533*(pow(10,-3));   // Steinhart-hart constants
const float B = -0.9356856656*(pow(10,-4)); // based on temp calibration
const float C = 11.49459621*(pow(10,-7));   // http://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm

// Compass variables
int x, y, z, Degrees;

// Compass constants
#define HMC5883_I2CADDR  0x1E               // Define compass
int x_max=-10000;                           // Starting values for hard iron calibration
int y_max=-10000;                           // These values should be extreme in the 
int x_min=10000;                            // opposite direction so it calibrates nicely
int y_min=10000;
int z_max=10000;
int z_min=10000;

// Time 1 sec delay
unsigned long previousMillis = 0;           // To store last time clock was updated
const long interval = 1000;                 // 1 Second interval

// Temp 1 sec delay
unsigned long previousMillist = 0;          // To store last time clock was updated

int R;                                      // Range for modems
const int RX = 2;     
const int TX = 3;
int count=0;

String pin[] = {"000", "001", "002"};
String pinfin;
int ipin = 0;

// Accelerometer
BMA250 accel;
int ax, ay, az, axh, ayh, pitch, roll;

///////////////////////////////////////////Setup///////////////////////////////////////////
// Indicates version of TinyScreen display
TinyScreen display = TinyScreen(TinyScreenDefault); 
SoftwareSerial Serial1(2,3);

void setup() {
  Wire.begin();                             // Initialize I2C
  display.begin();                          // Initialize display
  display.setBrightness(10);                // Set brightness from 0-15
  display.setFlip(1);
 
  accel.begin(BMA250_range_2g, BMA250_update_time_64ms);//This sets up the BMA250 accelerometer
  
  HMC5883nit();                             // Initialize compass
          
  pinMode(aPin0, INPUT);                    // Initialize analog pin 0
  pinMode(aPin2, INPUT);                    // Initialize analog pin 2

  Serial1.begin(9600);
  
  display.setFont(thinPixel7_10ptFontInfo);// Set text size/font
  display.fontColor(TS_8b_White,TS_8b_Black);// Set text color

  setTime(13,58,00,2,3,2018);               // Set time (hr,min,sec,day,month,year)
  
  boot();
  pinFinder();
  updateTime();
  compassMessage();
  
  //Arrow for compass heading
  display.drawLine(2,30,7,20,TS_8b_White);
  display.drawLine(7,45,7,20,TS_8b_White);      
  display.drawLine(12,30,7,20,TS_8b_White); 
  
}


//////////////////////////////////////////////Main//////////////////////////////////////////////
void loop() {
  dispHeading();
  buttonLoop();
  readTime();
  readTemp();
  delay(100);
}

///////////////////////////////////////////Functions///////////////////////////////////////////
void boot (){
  // to select unit ID to communicate with partner modem
  display.setFont(liberationSansNarrow_16ptFontInfo);// Set text size/font
  display.fontColor(TS_8b_Red, TS_8b_Black);
  display.setCursor(25,22);   
  display.print("DigSki"); 
  delay(2000);
  display.clearScreen();

  while (display.getButtons(TSButtonUpperRight) == 0) { // Right button is used to confirm selection
    display.setFont(thinPixel7_10ptFontInfo);   // Set text size/font
    display.fontColor(TS_8b_Red, TS_8b_Black);
    display.setCursor(10,2);                     // Set position on screen
    display.print("Select unit ID");
    display.fontColor(TS_8b_White, TS_8b_Black); // Font color
    display.setCursor(2,15);
    display.print("<Change | Confirm>");

    if (display.getButtons(TSButtonUpperLeft)) { // Left button is used to cycle through options
      display.fontColor(TS_8b_Red, TS_8b_Black);
      display.setCursor(38,35);
      display.print(pin[ipin]);
      delay(500);
      ipin ++;
    if (ipin>2) ipin = 0;
    }
    }
    pinfin = pin[ipin]; // set pinfin as selected ID
  }

void compassMessage(){
    // Instructions to do a "hard-iron" calibration
    display.clearScreen();
    display.setCursor(2,2);
    display.print("Calibrate compass");
    display.setCursor(2,14);
    display.print("by rotating on all");
    display.setCursor(2,26);
    display.print("three axes of");
    display.setCursor(2,38);
    display.print("device.");
    delay(3000);
    display.clearScreen();
  }
  
void pinFinder(){
  // query modem to find ping ID, display on screen
  
  display.fontColor(TS_8b_Blue, TS_8b_Black);
  Serial1.println("$?");

  if (Serial1.available()) {
      String response = Serial1.readString();       // Read serial, response should be 
                                              // #AxxxVyyyy (xxx is node adress, 
                                              // yyyyy is 10-bit battery voltage monitor value)                                
      
      String mypin = response.substring(2,4);
      display.setCursor(88,2);
      display.print(mypin);
      }
  }
  
void readTime(){
  // Display time
  
  unsigned long currentMillis = millis();
  display.setFont(liberationSansNarrow_12ptFontInfo);
  display.fontColor(TS_8b_White, TS_8b_Black); // Font color
  display.setCursor(2,2);                     // Set position on screen
  
  //if (currentMillis - previousMillis >= interval) {
    //previousMillis = currentMillis;           // Save last time clock was updated
    if(hour()<10) display.print(0);           // Print a leading 0 if hour value is less than 0
    display.print(hour());
    display.print(":");
    if(minute()<10) display.print(0);         // Print a leading 0 if minute value is less than 0
    display.print(minute());
    display.print(":");
    if(second()<10) display.print(0);         // Print a leading 0 if seconds value is less than 0
    display.print(second());
    display.print(" ");                       // Empty space after seconds 
  //}
}

void updateTime(){
  // Set time for bootup process
  
  display.clearScreen();
  display.setFont(thinPixel7_10ptFontInfo);   // Set text size/font
  display.fontColor(TS_8b_Red, TS_8b_Black); // Font color
  display.setCursor(20,2);
  display.print("SET TIME");
  display.fontColor(TS_8b_White, TS_8b_Black); // Font color
  display.setCursor(2,15);
  display.print("<Change | Confirm>");
  display.setCursor(15,40);
  display.print("Set Hour:");
  int myhour=hour();

  // Right button to confirm, left to cycle through options
  
  while(display.getButtons(TSButtonUpperRight) == 0) {
    display.setCursor(75,40);
    if(myhour<10) display.print("0");
    display.print(myhour);
    if(display.getButtons(TSButtonUpperLeft))
      myhour++;
    if(myhour>23) myhour = 0;
    delay(200);
    }
    
  setTime(myhour, minute(), second(),day(),month(),year());
  delay(500);
  display.clearScreen();
  display.setCursor(10,25);
  display.print("Hour Saved");
  delay(1000);
  
  display.clearScreen();
  display.fontColor(TS_8b_Red, TS_8b_Black); // Font color
  display.setCursor(20,2);
  display.print("SET TIME");
  display.fontColor(TS_8b_White, TS_8b_Black); // Font color
  display.setCursor(2,15);
  display.print("<Change | Confirm>");
  display.setCursor(15,40);
  display.print("Set Minutes:");
  int myminute=minute();
  
  while(display.getButtons(TSButtonUpperRight) == 0) {
    display.setCursor(75,40);
    if(myminute<10) display.print("0");
    display.print(myminute);
    if(display.getButtons(TSButtonUpperLeft))
      myminute++;
    if(myminute>59) myminute = 0;
    delay(200);
    }
    
  setTime(hour(), myminute, second(), day(), month(), year());
  delay(500);
  display.clearScreen();
  display.setCursor(10,25);
  display.print("Minute Saved");
  delay(1000);

  display.clearScreen();
  display.fontColor(TS_8b_Red, TS_8b_Black); // Font color
  display.setCursor(20,2);
  display.print("SET TIME");
  display.fontColor(TS_8b_White, TS_8b_Black); // Font color
  display.setCursor(2,15);
  display.print("<Change | Confirm>");
  display.setCursor(15,40);
  display.print("Set Seconds:");
  int mysecond=second(); 
  mysecond = 0;

  while(display.getButtons(TSButtonUpperRight) == 0) {
    display.setCursor(75,40);
    if(mysecond<10) display.print("0");
    display.print(mysecond);
    if(display.getButtons(TSButtonUpperLeft))
      mysecond++;
    if(mysecond>59) mysecond = 0;
    delay(200);
    }
   
   setTime(hour(), minute(), mysecond, day(), month(), year());
    delay(500);
    display.clearScreen();
    display.setCursor(10,25);
    display.print("Seconds Saved");
    delay(1000);
    display.clearScreen();
  }
  

void readTemp(){
  // Read temperature from thermistor 
  
  unsigned long currentMillist = millis();
  
  Vin = analogRead(aPin0);                    // Read V from pin 0
  Vin_f = 5*float(Vin)/1023;                  // Vin forward

  Vin2 = analogRead(aPin2);                   // Read V from pin 1
  Vin_f2 = 5*float(Vin2)/1023;                // Vin2 forward
  
  thermRes = float(R2*((Vin_f/Vin_f2)-1));    // Resistance across thermistor
  l = log(thermRes);                          // Log calculation for Steinhart-Hart equation
  Temp = (1 / (A + B*l + C*l*l*l))-273.15;    // Temperature in C, Steinhart-Hart Equation
  
  if (currentMillist - previousMillist >= interval) {
    previousMillist = currentMillist;           
    // Display temperature
    display.setFont(thinPixel7_10ptFontInfo);
    display.fontColor(TS_8b_Blue, TS_8b_Black);
    display.setCursor(2,54);
    display.print("Temp: ");
    display.print(Temp);
    display.print(" *C");
  }
  display.setFont(thinPixel7_10ptFontInfo);// Set text size/font
  display.fontColor(TS_8b_White,TS_8b_Black);// Set text color
}


// Based on TinyCompass demo by Tony Batey 
// https://www.hackster.io/tbatey_tiny-circuits/tinycompass-32de65
void HMC5883nit(){
  // Put the HMC5883 3-axis magnetometer into operating mode
  Wire.beginTransmission(HMC5883_I2CADDR);
  Wire.write(0x02);                           // Mode register
  Wire.write(0x00);                           // Continuous measurement mode
  Wire.endTransmission();
}

void dispHeading(){
  // To display compass heading on screen
  
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_White,TS_8b_Black);
  compassCal();
  display.setCursor(20,22);

  Degrees = Degrees-270;                      // Adjust 270 degrees because the sensor is pointing right
  if (Degrees < 0) Degrees=Degrees+360;
  if(Degrees<30 || Degrees> 330) display.print("N  ");
  if(Degrees >= 30 && Degrees< 60) display.print("NW "); 
  if(Degrees >= 60 && Degrees< 120) display.print("W  "); 
  if(Degrees >= 120 && Degrees< 150) display.print("SW ");
  if(Degrees >= 150 && Degrees< 210) display.print("S ");
  if(Degrees >= 210 && Degrees< 240) display.print("SE ");
  if(Degrees >= 240 && Degrees< 300) display.print("E  "); 
  if(Degrees >= 300 && Degrees< 330) display.print("NE ");
    
  display.setCursor(20,37);
  Degrees = abs(Degrees - 360);
  display.print(Degrees);                     // Display the heading in degrees
  display.print("  ");      
}

void tiltCorrection(){
  // Read accelerometer data
  
  accel.read();//This function gets new data from the accelerometer
  ax = accel.X;
  ay = accel.Y;
  az = accel.Z;
  pitch = asin(-ax);
  roll = asin(ay/(cos(pitch)));
  }

void compassCal(){
  // calibrate compass
  
  readMyCompass();                            // Read compass
  
  if(x > x_max)                               // Find values of hard iron distortion
    x_max = x;                                // This will store the max and min values
  if(y >y_max)                                // of the magnetic field around you
     y_max = y;
  if(y<y_min)
     y_min = y;
  if(x<x_min)
     x_min = x;
  if(z > z_max)
    z_max = z;
  if(z < z_min)
    z_min = z;
  
  int xoffset= (x_max+x_min)/2;
  int yoffset= (y_max+y_min)/2;
  int zoffset= (z_max+z_min)/2;
  
  int x_scale = x-xoffset;                    // Math to compensate for hard 
  int y_scale = y-yoffset;                    // iron distortions
  int z_scale = z-zoffset;

  tiltCorrection();
  
  // incorporate accelerometer data to account for small tilts of the device in pitch and roll 
  axh = (x_scale*(1-sq(pitch))) - (y_scale*pitch*roll) - (z_scale*pitch*sqrt(1-sq(pitch)-sq(roll)));
  ayh = (y_scale*sqrt(1-sq(pitch)-sq(roll))) - (z_scale*roll);
  
  float heading = atan2(axh,ayh);            // Heading in radians
  
  // Heading between 0 and 6.3 radians
  if(heading < 0)
    heading += 2*PI;
    
  if(heading>2*PI)
    heading -= 2*PI;
  
  Degrees = heading * 180/M_PI;               // Conversion to degrees  
}

void readMyCompass(){
  // read compass
  
  Wire.beginTransmission(HMC5883_I2CADDR);
  Wire.write(byte(0x03));                     // Send request to X MSB register
  Wire.endTransmission();
  Wire.requestFrom(HMC5883_I2CADDR, 6);       // Request 6 bytes; 2 bytes per axis
  
  if(Wire.available() <=6){                   // If 6 bytes available    
    x = (int16_t)(Wire.read() << 8 | Wire.read());
    z = (int16_t)(Wire.read() << 8 | Wire.read());
    y = (int16_t)(Wire.read() << 8 | Wire.read());
  }
}

void pingResponse() {
  // Modem ping function
  
  while (display.getButtons(TSButtonUpperRight) == 0) {
      
    if (Serial1.available()) {
      String res = Serial1.readString();       // Read serial, response should be 
                                              // #RxxxTyyyyy (xxx is unit tag, 
                                              // yyyyy used to calculate range)                                
      
      String character = res.substring(6);    // Store data into character string after 6
      
      y = character.toInt();                  // Convert to int for calc

      R = y * c * 6.25 * exp(-5);             // Calculate range in meters
      display.clearScreen();
      display.setCursor(38, 32);              // Display position
      display.println("Range: ");
      display.println(R);
      display.setCursor(14, 10);               // Display position
      display.println("Press to return>");
    }
  }
  display.clearScreen();
  
  // Redraw arrow for compass heading
  display.drawLine(2,30,7,20,TS_8b_White);
  display.drawLine(7,45,7,20,TS_8b_White);      
  display.drawLine(12,30,7,20,TS_8b_White); 
}

void buttonLoop() {
  // Function to send ping command
  if (display.getButtons(TSButtonUpperLeft)) {
    display.clearScreen();
    display.setCursor(0,0);
    display.println("Pinged!");
    Serial1.print("$P000");
    pingResponse();
    delay(500);
  }
}




