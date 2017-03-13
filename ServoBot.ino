#include <Servo.h> 
#include <Wire.h>
#include "SoundEffects.h"
#include "Adafruit_NeoPixel.h"
#include "I2CManager.h"

#define BAUD_RATE 9600

//Flora Pixel Defines
#define AMOUNT_OF_FLORA_PIXELS 1
#define FLORA_MAX_BRIGHTNESS 255
#define FLORA_BRIGHTNESS_STEP_SIZE 10

#define FLORA_LED_PIN 8
#define PIR_PIN 7
#define IR_PIN A0
#define SERVO_FEET_PIN 5
#define SERVO_HEAD_PIN 6
#define SERVO_CALIBRATE_PIN 11
#define BUZZER_PIN 2

#define USE_I2C true
#define I2C_ID_MASTER 2
#define I2C_ID_SLAVE1 3
#define I2C_ID_SLAVE2 4
#define I2C_IS_MASTER true

#define I2C_WAKEUP 'w'
#define I2C_SLEEP 's'
#define I2C_ATTENTION 'a'

#define SLEEP_TIMEOUT 8

#define HEAD_LOOKUP_ANGLE 150
#define HEAD_SLEEP_ANGLE 60
#define FEET_SLEEP_ANGLE 90
#define FEET_WAKEUP_ANGLE 45
#define HEAD_WAKEUP_ANGLE 45
#define FEET_LOOKAROUND_ANGLE 20
#define HEAD_LOOKAROUND_ANGLE 20

char i2cDeviceIDs[] = {I2C_ID_MASTER, I2C_ID_SLAVE2};
Servo servoFeet;
Servo servoHead;
Servo servoCalibration;
char currentI2CCommand = 0;

I2CManager i2cManager = I2CManager();
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(AMOUNT_OF_FLORA_PIXELS, FLORA_LED_PIN, NEO_GRB + NEO_KHZ800);

bool isAwake = true;
unsigned long lastAwake = 0;
int currentLEDBrightness = 0;
int pulsateDirection = 1;

enum IRProximity {
  CloseProximity = 4,
  MediumProximity = 6,
  FarProximity = 20
};

