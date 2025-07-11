#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "config.h"

void display_init();
void display_update(int flowMinutes, int elapsedMinutes, int countdownValue, int countdownSeconds, int initialCountdownValue, MenuOption menuIndex, AppState currentState, bool displayOff, String menuOptions[]);
void display_success_animation();
void display_on();
void display_off();

#endif // DISPLAY_H