# pumpcontroller
Pump controller for the septic pump system

This is for the arduino genuino flavor of arduino, but I would think it should compile on any of the arduino variants. 
Make sure you copy the NewPing library into your Arduino library folder. Mine is at "C:\Users\%USERNAME%\Documents\Arduino\libraries", 
but yours may be different.

To turn on the motor, you have to throw the MOTOR_PIN output to low, which is counterintuitive, 
but that seems to be the way it works. 

I plan on adding more code to log levels, alarms and other stuff to an online database/interface 
soon. 

Also, the current version has tons of garbage in it that I've commented out. That will go away soon.
