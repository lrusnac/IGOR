#include "state_machine.h"
#include "config.h"
#include "display.h"
#include "timer.h"
#include "button.h"
#include "encoder.h"

AppState currentState = AppState::MENU;
int flowMinutes = 0;
MenuOption menuIndex = MENU_UP;
String menuOptions[3] = {"UP", "DOWN", "Reset"};

void initializeStateMachine() {
}

void handleButtonPressStateMachine() {
  if (buttonPressedFlag) {
    buttonPressedFlag = false;
    updateActivity();

    switch (currentState) {
      case AppState::MENU:
        if (menuIndex == MENU_UP) {
          startCountingUp();
        } else if (menuIndex == MENU_DOWN) {
          startSelectingDownDuration();
        } else if (menuIndex == MENU_RESET) {
          resetFlowMinutes();
        }
        break;

      case AppState::SELECTING_DOWN_DURATION:
        confirmCountdownSelection();
        break;

      case AppState::COUNTING_UP:
        stopCountingUp();
        break;

      case AppState::COUNTING_DOWN:
        stopCountingDown();
        break;

      case AppState::IDLE:
        currentState = AppState::MENU;
        lastActivityTime = millis();
        if (displayOff) {
          display_on();
          displayOff = false;
        }
        break;
    }
  }
}

void handleRotaryInputStateMachine() {
  if (rotation / 4 != 0) {
    int rotation_value = rotation / 4;
    rotation = 0;

    if (currentState == AppState::IDLE) {
      currentState = AppState::MENU;
      if (displayOff) {
        display_on();
        displayOff = false;
      }
    } else if (currentState == AppState::MENU) {
      menuIndex = (MenuOption)((menuIndex + rotation_value + 3) % 3);
    } else if (currentState == AppState::SELECTING_DOWN_DURATION) {
      countdownValue = max(1, countdownValue + rotation_value);
    }
  }
}

void startCountingUp() {
  currentState = AppState::COUNTING_UP;
  elapsedMinutes = 0;
  isCounting = true;
  countingStartTime = millis();
  updateActivity();
  Serial.println("Counting UP started.");
}

void startSelectingDownDuration() {
  currentState = AppState::SELECTING_DOWN_DURATION;
  countdownValue = 20;
  countdownSeconds = countdownValue * 60;
  isCounting = true;
  updateActivity();
  Serial.println("Selecting DOWN duration.");
}

void confirmCountdownSelection() {
  initialCountdownValue = countdownValue;
  countdownSeconds = initialCountdownValue * 60;
  currentState = AppState::COUNTING_DOWN;
  isCounting = true;
  countingStartTime = millis();
  updateActivity();
  Serial.print("Counting DOWN started with "); Serial.print(countdownValue); Serial.println(" minutes.");
}

void stopCountingUp() {
  flowMinutes += elapsedMinutes;
  display_success_animation();
  updateActivity();
  currentState = AppState::MENU;
  isCounting = false;
  Serial.println("Counting UP stopped. Returning to MENU.");
}

void stopCountingDown() {
  flowMinutes += (initialCountdownValue - countdownValue);
  display_success_animation();
  updateActivity();
  currentState = AppState::MENU;
  isCounting = false;
  Serial.println("Counting DOWN stopped. Returning to MENU.");
}

void resetFlowMinutes() {
  flowMinutes = 0;
  Serial.println("Flow minutes reset to 0.");
}