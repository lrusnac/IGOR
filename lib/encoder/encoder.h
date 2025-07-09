#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include "config.h"

extern volatile int rotation;

void IRAM_ATTR handleEncoder();

#endif // ENCODER_H