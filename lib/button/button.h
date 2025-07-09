#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "config.h" // For buttonDebounceDelay

extern volatile bool buttonPressedFlag;
extern unsigned long lastButtonPress;

void IRAM_ATTR handleButton();

#endif // BUTTON_H