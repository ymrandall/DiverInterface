// include the library code:
#include <SoftwareSerial.h>

//Adjustable Parrameters:
//************************************************************
//time before start command stops the starter
int starttime=5000;

//Delay needed between messages min: 1100
int messagedelay=1100;

//set servo min and max 
//location max is 256 as it is 8 bit
int servomax=250;
//location min is 0 but does not read well below 100
int servomin=100;

//Changing steps changges the amount of steps between min and max on the servo
int steps=3;
//************************************************************

//makes steps between max and min for servo
int stepcount=(servomax-servomin)/steps;

// Start and stop relay pins
int stoprelay=19;
int startrelay=20;
//Sensor input pins
int fuelsensor=23;
int enginecheck=22;
//servo pin
int servopin=3;

//Define Variables used
int fuelread=0;
int fuelreadnew=0;
int enginestat=0;
int enginestatnew=0;
int servoposition=servomin;


String message;
char sendmessage[10];



void setup() {
   //setup start and stop button inputs
  pinMode(fuelsensor,INPUT);
  pinMode(enginecheck,INPUT);
  pinMode(stoprelay,OUTPUT);
  pinMode(startrelay,OUTPUT);
  pinMode(servopin,OUTPUT);
  pinMode(13,OUTPUT);
  //turn off engine when system is turned on
  digitalWrite(stoprelay,HIGH);
  
  //delay for controller to start and be ready to recieve messages
  delay(2000);
  
  //start communication with controller
  Serial2.begin(9600,SERIAL_8N1);
  
  //set location of servo to min value for lowest power
  analogWrite(servopin,servomin);
  
  //tell controller current state of topside
  sprintf(sendmessage,"power%d",int(100*(servoposition-servomin)/(servomax-servomin)));
  Serial2.println(sendmessage);
  delay(messagedelay);
  Serial2.println("off");
  }

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  // fuelreadnew=analogRead(fuelsensor);
  enginestatnew=digitalRead(enginecheck);
  //send updated data every second
  
//  if (fuelread!=fuelreadnew){
//    fuelread=fuelreadnew;
//    sprintf(sendmessage,"fuel%d",int(fuelread/2.56));
//    Serial.println(sendmessage);
//    delay(100);
//    }
//  if(enginestat!=enginestatnew){
//    enginestat=enginestatnew;
//    if (enginestat==1){
//      Serial2.println("on");
//      delay(100);}
//    if (enginestat==0){
//      Serial2.println("off");
//      delay(100);}}

///recieving messages from controller
  if (Serial2.available()){
    message=Serial2.readString();
    
///start message to start PWC engine
    if (message.startsWith("start")){
      digitalWrite(stoprelay,LOW);
      digitalWrite(startrelay,HIGH);
      delay(starttime);
      digitalWrite(startrelay,LOW);
      Serial2.println("on");
      enginestat=1;}

 //stop message to stop PWC engine
    else if (message.startsWith("stop")){
      digitalWrite(startrelay,LOW);
      digitalWrite(stoprelay,HIGH);
      servoposition=servomin;
      analogWrite(servopin,servoposition);
      sprintf(sendmessage,"power%d",int(100*(servoposition-servomin)/(servomax-servomin)));
      Serial2.println(sendmessage);
      delay(messagedelay);
      Serial2.println("off");
      enginestat=0;}
      
  //increase power to engine by pulling on throttle cable with servo
    else if (message.startsWith("inpower") && (servoposition<servomax) && (enginestat==1)){
      servoposition+=stepcount;
      analogWrite(servopin,servoposition);
      sprintf(sendmessage,"power%d",int(100*(servoposition-servomin)/(servomax-servomin)));
      Serial2.println(sendmessage);
      delay(messagedelay);}
      
    //decrease power to engine by pulling on throttle cable with servo
    else if (message.startsWith("decpower") && (servoposition>servomin) && (enginestat==1)){
      servoposition-=stepcount;
      analogWrite(servopin,servoposition);
      sprintf(sendmessage,"power%d",int(100*(servoposition-servomin)/(servomax-servomin)));
      Serial2.println(sendmessage);
      delay(messagedelay);
      }
}}
