// Dog treat dispenser code
// HDT
// This code drives a servo to move when a button is pushed.

#include <Servo.h> 
#include "LowPower.h"

#define   SERVO_DELAY   500
#define   LED_DELAY     200
 
Servo myservo;      // create servo object to control a servo 
 
int servo_pin = 6;  // servo connection
int button_pin = 2; // input button
int lbo_pin = 4;    // low battery
int led_pin = 13;   // status led

int servo_lower_limit = 10;  // in degrees
int servo_upper_limit = 170; // in degrees
int servo_dir = 1;           // 1 = forward, 0 = back
int dispense = 1;
 
void setup() {
  // initialize pins
  pinMode(button_pin, INPUT_PULLUP); // set button to input with internal pullup resistor
  pinMode(lbo_pin, INPUT_PULLUP);    // set lbo to input with internal pullup resistor
  pinMode(led_pin, OUTPUT);          // led output status
}

void wake() {
}

void flash_led(int num) {
  // flash led specified number of times
  for(int i = 0; i < num; i++) {
    digitalWrite(led_pin, HIGH);
    delay(LED_DELAY);
    digitalWrite(led_pin, LOW);
    delay(LED_DELAY);
  }
}
 
void loop() {
  // attach interrupt, sleep for maximum duration
  attachInterrupt(digitalPinToInterrupt(button_pin), wake, FALLING);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(digitalPinToInterrupt(button_pin)); 

  // check battery status, don't actuate if lbo is low
  if(digitalRead(lbo_pin) == LOW) {
    // update led status
    flash_led(2);
  }
  // otherwise dispense treat!
  else {
    // dispense
    myservo.attach(servo_pin);  
    
    if(servo_dir ==1) {
      myservo.write(servo_lower_limit);            // tell servo to go to lower limit
      servo_dir = 0;                               // switch direction for next loop
    }
    else {   
      myservo.write(servo_upper_limit);            // tell servo to go to upper limit
      servo_dir = 1;                               // switch direction for next loop
    }
    
    delay(SERVO_DELAY);                            // wait for the servo to reach the position 
    myservo.detach();                              // detach turns off the servo so it doesn't keep pulling current
  
    // turn off led
    flash_led(1);
  }
} 
