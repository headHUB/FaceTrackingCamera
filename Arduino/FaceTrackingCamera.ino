/****************************************************************************************************
            Face Tracking Camera Using OpenCV, C++ and Arduino
          Haar Feature-based Cascade Classifier for Face Detection
                              [ Arduino ]
*****************************************************************************************************/

#include <Servo.h>

Servo servoTilt, servoPan;

char tiltChannel = 1, panChannel = 2;
char serialChar = 0;

void setup() 
{
  servoTilt.attach(2);  //The Tilt servo is attached to pin 2.
  servoPan.attach(3);   //The Pan servo is attached to pin 3.
  
  //Initially put both the servos at 90 degree
  servoTilt.write(90);  
  servoPan.write(90);      

  //Set up serial communication
  Serial.begin(57600);  
}

void loop() 
{
  while(Serial.available() <=0);  
  serialChar = Serial.read();     
  if(serialChar == tiltChannel)       //Check if the first serial character is the servo ID for the tilt servo
  {
    while(Serial.available() <=0);    
    servoTilt.write(Serial.read());   //Set the tilt servo position to the value of the second command byte received on the serial port
  }
  else if(serialChar == panChannel)   //Check if the first serial character is the servo ID for the pan servo.
  {
    while(Serial.available() <= 0);   
    servoPan.write(Serial.read());    //Set the pan servo position to the value of the second command byte received from the serial port.
  }
  else if(serialChar == 0)            // Set the servo to normal position
  {
    servoTilt.write(90);  
    servoPan.write(90); 
  }
}

