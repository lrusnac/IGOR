#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include "config.h"

extern AppState currentState;
extern int flowMinutes;
extern MenuOption menuIndex;
extern String menuOptions[3];

void initializeStateMachine();
void handleButtonPressStateMachine();
void handleRotaryInputStateMachine();
void startCountingUp();
void startSelectingDownDuration();
void confirmCountdownSelection();
void stopCountingUp();
void stopCountingDown();
void resetFlowMinutes();

#endif // STATE_MACHINE_H