#include "encoder.h"
#include "config.h"
#include "Arduino.h" // For digitalRead, etc.

volatile int rotation = 0;

// This will be moved to timer.cpp
extern volatile bool activity_in_isr;

void IRAM_ATTR handleEncoder() {
  static uint8_t old_state = 0;
  static int8_t enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
  old_state = (old_state << 2) | ((digitalRead(DT) << 1) | digitalRead(CLK));
  rotation += enc_states[(old_state & 0x0f)];
  activity_in_isr = true; // This will be handled by the timer module
}