#include "state_machine.h"
#include "config.h"
#include "display.h"
#include "button.h"
#include "encoder.h"

AppState currentState = AppState::MENU;
MenuOption menuIndex = MENU_UP;
int flowMinutes = 0;
String menuOptions[3] = {"UP", "DOWN", "Reset"};

void handleButtonPressStateMachine() {
  if (buttonPressedFlag) {
    buttonPressedFlag = false;

    switch (currentState) {
      case AppState::MENU:
        lastActivityTime = millis();
        if (menuIndex == MENU_UP) {
          currentState = AppState::COUNTING_UP;
          elapsedMinutes = 0;
          isCounting = true;
          countingStartTime = millis();
        } else if (menuIndex == MENU_DOWN) {
          currentState = AppState::SELECTING_DOWN_DURATION;
          countdownValue = 20;
          countdownSeconds = countdownValue * 60;
          isCounting = true;
        } else if (menuIndex == MENU_RESET) {
          flowMinutes = 0;
        }
        break;

      case AppState::SELECTING_DOWN_DURATION:
        lastActivityTime = millis();
        initialCountdownValue = countdownValue;
        countdownSeconds = initialCountdownValue * 60;
        currentState = AppState::COUNTING_DOWN;
        isCounting = true;
        countingStartTime = millis();
        break;

      case AppState::COUNTING_UP:
        lastActivityTime = millis();
        flowMinutes += elapsedMinutes;
        display_success_animation();
        currentState = AppState::MENU;
        menuIndex = MENU_UP;
        isCounting = false;
        break;

      case AppState::COUNTING_DOWN:
        lastActivityTime = millis();
        currentState = AppState::MENU;
        menuIndex = MENU_DOWN;
        isCounting = false;
        break;

      case AppState::IDLE:
        currentState = AppState::MENU;
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
    } else { // Update activity only if not IDLE
      lastActivityTime = millis();
      if (currentState == AppState::MENU) {
        menuIndex = (MenuOption)((menuIndex + rotation_value + 3) % 3);
      } else if (currentState == AppState::SELECTING_DOWN_DURATION) {
        countdownValue = max(1, countdownValue + rotation_value);
      }
    }
  }
}

void handleInactivity() {
  if ((currentState == AppState::MENU || currentState == AppState::SELECTING_DOWN_DURATION) &&
      (millis() - lastActivityTime > inactivityLimit)) {
    currentState = AppState::IDLE;
    idleStartTime = millis();
  }

  if (currentState == AppState::IDLE && !displayOff && (millis() - idleStartTime > displayOffTimeLimit)) {
    display_off();
    displayOff = true;
  }
}

void handleCounting() {
  if (!isCounting) return;

  unsigned long elapsedTime = millis() - countingStartTime;

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
        lastActivityTime = millis();
        currentState = AppState::MENU;
        menuIndex = MENU_DOWN;
        isCounting = false;
      }
    }
  }
}
