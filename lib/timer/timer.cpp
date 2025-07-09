#include "timer.h"
#include "config.h"
#include "display.h"
#include "state_machine.h"

unsigned long lastActivityTime = 0;
unsigned long countingStartTime = 0;
unsigned long idleStartTime = 0;
volatile bool activity_in_isr = false;
bool isCounting = false;
int elapsedMinutes = 0;
int countdownValue = 20;
int initialCountdownValue = 20;
int countdownSeconds = 0;
bool displayOff = false;

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
  } else if (currentState == AppState::COUNTING_DOWN) {
    if (elapsedTime >= (unsigned long)(initialCountdownValue * 60 - countdownSeconds + 1) * 1000) {
      countdownSeconds--;
      if (countdownSeconds % 60 == 0) {
        countdownValue--;
      }
      if (countdownValue <= 0 && countdownSeconds <= 0) {
        stopCountingDown();
        isCounting = false;
      }
    }
  }
}