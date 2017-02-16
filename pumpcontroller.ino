/*
This is for the arduino genuino flavor of arduino, but I would think it should compile on any of the arduino variants. 
Make sure you copy the NewPing library into your Arduino library folder. Mine is at "C:\Users\%USERNAME%\Documents\Arduino\libraries", 
but yours may be different.

To turn on the motor, you have to throw the MOTOR_PIN output to low, which is counterintuitive, 
but that seems to be the way it works. 

I plan on adding more code to log levels, alarms and other stuff to an online database/interface 
soon. 

Also, the current version has tons of garbage in it that I've commented out. That will go away soon.
*/

#include <NewPing.h>
#define MOTOR_PIN 9
#define TRIGGER_PIN  11
#define ECHO_PIN     12
#define MAX_DISTANCE 500

unsigned int ping;
unsigned int distancecm;
unsigned int upperlimit=50;
unsigned int lowerlimit=100;
unsigned int alarmlimit=30;
unsigned int systemstate=0;
bool motorrunning=false;
//char* systemstates[]={"system idle ","motor running ","high alarm ","no reading alarm "};
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(MOTOR_PIN, OUTPUT);
digitalWrite(MOTOR_PIN,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
delay(50);


  ping=sonar.ping_median(20);
  distancecm=(sonar.convert_cm(ping));


if ((distancecm<alarmlimit)&&(distancecm!=0))
  {
    digitalWrite(MOTOR_PIN,LOW);
    Serial.print("Motor is running");
    Serial.print(",High Alarm");
    motorrunning=true;
    //systemstate=2;
    
  }
  else if ((distancecm<upperlimit)&&(distancecm!=0)&&(motorrunning==false))
  {
    digitalWrite(MOTOR_PIN,LOW);
    Serial.print("Motor is turning on");
    motorrunning=true;
    //systemstate=1;
  }
   else if ((distancecm<lowerlimit)&&(distancecm!=0)&&(motorrunning==true))
  {
    //digitalWrite(MOTOR_PIN,LOW);
    Serial.print("Motor is running-Normal");
    motorrunning=true;
    //systemstate=1;
  }
  else if ((distancecm>upperlimit)&&(distancecm!=0)&&(motorrunning==true))
  {
    digitalWrite(MOTOR_PIN,HIGH);
   Serial.print("Motor is turning off");
   motorrunning=false;
    //systemstate=0;
  }
  else if (ping==0) 
  {
    Serial.print("No reading!");
    digitalWrite(MOTOR_PIN,HIGH);
   Serial.print("Motor is not running");
   motorrunning=false;
    //systemstate=3;
  }
 else if ((distancecm>upperlimit)&&(distancecm!=0)&&(motorrunning==false))
  {
    //digitalWrite(MOTOR_PIN,LOW);
    Serial.print("IDLE-Motor is not running");
    //systemstate=1;
  }
  else
  {
    Serial.print("Nothing to see here.");
    }
 // Serial.print(systemstates[systemstate]);
  /*Serial.print(",motor is ");
  if (motorrunning==true)
  {
    Serial.print("running.");
  }
 else if (motorrunning==false) 
  {
    Serial.print("not running.");
  }
 else
 {
  Serial.print("No idea");
 }
  */
  //Serial.print(motorrunning);
  //if ((sonar.convert_cm(ping)<alarmlimit)&&(sonar.convert_cm(ping)!=0)) Serial.print("alarm ");
  //if ((sonar.convert_cm(ping)<upperlimit)&&(sonar.convert_cm(ping)!=0)) Serial.print("motor stop ");
  //if ((sonar.convert_cm(ping)>lowerlimit)&&(sonar.convert_cm(ping)!=0)) Serial.print("motor start ");
  //if (sonar.convert_cm(ping)==0) Serial.print("sensor error ");
  Serial.print(",Ping: ");
  Serial.print(sonar.convert_cm(ping));
  Serial.println("cm");
 // digitalWrite(MOTOR_PIN,LOW);

}



