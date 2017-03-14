#include "Flora.h"
#include <Arduino.h>

//Flora Pixel Defines
#define AMOUNT_OF_FLORA_PIXELS 1


Flora::Flora(int ledPin) : _ledPin(ledPin) {
	pinMode(_ledPin, OUTPUT);
	_pixels = Adafruit_NeoPixel(AMOUNT_OF_FLORA_PIXELS, _ledPin, NEO_GRB + NEO_KHZ800);
	_pixels.begin();
 	_pixels.setPixelColor(0, _pixels.Color(0,0,0)); 	
}

void Flora::update() {
	pulsateFloraLED();
}

void Flora::pulsateFloraLED(){	
  _currentLEDBrightness = _currentLEDBrightness + (BrightnessStepSize * _pulsateDirection);  
  _pixels.setPixelColor(0, _pixels.Color(_currentLEDBrightness,0,0));
  _pixels.show();
  if(_currentLEDBrightness + BrightnessStepSize > FLORA_MAX_BRIGHTNESS || _currentLEDBrightness - BrightnessStepSize < 0) _pulsateDirection *= -1;
}