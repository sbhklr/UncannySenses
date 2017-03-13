#ifndef FLORA_H
#define FLORA_H

#include "Adafruit_NeoPixel.h"

class Flora {	
 public:
 	static const int BrightnessStepSize = 10; 	
 	void update();
 	Flora(int ledPin);

 private:
 	int _ledPin;
 	int _currentLEDBrightness = 0;
	int _pulsateDirection = 1;
	Adafruit_NeoPixel _pixels;	
	void pulsateFloraLED();
};

#endif