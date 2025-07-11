#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include "config.h"

extern volatile int rotation;
extern volatile uint8_t old_state;

void IRAM_ATTR handleEncoder();

#endif // ENCODER_H