#include "config.h"

const unsigned long inactivityLimit = 3 * 60000; // 3 minutes
const unsigned long buttonDebounceDelay = 500;
const unsigned long displayOffTimeLimit = 27 * 60000; // 27 minutes

// Global variables for timer and state management
unsigned long lastActivityTime = 0;
unsigned long countingStartTime = 0;
unsigned long idleStartTime = 0;
bool isCounting = false;
int elapsedMinutes = 0;
int countdownValue = 20;
int initialCountdownValue = 20;
int countdownSeconds = 0;
bool displayOff = false;