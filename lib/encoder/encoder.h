#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include "config.h" // For CLK, DT pins

extern volatile int rotation;

void IRAM_ATTR handleEncoder();

#endif // ENCODER_H