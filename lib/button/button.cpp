#include "button.h"
#include "config.h"

volatile bool buttonPressedFlag = false;
unsigned long lastButtonPress = 0;
extern volatile bool activity_in_isr; // This will be moved to timer.cpp

void ICACHE_RAM_ATTR handleButton() {
  if (millis() - lastButtonPress > buttonDebounceDelay) {
    buttonPressedFlag = true;
    lastButtonPress = millis();
    activity_in_isr = true; // This will be handled by the timer module
  }
}