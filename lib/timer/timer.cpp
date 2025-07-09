#include "timer.h"
#include "config.h"
#include "display.h"
#include "state_machine.h"

void updateActivity(unsigned long currentMillis) {
  lastActivityTime = currentMillis;
}

void handleInactivity(unsigned long currentMillis) {
  if ((currentState == AppState::MENU || currentState == AppState::SELECTING_DOWN_DURATION) &&
      (currentMillis - lastActivityTime > inactivityLimit)) {
    if (currentState != AppState::IDLE) {
      currentState = AppState::IDLE;
      idleStartTime = millis();
    }
  }

  if (currentState == AppState::IDLE && !displayOff && (currentMillis - idleStartTime > displayOffTimeLimit)) {
    displayOff = true;
  }
}

void handleCounting(unsigned long currentMillis) {
  if (!isCounting) return;

  unsigned long elapsedTime = currentMillis - countingStartTime;

  if (currentState == AppState::COUNTING_UP) {
    if (elapsedTime >= (unsigned long)(elapsedMinutes + 1) * 60000) {
      elapsedMinutes++;
    }
  }
  else if (currentState == AppState::COUNTING_DOWN) {
    if (elapsedTime >= (unsigned long)(initialCountdownValue * 60 - countdownSeconds + 1) * 1000) {
      countdownSeconds--;
      if (countdownSeconds % 60 == 0) {
        countdownValue--;
      }
      if (countdownValue <= 0 && countdownSeconds <= 0) {
        flowMinutes += initialCountdownValue;
        display_success_animation();
        currentState = AppState::MENU;
        menuIndex = MENU_DOWN;
        isCounting = false;
      }
    }
  }
}