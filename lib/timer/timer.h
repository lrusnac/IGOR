#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>
#include "config.h"

extern unsigned long lastActivityTime;
extern unsigned long countingStartTime;
extern unsigned long idleStartTime;

extern bool isCounting;
extern int elapsedMinutes;
extern int countdownValue;
extern int initialCountdownValue;
extern int countdownSeconds;
extern bool displayOff;

void updateActivity();
void handleInactivity(unsigned long currentMillis);
void handleCounting(unsigned long currentMillis);

#endif // TIMER_H