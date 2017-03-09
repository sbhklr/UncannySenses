#include <Servo.h> 

#define PIR_PIN 7
#define IR_PIN A0
#define SERVO_FEET_PIN 3
#define SERVO_HEAD_PIN 9
#define SERVO_CALIBRATE_PIN 11
#define BAUD_RATE 9600

#define SLEEP_TIMEOUT 10
#define HEAD_LOOKUP_ANGLE 75
#define FEED_WAKEUP_TARGET_ANGLE 90
#define HEAD_WAKEUP_TARGET_ANGLE 180 - HEAD_LOOKUP_ANGLE

Servo servoFeet;
Servo servoHead;
Servo servoCalibration;

bool isAwake = true;
unsigned long lastAwake = 0;

int currentHeadPosition = 0;
int currentFeetPosition = 0;

enum Proximity {
  Close = 4,
  Medium = 10,
  Far = 20
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
  if(&servo == &servoHead) currentHeadPosition = position;
  else if(&servo == &servoFeet) currentFeetPosition = position;
}

void gotoSleep(){
  if(!isAwake){ return; }
  setServoPosition(servoFeet,0);
  setServoPosition(servoHead,180);
  isAwake = false;
}

void wakeUp(){
  if(isAwake){ return; }
  setServoPosition(servoFeet, FEED_WAKEUP_TARGET_ANGLE);
  setServoPosition(servoHead, HEAD_WAKEUP_TARGET_ANGLE);
  isAwake = true;
  lastAwake = millis();
}

void lookAround(){
  setServoPosition(servoHead,180 - 45);
}

int secondsSinceLastAwakening(){
  if(lastAwake == 0) return -1;
  return (millis() - lastAwake) / 1000;
}

void loop() {
  unsigned long timestamp = millis();
  int proximity = calculate_distance(analogRead(IR_PIN));
  Serial.println(proximity);
  
  if(proximity < Close){
    wakeUp();
  }

  if(secondsSinceLastAwakening() >= SLEEP_TIMEOUT) gotoSleep();

  delay(100);
}
