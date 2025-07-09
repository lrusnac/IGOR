#include "state_machine.h"
#include "config.h"
#include "display.h" // For updateDisplay, display_success_animation
#include "timer.h"   // For lastActivityTime, countingStartTime, elapsedMinutes, countdownValue, initialCountdownValue, countdownSeconds, isCounting, displayOff
#include "button.h" // For buttonPressedFlag
#include "encoder.h" // For rotation

AppState currentState = AppState::MENU;
int flowMinutes = 0;
int menuIndex = 0;
String menuOptions[3] = {"UP", "DOWN", "Reset"};

void initializeStateMachine() {
  // Any initial setup for the state machine
}

void handleButtonPressStateMachine() {
  if (buttonPressedFlag) {
    buttonPressedFlag = false;
    lastActivityTime = millis();

    switch (currentState) {
      case AppState::MENU:
        if (menuIndex == 0) {  // UP selected
          startCountingUp();
        } else if (menuIndex == 1) {  // DOWN selected
          startSelectingDownDuration();
        } else if (menuIndex == 2) {  // Reset selected
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
    // updateDisplay(); // This will be called from main loop after state change
  }
}

void handleRotaryInputStateMachine() {
  if (rotation / 4 != 0) {
    int rotation_value = rotation / 4;
    rotation = 0; // Reset rotation after processing

    if (currentState == AppState::IDLE) { // REPLACED
      currentState = AppState::MENU;
      if (displayOff) {
        display_on();
        displayOff = false;
      }
    } else if (currentState == AppState::MENU) {
      menuIndex = (menuIndex + rotation_value + 3) % 3;
    } else if (currentState == AppState::SELECTING_DOWN_DURATION) { // REPLACED
      countdownValue = max(1, countdownValue + rotation_value);
    }

    // updateDisplay(); // This will be called from main loop after state change
  }
}

void startCountingUp() {
  currentState = AppState::COUNTING_UP; // REPLACED
  elapsedMinutes = 0;
  isCounting = true;
  countingStartTime = millis();
  lastActivityTime = millis();
  Serial.println("Counting UP started.");
}

void startSelectingDownDuration() {
  currentState = AppState::SELECTING_DOWN_DURATION; // REPLACED
  countdownValue = 20;
  countdownSeconds = countdownValue * 60;
  isCounting = true;
  lastActivityTime = millis();
  Serial.println("Selecting DOWN duration.");
}

void confirmCountdownSelection() {
  initialCountdownValue = countdownValue;
  countdownSeconds = initialCountdownValue * 60;
  currentState = AppState::COUNTING_DOWN; // REPLACED
  isCounting = true;
  countingStartTime = millis();
  lastActivityTime = millis();
  Serial.print("Counting DOWN started with "); Serial.print(countdownValue); Serial.println(" minutes.");
}

void stopCountingUp() {
  flowMinutes += elapsedMinutes;
  display_success_animation();
  currentState = AppState::MENU; // REPLACED
  isCounting = false;
  Serial.println("Counting UP stopped. Returning to MENU.");
}

void stopCountingDown() {
  flowMinutes += (initialCountdownValue - countdownValue);
  display_success_animation();
  currentState = AppState::MENU; // REPLACED
  isCounting = false;
  Serial.println("Counting DOWN stopped. Returning to MENU.");
}

void resetFlowMinutes() {
  flowMinutes = 0;
  Serial.println("Flow minutes reset to 0.");
  // updateDisplay(); // This will be called from main loop after state change
}