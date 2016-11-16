// Dog treat dispenser code
// HDT
// This code drives a servo to move when a button is pushed.

#include <EEPROM.h>
#include <Servo.h> 
#include "LowPower.h"

#define   USE_FEEDBACK

#define   SERVO_DELAY   2000
#define   SERVO_MAX     170   // in degrees
#define   SERVO_MIN     10    // in degrees
#define   LED_DELAY     200
#define   CAL_DELAY     5000
#define   SAMPLE_RATE   100


#ifdef USE_FEEDBACK
#define   JIGGLE_DEG    10    // in degrees
#define   JIGGLE_TRIES  5
#define   JIGGLE_DELAY  200
#define   JIGGLE_THRES  5

#define   FB_PIN        A0    // servo feedback

#define   EEPROM_START  0
#define   SLOPE_ADDR    EEPROM_START
#define   OFFSET_ADDR   EEPROM_START + sizeof(float)
#endif

#define   SERVO_PIN     6     // servo command
#define   BUTTON_PIN    2     // input button
#define   LBO_PIN       4     // low battery
#define   LED_PIN       13    // status led

Servo myservo;                // create servo object to control a servo 
int servo_dir;                // 1 = forward, 0 = back
unsigned long ticks;          // for button timing

#ifdef USE_FEEDBACK
float fb_slope, fb_offset;
#endif

/***************************************************************************
 * setup
 **************************************************************************/
void setup() {
  Serial.begin(9600);
  
  // initialize pins
  pinMode(BUTTON_PIN, INPUT_PULLUP); // set button to input with internal pullup resistor
  pinMode(LBO_PIN, INPUT_PULLUP);    // set lbo to input with internal pullup resistor
  pinMode(LED_PIN, OUTPUT);          // led output status
  
  // initialize variables
  ticks = 0;
  servo_dir = 0;
  
#ifdef USE_FEEDBACK
  // get slope and offset from eeprom
  EEPROM.get(SLOPE_ADDR, fb_slope);
  EEPROM.get(OFFSET_ADDR, fb_offset);

  // get initial position
  delay(SAMPLE_RATE);
  float fb_pos = convert_fb(analogRead(FB_PIN));

  // set dir to forward if closer to min position
  if(fabs((float)SERVO_MAX - fb_pos) > fabs((float)SERVO_MIN - fb_pos)) {
    servo_dir = 1;
  }
#endif

  Serial.println("treat dispenser ready");
}

/***************************************************************************
 * wake
 **************************************************************************/
void wake() {
  ticks = 0;
}

#ifdef USE_FEEDBACK
/***************************************************************************
 * convert analog servo feedback
 **************************************************************************/
float convert_fb(int val) {
  return (float)val*fb_slope + fb_offset;
}

/***************************************************************************
 * calibrate servo feedback
 **************************************************************************/
void calibrate(void) {
  int ee_address = EEPROM_START;

  // start calibration flash led
  flash_led(3);

  myservo.attach(SERVO_PIN);  

  // send to min position
  myservo.write(SERVO_MIN);
  delay(SERVO_DELAY);
  int fb_min = analogRead(FB_PIN);

  // send to max position
  myservo.write(SERVO_MAX);
  delay(SERVO_DELAY);
  int fb_max = analogRead(FB_PIN);

  myservo.detach(); 

  Serial.print("fb_min = ");
  Serial.println(fb_min);
  Serial.print("fb_max = ");
  Serial.println(fb_max);

  // calculate slope and offset
  fb_slope = (float)(SERVO_MAX - SERVO_MIN)/(float)(fb_max - fb_min);
  fb_offset = (float)SERVO_MAX - fb_slope*(float)fb_max;

  Serial.print("slope = ");
  Serial.println(fb_slope);
  Serial.print("offset = ");
  Serial.println(fb_offset);

  // write to eeprom
  EEPROM.put(SLOPE_ADDR, fb_slope);
  EEPROM.put(OFFSET_ADDR, fb_offset);
  
  // set servo direction for next spin
  servo_dir = 0;
  
  flash_led(4);
}

/***************************************************************************
 * jiggle to unclog dispenser
 **************************************************************************/
void jiggle(int cmd, int fb, int dir) { 
  // get jiggle position
  int j_pos;
  if(servo_dir == 1) j_pos = fb - JIGGLE_DEG;
  else j_pos = fb + JIGGLE_DEG;

  Serial.print("j_pos = ");
  Serial.println(j_pos);

  myservo.attach(SERVO_PIN); 

  // send to commanded pos
  myservo.write(j_pos); 
  delay(JIGGLE_DELAY);                    // wait for the servo to reach the position

  // send to commanded pos
  myservo.write(cmd); 
  delay(SERVO_DELAY);                    // wait for the servo to reach the position 

  myservo.detach();
}
#endif

/***************************************************************************
 * flash status led
 **************************************************************************/
void flash_led(int num) {
  // flash led specified number of times
  for(int i = 0; i < num; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(LED_DELAY);
    digitalWrite(LED_PIN, LOW);
    delay(LED_DELAY);
  }
}

/***************************************************************************
 * dispense treat
 **************************************************************************/
void dispense(void) {
    
    // get commanded pos
    int cmd_pos;
    for (int p=0; p < 1000; p++) {  
    if(servo_dir == 1) cmd_pos = SERVO_MAX;
    else cmd_pos = SERVO_MIN;

    Serial.print("cmd_pos = ");
    Serial.println(cmd_pos);

    myservo.attach(SERVO_PIN);  

    // send to commanded pos
    myservo.write(cmd_pos); 
    delay(SERVO_DELAY);                    // wait for the servo to reach the position 
    myservo.detach();                      // detach turns off the servo so it doesn't keep pulling current



    servo_dir = 1 - servo_dir;              // switch servo direction for next loop

    }
    // turn off led
    flash_led(1);
}




/***************************************************************************
 * main loop
 **************************************************************************/
void loop() {
  // attach interrupt, sleep for maximum duration
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wake, FALLING);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));

  // wait for button up
  while(digitalRead(BUTTON_PIN) == LOW) {
    ticks += SAMPLE_RATE;
    delay(SAMPLE_RATE);
  }
  
  Serial.print("ticks = ");
  Serial.println(ticks);
  
  // check battery status, don't actuate if lbo is low
  if(digitalRead(LBO_PIN) == LOW) {
    // update led status
    flash_led(2);
  }
  // otherwise dispense treat!
  else {
#ifdef USE_FEEDBACK
    if(ticks > CAL_DELAY) calibrate();
    else dispense();
#else
    dispense();
#endif
  }
}

