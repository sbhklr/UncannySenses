/**********************************************************************************************
* ZTimer Library - Version 0.0.1 
* by ZHomeSlice http://forum.arduino.cc/index.php?action=profile;u=258357
*
* This Library is licensed under a GPLv3 License
**********************************************************************************************/

#include <Arduino.h>
#include "ZTimer.h"

ZTimer::ZTimer() {
}
bool ZTimer::CheckTime() { //Delay time expired function "CheckTime()"
  unsigned long Now = GetNow();
  if (enableFlag && (((unsigned long) Now - lastTime) >= (waitTime))) { // Test Timer
    if (restart) lastTime = (everyTime) ? (lastTime + waitTime) : Now; //get ready for the next iteration Every or (After) Has Different affects
    if (ZTimer::_CB) {
      ZTimer::_CB(); // call the function we assigned to the once empty function pointer
    }
    return true; // Timer Finished
  }
  return false; // Still Waiting
} //END of CheckTime()
ZTimer & ZTimer::SetCallBack(void (*CB)(void)){
  ZTimer::_CB = CB;
  return *this;
}

ZTimer & ZTimer::StopTimer() {
  remainingTime = GetNow() - lastTime;
  enableFlag = false;
  return *this;
}

ZTimer & ZTimer::StartTimer() {
  lastTime = GetNow() + remainingTime;
  enableFlag = true;
  return *this;
}

ZTimer & ZTimer::ResetTimer(bool r ) {
  lastTime = GetNow();
  restart = r;
  enableFlag = true;
  return *this;
}
ZTimer & ZTimer::SetWaitTime(unsigned long Time){
  waitTime = Time;     // Change the timers delay time
  return *this; 
}

ZTimer & ZTimer::SetLastTime(unsigned long Time){
  lastTime  = Time;    // Change the timers last time 
  return *this; 
}

ZTimer & ZTimer::Pause() {
  restart = false ;    // Pauses Timer at end of timing WARNING CheckTimer() returns true at end of timing event and wont reset to false
  return *this; 
}
ZTimer & ZTimer::Every()  {
  everyTime = true ;   // By adding waitMillis to lastMillis timing will always shift by (waitMillis) 
  return *this; 
}
ZTimer & ZTimer::After()  {
  everyTime = false;   // By shifting to millis() the next time interval will always be (waitMillis) long
  return *this; 
}
ZTimer & ZTimer::Micros() {
  micro = true ;    // Pauses Timer at end of timing WARNING CheckTimer() returns true at end of timing event and wont reset to false
  lastTime = GetNow();
  return *this; 
}
ZTimer & ZTimer::Millis() {
  micro = false ;    // Pauses Timer at end of timing WARNING CheckTimer() returns true at end of timing event and wont reset to false
  lastTime = GetNow();
  return *this; 
}

ZTimer & ZTimer::This() {
  return *this;                     // Get the name of the Structure for Direct Access This_S_TIME(ID).lastMillis for example
} 

unsigned long ZTimer::GetNow() {
  return ((micro) ? micros() : millis());
}
unsigned long ZTimer::WaitTime(){ 
  return(waitTime);            // Access the timers delay time
}
unsigned long ZTimer::LastTime(){
  return(lastTime );           // Access the timers last time 
}
