#ifndef I2CMANAGER_H
#define I2CMANAGER_H

class I2CManager {
		
	private:		
		void i2cSend(char deviceID, char cmd);		

	public:
		void i2cSendToSlaves(char deviceIDs[], char senderID, char cmd);
		void scanDevices();
};


#endif