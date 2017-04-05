#define DEBUG
#ifndef _ZTimer_H_
#define _ZTimer_H_
#define LIBRARY_VERSION	0.0.1
static void nothing(void) {
}

class ZTimer {
public:
	typedef void (*ZvoidFuncPtr)(void);// Create a type to point to a funciton.
	ZTimer();
	bool CheckTime();
	ZTimer & SetCallBack(void (*CB)(void));
	ZTimer & StopTimer();
	ZTimer & StartTimer();
	ZTimer & ResetTimer(bool r = true);
	ZTimer & SetWaitTime(unsigned long Time);
	ZTimer & SetLastTime(unsigned long Time);
	ZTimer & Pause();
	ZTimer & Every();
	ZTimer & After();
	ZTimer & Millis();
	ZTimer & Micros();
	ZTimer & This();
	unsigned long GetNow();
	unsigned long WaitTime();
	unsigned long LastTime();


	volatile ZvoidFuncPtr _CB = nothing;
	unsigned long waitTime;
	bool          everyTime;
	bool          micro;
	bool          restart;
	bool          enableFlag;
	unsigned long lastTime;
	unsigned long remainingTime;
};
#endif