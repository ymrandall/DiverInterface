//-------------------------------------------------------------------------------
//  Diver Interface
//  Last Updated 7 March 2018
//  
//  Diver Interface for interface with TinyCircuits TinyScreen
//
//  Written by Yelena Randall and Justin Lewis (URI)
//
//-------------------------------------------------------------------------------

#include <Wire.h>
#include <SPI.h>
#include <TinyScreen.h>
#include <TimeLib.h> //include the Arduino Time library

//Pin names
const int aPin0 = 0;
const int aPin2 = 2;

//Variables/constants
int Vin;
int Vin2;
float Temp;
const float R2 = 82000; //resistance 
const float c = 1500; //m/s, sound of speed in water
const float pi = 3.14;
float thermRes;
float I;
float Vin_f;
float Vin_f2;
float V_therm;
float l;
int y;
int R;
const float A = -13.01169583*(pow(10,-3));
const float B = 27.64739816*(pow(10,-4));
const float C = -112.1391350*(pow(10,-7));
int CompassX;
int CompassY;
int CompassZ;
int Heading;
char brightnessChanged = 0;
char brightness = 5;

// Color definitions
#define  BLACK           0x00
#define BLUE            0xE0
#define RED             0x03
#define GREEN           0x1C
#define DGREEN           0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define BROWN           0x32
#define HMC5883_I2CADDR     0x1E

//Library must be passed the board type
//TinyScreenDefault for TinyScreen shields
//TinyScreenAlternate for alternate address TinyScreen shields
//TinyScreenPlus for TinyScreen+

TinyScreen display = TinyScreen(TinyScreenDefault);

void setup(void) {
  Wire.begin(); //Initialize I2C
  display.begin();
  display.setBrightness(10); //set brightness from 0-15
  HMC5883nit();
  
  //initialize analog pins
  pinMode(aPin0, INPUT);
  pinMode(aPin2, INPUT);

  //Initialize serial comm, 9600 baud rate
  Serial.begin(9600);
  
  //set text
  display.setFont(thinPixel7_10ptFontInfo);
  int width=5;
  display.fontColor(TS_8b_White,TS_8b_Black);

  //time
  setTime(13,11,00,2,3,2018);    //values in the order hr,min,sec,day,month,year
}

void loop() {
  //display.clearScreen();
  readTime();
  readTemp();
  drawPixels();
  HMC5883ReadCompass();
  
  display.setCursor(0, 20);
  display.fontColor(BLUE, BLACK);
  display.print("Heading: ");
  display.print(Heading);
  //display.setCursor(0,30);
  //display.print("y: ");
  //display.print(CompassY);
  //display.setCursor(0,40);
  //display.print("z: ");
  //display.print(CompassZ);
  
  //drawPixels();
  buttonLoop();

}

void readTime(){
  display.setCursor(2,2); //Set the cursor where you want to start printing the time
  if(hour()<10) display.print(0); //print a leading 0 if hour value is less than 0
  display.print(hour());
  display.print(":");
  if(minute()<10) display.print(0); //print a leading 0 if minute value is less than 0
  display.print(minute());
  display.print(":");
  if(second()<10) display.print(0); //print a leading 0 if seconds value is less than 0
  display.print(second());
  display.print(" "); //just a empty space after the seconds
  delay(1000);      
}

void readTemp(){
  Vin = analogRead(aPin0); //read V from pin 0
  Vin_f = 5*float(Vin)/1023; //Vin forward

  Vin2 = analogRead(aPin2); //read V from pin 1
  Vin_f2 = 5*float(Vin2)/1023; //Vin2 forward
  
  thermRes = float(R2*((Vin_f/Vin_f2)-1));; //resistance across thermistor
  l = log(thermRes); //for Steinhart-Hart equation
  Temp = (1 / (A + B*l + C*l*l*l))-273.15; //Temperature in C, Steinhart-Hart Equation

  //display temp
  display.setCursor(2,54);
  display.print("Temp: ");
  display.print(Temp);
  display.print(" *C");
}

