#include <Servo.h> 
//#include "Random.h"

#define PIR_PIN 7
#define IR_PIN A0
#define SERVO_FEET_PIN 3
#define SERVO_HEAD_PIN 6
#define SERVO_CALIBRATE_PIN 11
#define BAUD_RATE 9600

#define SLEEP_TIMEOUT 8

#define HEAD_SLEEP_ANGLE 60
#define FEET_SLEEP_ANGLE 90

#define FEET_WAKEUP_ANGLE 45
#define HEAD_WAKEUP_ANGLE 45

#define FEET_LOOKAROUND_ANGLE 20
#define HEAD_LOOKAROUND_ANGLE 20

Servo servoFeet;
Servo servoHead;
Servo servoCalibration;

bool isAwake = true;
unsigned long lastAwake = 0;

enum IRProximity {
  CloseProximity = 6,
  MediumProximity = 10,
  FarProximity = 20
};

void setupPins(){
  pinMode(PIR_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(SERVO_FEET_PIN, OUTPUT);
  pinMode(SERVO_HEAD_PIN, OUTPUT);
  pinMode(SERVO_CALIBRATE_PIN, OUTPUT);
}

void setup() {
  Serial.begin(BAUD_RATE);
  setupPins();
  servoFeet.attach(SERVO_FEET_PIN);
  servoHead.attach(SERVO_HEAD_PIN);
  calibrate();
  gotoSleep();
}

void calibrate(){  
  servoCalibration.attach(SERVO_CALIBRATE_PIN);
  servoCalibration.write(180);
}

int randomDirection(){
  return random(2) == 0 ? -1 : 1;
}

bool randomBool(){
  return random(2) == 0 ? true : false;
}

int calculate_distance(int readVal) {
  //float volts = (float)readVal * 0.0048828125;
  int distance = (2914 / (readVal + 5)) - 1;
  return distance;
}

void moveServo(Servo servo, int from, int to, int step = 1, int delayTime = 15){
  int position;

  for(position = from; position <= to; position += step){
    setServoPosition(servo, position);
    delay(delayTime);
  }
}

void setServoPosition(Servo servo, int position){
  servo.write(position);
}

void gotoSleep(){
  if(!isAwake){ return; }
  setServoPosition(servoFeet, FEET_SLEEP_ANGLE);
  setServoPosition(servoHead, HEAD_SLEEP_ANGLE);
  isAwake = false;
}

void wakeUp(){
  lastAwake = millis();  
  if(isAwake){ return; }

  int direction = randomDirection();
  setServoPosition(servoFeet, servoFeet.read() + FEET_WAKEUP_ANGLE * direction);
  setServoPosition(servoHead, servoHead.read() + HEAD_WAKEUP_ANGLE);
  isAwake = true;
}

void lookAround(){
  int originalFeetPosition = servoFeet.read();
  int originalHeadPosition = servoHead.read();

  setServoPosition(servoFeet, originalFeetPosition - random(FEET_LOOKAROUND_ANGLE));  
  delay(750);
  setServoPosition(servoFeet, originalFeetPosition + random(FEET_LOOKAROUND_ANGLE));
  delay(1000);
  setServoPosition(servoFeet, originalFeetPosition);
  delay(350);
  setServoPosition(servoHead, servoHead.read() + HEAD_LOOKAROUND_ANGLE);
  delay(1500);
  setServoPosition(servoHead, originalHeadPosition);
}

void shakeHead(int speedPercentage = 100) {
  int originalFeetPosition = servoFeet.read();
  setServoPosition(servoFeet, originalFeetPosition - 10);
  delay(100);
  setServoPosition(servoFeet, originalFeetPosition + 20);
  delay(100);
  setServoPosition(servoFeet, originalFeetPosition);
  delay(100);
}

bool shouldBackOff(){
  int proximity = calculate_distance(analogRead(IR_PIN));
  bool objectIsClose = proximity < CloseProximity;
  return objectIsClose;
}

int secondsSinceLastAwakening(){
  if(lastAwake == 0) return -1;
  return (millis() - lastAwake) / 1000;
}

void loop() {  
  int proximity = calculate_distance(analogRead(IR_PIN));
  Serial.println(proximity);
  
  if(proximity < MediumProximity && !isAwake){
    wakeUp();
    delay(1500);
    lookAround();    
  }

  if(secondsSinceLastAwakening() >= SLEEP_TIMEOUT) gotoSleep();

  delay(100);
}
