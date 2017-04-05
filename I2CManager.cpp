#include "I2CManager.h"
#include <Arduino.h>
#include <Wire.h>

#define I2C_ID_MASTER 2

void I2CManager::i2cSendToSlaves(char deviceIDs[], char senderID, char cmd){
  if(cmd == 0) return;

  for(unsigned int j = 0; j < sizeof(deviceIDs) / sizeof(char); ++j){
       
      char receiverID = deviceIDs[j];

      if(receiverID == senderID || receiverID == I2C_ID_MASTER) continue;

      Serial.print("Sending cmd ");
      Serial.print(cmd);
      Serial.print(" from ");
      Serial.print(senderID, BIN);
      Serial.print(" to ");
      Serial.print(receiverID, BIN);
      Serial.println();

      i2cSend(receiverID, cmd);
    }    
}

void I2CManager::i2cSend(char deviceID, char cmd) {  
  if(cmd == 0) return;

  Wire.beginTransmission(deviceID);
  char data[] = {deviceID, cmd};
  Wire.write(data,sizeof(data));
  Wire.endTransmission();
}

void I2CManager::scanDevices(){
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}