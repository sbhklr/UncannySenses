#include "musical_notes.h"
#include <Arduino.h>
#include "SoundEffects.h"

void beep (int speakerPin, float noteFrequency, long noteDuration) {      
	// Convert the frequency to microseconds
	float microsecondsPerWave = 1000000/noteFrequency;

	// Calculate how many HIGH/LOW cycles there are per millisecond
	float millisecondsPerCycle = 1000/(microsecondsPerWave * 2);

	// Multiply noteDuration * number or cycles per millisecond
	float loopTime = noteDuration * millisecondsPerCycle;

	// Play the note for the calculated loopTime.
	for (int x=0; x<loopTime; ++x) {   
		digitalWrite(speakerPin,HIGH); 
		delayMicroseconds(microsecondsPerWave); 
		digitalWrite(speakerPin,LOW); 
		delayMicroseconds(microsecondsPerWave); 
	} 
}   

void ohhhSound(int speakerPin) {
  for (int i=1000; i<2000; i=i*1.02) {
    beep(speakerPin,i,8);
  }
  for (int i=2000; i>1000; i=i*.98) {
    beep(speakerPin,i,8);
  }
}

void ohnoSound(int speakerPin) {
  for (int i=1500; i>400; i=i*.98) {
    beep(speakerPin,i,10);
  }
}

void squeakSound(int speakerPin) {  
  for (int i=100; i<6000; i=i*1.5) {
    beep(speakerPin,i,20);
  }
}