void setupPins(){
  pinMode(FLORA_LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(SERVO_FEET_PIN, OUTPUT);
  pinMode(SERVO_HEAD_PIN, OUTPUT);
  pinMode(SERVO_CALIBRATE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void setup() {
  Serial.begin(BAUD_RATE);
  setupPins();

  #if USE_I2C
  Wire.begin(I2C_ID_MASTER);
  Wire.onReceive(onReceiveEvent);
  #endif

  #if ! I2C_IS_MASTER
  Wire.onRequest(onRequestEvent);
  #endif

  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(0,0,0));
  servoFeet.attach(SERVO_FEET_PIN);
  servoHead.attach(SERVO_HEAD_PIN);
  calibrate();
  gotoSleep();
}

void calibrate(){  
  servoCalibration.attach(SERVO_CALIBRATE_PIN);
  servoCalibration.write(180);
}

bool senderIsValid(char senderID) {
  if(senderID <= 0) return false;
  
  for(char device : i2cDeviceIDs){
    if(device == senderID) return true;
  }

  return false;
}

bool commandIsValid(char command){
  return command > 0;
}

void processI2CCommand(char senderID, char cmd){
  if(cmd == 0) return;

  if(cmd == I2C_WAKEUP){
    wakeUp();
  } else if(cmd == I2C_SLEEP){
    gotoSleep();
  } else if(cmd == I2C_ATTENTION){
    //TODO
  }
}

char pollI2CCommand(){
  char cmd = currentI2CCommand;
  currentI2CCommand = 0;
  return cmd;
}

void onRequestEvent() {
  char cmd = pollI2CCommand();

  if(cmd != 0){
    Wire.write(cmd);
  }
}

void onReceiveEvent(int howMany) {
  
  Serial.println("Received I2C data.");
  
  for(int i=0; i < howMany; ++i){
      if(!Wire.available()) break;
      char sender = Wire.read();                
      char cmd = Wire.read(); 
      processI2CCommand(sender, cmd);
  }
}

void i2cMasterLoop(){
  
  i2cManager.i2cSendToSlaves(i2cDeviceIDs, I2C_ID_MASTER, pollI2CCommand());

  for(char deviceIDRequest : i2cDeviceIDs){
    
    if(deviceIDRequest == I2C_ID_MASTER) continue;

    int bytesReceived = Wire.requestFrom(deviceIDRequest, 2);    
    
    while(bytesReceived == 2 && Wire.available()){ 
      char sender = Wire.read();      
      char cmd = Wire.read();
      if(!senderIsValid(sender) || !commandIsValid(cmd)) continue;
                                    
      i2cManager.i2cSendToSlaves(i2cDeviceIDs, sender, cmd);
      processI2CCommand(sender, cmd);      
    }

  }
}

void pulsateFloraLED(){
  currentLEDBrightness = currentLEDBrightness + (FLORA_BRIGHTNESS_STEP_SIZE * pulsateDirection);  
  pixels.setPixelColor(0, pixels.Color(currentLEDBrightness,0,0));
  pixels.show();
  if(currentLEDBrightness + FLORA_BRIGHTNESS_STEP_SIZE > FLORA_MAX_BRIGHTNESS || currentLEDBrightness - FLORA_BRIGHTNESS_STEP_SIZE < 0) pulsateDirection *= -1;
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

void shakeHead() {
  int originalFeetPosition = servoFeet.read();
  setServoPosition(servoFeet, originalFeetPosition - 10);
  delay(100);
  setServoPosition(servoFeet, originalFeetPosition + 20);
  delay(100);
  setServoPosition(servoFeet, originalFeetPosition);
  delay(100);
  ohhhSound(BUZZER_PIN);
}

void lookUp(){
  setServoPosition(servoHead, HEAD_LOOKUP_ANGLE);
}

bool shouldBackOff(){
  int proximity = calculate_distance(analogRead(IR_PIN));
  bool objectIsClose = proximity < CloseProximity;
  return objectIsClose;
}

void gotoSleep(){
  if(!isAwake){ return; }
  Serial.println("Go to sleep.");
  setServoPosition(servoFeet, FEET_SLEEP_ANGLE);
  setServoPosition(servoHead, HEAD_SLEEP_ANGLE);
  isAwake = false;
  //ohnoSound(BUZZER_PIN);
}

void wakeUp(){
  lastAwake = millis();  
  if(isAwake){ return; }
  Serial.println("Wake up.");  

  int direction = randomDirection();
  setServoPosition(servoFeet, servoFeet.read() + FEET_WAKEUP_ANGLE * direction);
  setServoPosition(servoHead, servoHead.read() + HEAD_WAKEUP_ANGLE);
  isAwake = true;
  squeakSound(BUZZER_PIN);
}

void lookAround(){
  Serial.println("Look around.");
  int originalFeetPosition = servoFeet.read();
  int originalHeadPosition = servoHead.read();

  setServoPosition(servoFeet, originalFeetPosition - random(FEET_LOOKAROUND_ANGLE));
  if(shouldBackOff()){shakeHead(); return;}  
  delay(750);
  setServoPosition(servoFeet, originalFeetPosition + random(FEET_LOOKAROUND_ANGLE));
  if(shouldBackOff()){shakeHead(); return;}  
  delay(1000);
  setServoPosition(servoFeet, originalFeetPosition);
  if(shouldBackOff()){shakeHead(); return;}  
  delay(350);
  setServoPosition(servoHead, servoHead.read() + HEAD_LOOKAROUND_ANGLE);
  if(shouldBackOff()){shakeHead(); return;}  
  delay(1500);
  setServoPosition(servoHead, originalHeadPosition);
}

int secondsSinceLastAwakening(){
  if(lastAwake == 0) return -1;
  return (millis() - lastAwake) / 1000;
}

void loop() {  
  pulsateFloraLED();

  int proximity = calculate_distance(analogRead(IR_PIN));
  //Serial.println(proximity);
  
  if(proximity < MediumProximity && !isAwake){
    Serial.println("Obstacle detected.");
    currentI2CCommand = I2C_WAKEUP;
    wakeUp();
    delay(1500);
    lookAround();    
  }

  if(shouldBackOff()){shakeHead(); return;}  

  if(secondsSinceLastAwakening() >= SLEEP_TIMEOUT) gotoSleep();

  #if I2C_IS_MASTER && USE_I2C
  i2cMasterLoop();
  #endif
  delay(50);
}