#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

 /* wiring for LCD screen:
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD K pin to ground
 * LCD VDD pin to 5V
 * LCD A pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/
//LCD pins
int RS=2;
int Enable=3;
int D4=4;
int D5=5;
int D6=6;
int D7=7;

// Define button pins
int stopbutton=14;
int startbutton=13;
int inpowerbutton=16;
int decpowerbutton=15;

//Led pins
int greenpin=20;
int yellowpin=19;
int redpin=18;

//variables that are used
int sendstop=0;
int sendstart=0;
int sendinpower=0;
int senddecpower=0;

//Delay needed between messages min needed: 1100
int messagedelay=1100;

int dredgepower=0;
String message;
String prntmessage;


// initialize the LCD Screen with pins listed in header
LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //start communication with topside
  Serial2.begin(9600);
  //setup start and stop button inputs
  pinMode(stopbutton,INPUT);
  pinMode(startbutton,INPUT);
  pinMode(inpowerbutton,INPUT);
  pinMode(decpowerbutton,INPUT);
  pinMode(greenpin,OUTPUT);
  pinMode(yellowpin,OUTPUT);
  pinMode(redpin,OUTPUT);

  //Print labels for data on screen
   lcd.setCursor(0, 0);
   lcd.print("Status:");

   lcd.setCursor(0, 1);
   lcd.print("Power:");

   lcd.setCursor(11, 0);
   lcd.print("Fuel:");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):

  sendstop=digitalRead(stopbutton);
  sendstart=digitalRead(startbutton);
  sendinpower=digitalRead(inpowerbutton);
  senddecpower=digitalRead(decpowerbutton);

//what happens when buttons are pressed
  if (sendstop==HIGH){
    Serial2.println("stop");
    digitalWrite(redpin,HIGH);
    delay(messagedelay);
    digitalWrite(redpin,LOW);}
    
  if (sendstart==HIGH){
    Serial2.println("start");
    digitalWrite(greenpin,HIGH);
    delay(messagedelay);
    digitalWrite(greenpin,LOW);}
    
  if (sendinpower==HIGH){
    Serial2.println("inpower");
    digitalWrite(yellowpin,HIGH);
    delay(messagedelay);
    digitalWrite(yellowpin,LOW);}
    
  if (senddecpower==HIGH){
    Serial2.println("decpower");
    digitalWrite(yellowpin,HIGH);
    delay(messagedelay);
    digitalWrite(yellowpin,LOW);}
    
  if (Serial2.available()){  
    message=Serial2.readString();

    //uncomment to see serial2 message on screen
    //lcd.setCursor(0, 1);
    //lcd.print(message);
    if (message.startsWith("off")){
      lcd.setCursor(7, 0);
      lcd.print("off");
     }
      
    else if (message.startsWith("on")){
      lcd.setCursor(7, 0);
      lcd.print("on ");
      }
      
    else if (message.startsWith("power")){
      lcd.setCursor(6, 1);
      prntmessage=message.substring(5,(message.length()-2));
      lcd.print("   ");
      lcd.setCursor(6, 1);
      lcd.print(prntmessage);}
      
    else if (message.startsWith("fuel")){
      lcd.setCursor(13, 1);
      prntmessage=message.substring(4,(message.length()-2));
      lcd.print("  ");
      lcd.setCursor(13, 1);
      lcd.print(prntmessage);} 
   }}

