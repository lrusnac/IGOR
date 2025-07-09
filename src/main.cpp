#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "display.h"
#include "button.h"
#include "encoder.h"
#include "timer.h"
#include "state_machine.h"

void setup() {
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(CLK), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW), handleButton, FALLING);

  display_init();

  display_update(flowMinutes, elapsedMinutes, countdownValue, countdownSeconds, initialCountdownValue, menuIndex, currentState, displayOff, menuOptions);

  Serial.println("Setup complete, starting loop...");
}

void loop() {
  unsigned long currentMillis = millis();

  if (activity_in_isr) {
    lastActivityTime = currentMillis;
    activity_in_isr = false;
  }

  handleRotaryInputStateMachine();

  handleButtonPressStateMachine();

  handleCounting(currentMillis);

  handleInactivity(currentMillis);

  display_update(flowMinutes, elapsedMinutes, countdownValue, countdownSeconds, initialCountdownValue, menuIndex, currentState, displayOff, menuOptions);

  if (displayOff && (currentState != AppState::IDLE || activity_in_isr)) {
    display_on();
    displayOff = false;
  }
}