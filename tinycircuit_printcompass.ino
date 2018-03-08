
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


#include <TinyScreen.h>
#include <SPI.h>
#include <Wire.h>


int CompassX;
int CompassY;
int CompassZ;
char brightnessChanged = 0;
char brightness = 5;



TinyScreen display = TinyScreen(TinyScreenDefault);

void setup() {
  Wire.begin();
  Serial.begin(115200);
  HMC5883nit();

  display.begin();
  display.setBrightness(brightness);

}

void loop() {
  // put your main code here, to run repeatedly:
  // Print out the compass data
  HMC5883ReadCompass();
  ButtonLoop();
  display.setFont(liberationSans_10ptFontInfo);
  display.setCursor(0, 0);
  display.fontColor(BLUE, BLACK);
  display.print("x: ");
  display.print(CompassX);
  display.print(", y: ");
  display.print(CompassY);
  display.print(", z:");
  display.print(CompassZ);

  delay(1000);

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
}

void ButtonLoop()
{
  display.setCursor(0,0);
  if (display.getButtons(TSButtonUpperLeft)) {
    display.clearScreen();
    display.println("Pressed!");
    delay(1000);
    display.clearScreen();
  } else {
    display.println("          ");
  }
  display.setCursor(0, 54);
  if (display.getButtons(TSButtonLowerLeft)) {
    display.clearScreen();
    display.println("Pressed!");
    delay(1000);
    display.clearScreen();
  } else {
    display.println("          ");
  }
  display.setCursor(95 - display.getPrintWidth("Pressed!"), 0);
  if (display.getButtons(TSButtonUpperRight)) {
    display.clearScreen();
    display.println("Pressed!");
    delay(1000);
    display.clearScreen();
  } else {
    display.println("          ");
  }
  display.setCursor(95 - display.getPrintWidth("Pressed!"), 54);
  if (display.getButtons(TSButtonLowerRight)) {
    display.clearScreen();
    display.println("Pressed!");
    delay(1000);
    display.clearScreen();
  } else {
    display.println("          ");
  }
}
  