void HMC5883nit()
{
  //Put the HMC5883 into operating mode
  Wire.beginTransmission(HMC5883_I2CADDR);
  Wire.write(0x02);     // Mode register
  Wire.write(0x00);     // Continuous measurement mode
  Wire.endTransmission();

}


void HMC5883ReadCompass()
{
  uint8_t ReadBuff[6];

  // Read the 6 data bytes from the HMC5883
  Wire.beginTransmission(HMC5883_I2CADDR);
  Wire.write(0x03);
  Wire.endTransmission();
  Wire.requestFrom(HMC5883_I2CADDR, 6);

  for (int i = 0; i < 6; i++)
  {
    ReadBuff[i] = Wire.read();
  }

  CompassX = ReadBuff[0] << 8;
  CompassX |= ReadBuff[1];

  CompassY = ReadBuff[4] << 8;
  CompassY |= ReadBuff[5];

  CompassZ = ReadBuff[2] << 8;
  CompassZ |= ReadBuff[3];

//Calculate heading 
//declination angle in RI is 15deg (still need to add or subtract to find true North)

  if (CompassY > 0 ){
    Heading =90 - (atan(CompassX/CompassY))*(180/pi);
    }

  if (CompassY < 0){
    Heading =270 - (atan(CompassX/CompassY))*(180/pi);
    }

  if (CompassY == 0 && CompassX < 0){
    Heading = 180;
    }

  if (CompassY == 0 && CompassX > 0){
    Heading = 0;
    }
}

void writeText(){
  display.clearScreen();
  //setFont sets a font info header from font.h
  //information for generating new fonts is included in font.h
  display.setFont(thinPixel7_10ptFontInfo);
  //getPrintWidth(character array);//get the pixel print width of a string
  int width=display.getPrintWidth("Yelena's Test");
  //setCursor(x,y);//set text cursor position to (x,y)- in this example, the example string is centered
  display.setCursor(48-(width/2),10);
  //fontColor(text color, background color);//sets text and background color
  //display.fontColor(TS_8b_Green,TS_8b_Black);
  //display.print("Yelena's Test");
  display.drawRect(48,48,10,10,TSRectangleFilled,TS_8b_Blue);
  display.drawLine(30,30,60,30,TS_8b_Red);
}

void drawPixels(){
  //writing pixels one by one is slow, but neccessary for drawing shapes other than lines and rectangles
  //circle drawing algorithm from http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
  drawCircle(70,25,25,TS_8b_Red);
}

void drawCircle(int x0, int y0, int radius, uint8_t color)
{
  int x = radius;
  int y = 0;
  int radiusError = 1-x;
 
  while(x >= y)
  {
    //drawPixel(x,y,color);//set pixel (x,y) to specified color
    display.drawPixel(x + x0, y + y0, color);
    display.drawPixel(y + x0, x + y0, color);
    display.drawPixel(-x + x0, y + y0, color);
    display.drawPixel(-y + x0, x + y0, color);
    display.drawPixel(-x + x0, -y + y0, color);
    display.drawPixel(-y + x0, -x + y0, color);
    display.drawPixel(x + x0, -y + y0, color);
    display.drawPixel(y + x0, -x + y0, color);
    y++;
    if (radiusError<0)
    {
      radiusError += 2 * y + 1;
    }
    else
    {
      x--;
      radiusError += 2 * (y - x) + 1;
    }
  }
}

void pingResponse() {

while (display.getButtons(TSButtonLowerLeft)==0){
  while (Serial.available()){
    int res = Serial.read(); //read serial, response should be #RxxxTyyyyy (xxx is unit tag, yyyyy used to calculate range)
    //y = res[5]; HOW TO INDEX serial line??????
    R = y*c*6.25*exp(-5); //calculate range in meters
    display.clearScreen();
    display.setCursor(42,32); //display in center
    display.println(R); //display range on screen
    
    display.setCursor(0, 54); //display in bottom left corner
    display.println("Press to return");
  }
 }  
}

void buttonLoop() {
  display.setCursor(0, 0);
  if (display.getButtons(TSButtonUpperLeft)) {
    display.println("Pinged!");
    Serial.println("$Pxxx"); //change based on unit tag#
    pingResponse();         
  } else 
    display.println("          ");
  }

