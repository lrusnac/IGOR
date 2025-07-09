#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Include our new modules
#include "config.h"
#include "display.h"
#include "button.h"
#include "encoder.h"
#include "timer.h"
#include "state_machine.h"

//=========================================================
void setup() {
  // Initialize hardware pins
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  Serial.begin(9600);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(CLK), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW), handleButton, FALLING);

  // Initialize display
  display_init();

  // Initial display update
  display_update(flowMinutes, elapsedMinutes, countdownValue, countdownSeconds, initialCountdownValue, menuIndex, currentState, displayOff, menuOptions);

  Serial.println("Setup complete, starting loop...");
}

//=========================================================
void loop() {
  unsigned long currentMillis = millis();

  if (activity_in_isr) {
    lastActivityTime = currentMillis;
    activity_in_isr = false;
  }

  // Handle rotary encoder input
  handleRotaryInputStateMachine();

  // Handle button presses and states
  handleButtonPressStateMachine();

  // Handle counting logic
  handleCounting(currentMillis);

  // Handle inactivity
  handleInactivity(currentMillis);

  // Update display if any state or value has changed
  // This needs to be called after all logic updates to reflect the latest state
  display_update(flowMinutes, elapsedMinutes, countdownValue, countdownSeconds, initialCountdownValue, menuIndex, currentState, displayOff, menuOptions);

  // If display was turned off due to inactivity, turn it back on if there's activity
  if (displayOff && (currentState != AppState::IDLE || activity_in_isr)) {
    display_on();
    displayOff = false;
  }
}