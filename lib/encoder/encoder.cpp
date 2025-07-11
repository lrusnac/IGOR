#include "encoder.h"
#include "config.h"
#include "Arduino.h"

volatile int rotation = 0;
volatile uint8_t old_state = 0;

void IRAM_ATTR handleEncoder() {
  // static uint8_t old_state = 0; // Removed static as it's now global
  static int8_t enc_states[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
  old_state = (old_state << 2) | ((digitalRead(DT) << 1) | digitalRead(CLK));
  rotation += enc_states[(old_state & 0x0f)];
}
