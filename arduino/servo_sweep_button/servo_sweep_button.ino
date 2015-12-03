// Dog treat dispenser code
// HDT
// This code drives a servo to move when a button is pushed.

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
 
int servo_pin = 10;
int button_pin = 11; //button is connected to pin 11 and ground

int delay_full_rotation = 500;
int delay_between_sweeps = 50; // delay in milliseconds
int servo_lower_limit = 10; //in degrees
int servo_upper_limit = 170; //in degrees
int servo_dir = 1;   //1=forward, 2=back
 
void setup() 
{ 
  pinMode(button_pin, INPUT_PULLUP); // set button to input with internal pullup resistor
  Serial.begin(9600);

} 
 
 
void loop() 
{ 
 
    int button_state = !digitalRead(button_pin);
//    int button_state = digitalRead(button_pin);

    
    if (button_state == HIGH) {
      if (servo_dir ==1){
         
        myservo.attach(servo_pin);  
        myservo.write(servo_lower_limit);              // tell servo to go to lower limit
        delay(delay_full_rotation);                    // waits for the servo to reach the position 
        myservo.detach();                              // detach turns off the servo so it doesn't keep pulling current
//        delay(delay_between_sweeps-delay_full_rotation);
        servo_dir = 2;                                 // switch direction for next loop
      }
      else {   
        myservo.attach(servo_pin);                
        myservo.write(servo_upper_limit);              // tell servo to go to upper limit
        delay(delay_full_rotation);                    // waits for the servo to reach the position 
        myservo.detach();                              // detach turns off the servo so it doesn't keep pulling current
//        delay(delay_between_sweeps-delay_full_rotation);
        servo_dir = 1;                                 // switch direction for next loop
      }
      Serial.print(2);                                 // print for debugging only
      Serial.print("\t");                              // print for debugging only
    }
    Serial.println(button_state);                      // print for debugging only
    
    delay(delay_between_sweeps);
} 
