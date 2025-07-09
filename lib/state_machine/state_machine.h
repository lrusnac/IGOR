#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include "config.h"

extern AppState currentState;
extern MenuOption menuIndex;
extern int flowMinutes;
extern bool displayOff;
extern String menuOptions[3];

extern unsigned long lastActivityTime;
extern unsigned long countingStartTime;
extern unsigned long idleStartTime;

extern bool isCounting;
extern int elapsedMinutes;
extern int countdownValue;
extern int initialCountdownValue;
extern int countdownSeconds;

void handleInactivity();
void handleCounting();
void handleButtonPressStateMachine();
void handleRotaryInputStateMachine();

#endif // STATE_MACHINE_H
