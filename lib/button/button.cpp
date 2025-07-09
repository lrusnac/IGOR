#include "button.h"
#include "config.h"
#include "timer.h"

volatile bool buttonPressedFlag = false;
unsigned long lastButtonPress = 0;
extern volatile bool activity_in_isr;

void IRAM_ATTR handleButton() {
  if (millis() - lastButtonPress > buttonDebounceDelay) {
    buttonPressedFlag = true;
    lastButtonPress = millis();
    updateActivity();
  }
}