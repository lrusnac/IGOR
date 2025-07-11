#include "button.h"
#include "config.h"

volatile bool buttonPressedFlag = false;
unsigned long lastButtonPress = 0;

void IRAM_ATTR handleButton() {
  if (millis() - lastButtonPress > buttonDebounceDelay) {
    buttonPressedFlag = true;
    lastButtonPress = millis();
  }
}
