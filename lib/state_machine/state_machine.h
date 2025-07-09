#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include "config.h" // For State enum

extern AppState currentState;
extern int flowMinutes;
extern int menuIndex;
extern String menuOptions[3];

void initializeStateMachine();
void handleButtonPressStateMachine(); // Renamed from handleButtonPress in main.cpp
void handleRotaryInputStateMachine(); // Renamed from handleRotaryInput in main.cpp
void startCountingUp();
void startSelectingDownDuration();
void confirmCountdownSelection();
void stopCountingUp();
void stopCountingDown();
void resetFlowMinutes();

#endif // STATE_MACHINE_H