#ifndef CONFIG_H
#define CONFIG_H

#define CLK    D6
#define DT     D7
#define SW     D4

// Enum for the different states of the application
enum AppState { MENU, COUNTING_UP, COUNTING_DOWN, SELECTING_DOWN_DURATION, IDLE };

extern const unsigned long inactivityLimit;
extern const unsigned long buttonDebounceDelay;
extern const unsigned long displayOffTimeLimit;


#endif // CONFIG_H