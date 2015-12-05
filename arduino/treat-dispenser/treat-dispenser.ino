// Dog treat dispenser code
// HDT
// This code drives a servo to move when a button is pushed.

#include <Servo.h> 
#include "LowPower.h"
 
Servo myservo;  // create servo object to control a servo 
 
int servo_pin = 6;
int button_pin = 2; //button is connected to pin 11 and ground

int delay_full_rotation = 500;
int delay_between_sweeps = 50; // delay in milliseconds
int servo_lower_limit = 10; //in degrees
int servo_upper_limit = 170; //in degrees
int servo_dir = 1;   //1 = forward, 0 = back
int dispense = 1;
 
void setup() {
  //Serial.begin(9600);
  
  pinMode(button_pin, INPUT_PULLUP); // set button to input with internal pullup resistor
}

void wake() {
}
 
void loop() {
  // attach interrupt, sleep for maximum duration
  attachInterrupt(digitalPinToInterrupt(button_pin), wake, FALLING);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(digitalPinToInterrupt(button_pin)); 

  // check battery status
  
  
  // dispense
  myservo.attach(servo_pin);  
  
  if(servo_dir ==1) {
    myservo.write(servo_lower_limit);              // tell servo to go to lower limit
    servo_dir = 0;                                 // switch direction for next loop
  }
  else {   
    myservo.write(servo_upper_limit);              // tell servo to go to upper limit
    servo_dir = 1;                                 // switch direction for next loop
  }
  
  delay(delay_full_rotation);                    // waits for the servo to reach the position 
  myservo.detach();                              // detach turns off the servo so it doesn't keep pulling current


} 
