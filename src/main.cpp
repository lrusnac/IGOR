#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>


//-----------------------------------------------
Adafruit_SSD1306 display(128, 64, &Wire, D4);

//-----------------------------------------------
#define CLK    D6
#define DT     D7
#define SW     D4

//-----------------------------------------------
int flowMinutes = 0;   // Total flow minutes
int menuIndex = 0;     // 0 for UP, 1 for DOWN, 2 for Reset
String menuOptions[3] = {"UP", "DOWN", "Reset"};  // Label reset option as "Reset"
unsigned long lastActivityTime = 0;  // For inactivity detection
const unsigned long inactivityLimit = 3 * 60000;  // 3 minutes in milliseconds

enum State { MENU, COUNTING_UP, COUNTING_DOWN, SELECTING_DOWN_DURATION, IDLE };
State currentState = MENU;

int countdownValue = 20;  // Default value for countdown
int initialCountdownValue = 20;  // Store the countdown value when selected
unsigned long previousMillis = 0;  // For counting logic
int elapsedMinutes = 0;
bool isCounting = false;

// Rotary encoder state
volatile int rotation = 0;
volatile bool buttonPressedFlag = false;
volatile bool activity_in_isr = false;
static int8_t last_state = 0;

// Debounce timers
unsigned long lastButtonPress = 0;
const unsigned long buttonDebounceDelay = 500;

// IDLE mode extended behavior
const unsigned long displayOffTimeLimit = 30 * 60000;  // 30 minutes in milliseconds

unsigned long idleStartTime = 0;  // Track when IDLE mode starts
bool displayOff = false;  // Track if the display is off

//=========================================================
void initHardware();
void initDisplay();
void updateDisplay();
void handleButtonPress();
void startCountingUp();
void startSelectingDownDuration();
void confirmCountdownSelection();
void stopCountingUp();
void stopCountingDown();
void resetFlowMinutes();
void handleCounting(unsigned long currentMillis);
void successAnimation();
void handleRotaryInput();
void handleInactivity(unsigned long currentMillis);

// Interrupt Service Routines
void ICACHE_RAM_ATTR handleEncoder();
void ICACHE_RAM_ATTR handleButton();

//=========================================================
void setup() {  
  initHardware();
  last_state = (digitalRead(CLK) << 1) | digitalRead(DT);
  initDisplay();
  updateDisplay();
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
  handleRotaryInput();

  // Handle button presses and states
  handleButtonPress();

  // Handle counting logic
  handleCounting(currentMillis);

  // Handle inactivity
  handleInactivity(currentMillis);
}

//=========================================================
// Initialize hardware pins and serial communication
void initHardware() {
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  Serial.begin(9600);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(CLK), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW), handleButton, FALLING);
}

//=========================================================
// Initialize the OLED display
void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay(); 
  Serial.println("Display initialized.");
}

//=========================================================
// Update the OLED display with the current state
void updateDisplay() {
  display.setTextColor(WHITE);
  display.clearDisplay();

  // Display top row
  String topRowText;
  
  if (currentState == COUNTING_UP) {
    topRowText = "Focus! \x18";  // Focus with upward triangle for counting UP
  } else if (currentState == COUNTING_DOWN) {
    topRowText = "Focus! \x19";  // Focus with downward triangle for counting DOWN
  } else {
    topRowText = "Flow: " + String(flowMinutes);  // Display total flow minutes when not counting
  }

  int topRowTextWidth = topRowText.length() * 12;  // TextSize 2, so 12 pixels per char
  int topRowX = (128 - topRowTextWidth) / 2;  // Center the text on the top row

  display.setTextSize(2);  // Larger size for top row
  display.setCursor(topRowX, 0);  // Centered on top row
  display.print(topRowText);

  // Display main row (menu or counting values)
  String mainRowText;
  
  if (currentState == MENU) {
    mainRowText = menuOptions[menuIndex];  // Display UP, DOWN, or Reset in the menu
  } else if (currentState == COUNTING_UP) {
    mainRowText = String(elapsedMinutes);  // Display counting up minutes
  } else if (currentState == COUNTING_DOWN || currentState == SELECTING_DOWN_DURATION) {
    mainRowText = String(countdownValue);  // Display countdown minutes
  } else if (currentState == IDLE) {
    mainRowText = "IDLE?";
  }
  
  int mainRowTextWidth = mainRowText.length() * 24;  // TextSize 4, so 24 pixels per char
  int mainRowX = (128 - mainRowTextWidth) / 2;  // Calculate centered X position

  display.setTextSize(4);  // Larger size for main row
  display.setCursor(mainRowX, 30);  // Centered on main row
  display.print(mainRowText);
  
  display.display();  // Show the updated display
}

//=========================================================
// Handle button presses and manage state transitions
void handleButtonPress() {
  if (buttonPressedFlag) {
    buttonPressedFlag = false;
    lastActivityTime = millis();

    switch (currentState) {
      case MENU:
        if (menuIndex == 0) {  // UP selected
          startCountingUp();
        } else if (menuIndex == 1) {  // DOWN selected
          startSelectingDownDuration();
        } else if (menuIndex == 2) {  // Reset selected
          resetFlowMinutes();  // Reset the total focus time to 0
        }
        break;
        
      case SELECTING_DOWN_DURATION:
        confirmCountdownSelection();
        break;

      case COUNTING_UP:
        stopCountingUp();
        break;

      case COUNTING_DOWN:
        stopCountingDown();
        break;

      case IDLE:
        currentState = MENU;
        lastActivityTime = millis();
        if (displayOff) {
          display.ssd1306_command(SSD1306_DISPLAYON);
          displayOff = false;
        }
        break;
    }
    updateDisplay();
  }
}

//=========================================================
// Start counting up
void startCountingUp() {
  currentState = COUNTING_UP;
  elapsedMinutes = 0;
  isCounting = true;
  lastActivityTime = millis();  // Reset inactivity timer
  Serial.println("Counting UP started.");
}

//=========================================================
// Start selecting the countdown duration
void startSelectingDownDuration() {
  currentState = SELECTING_DOWN_DURATION;
  countdownValue = 20;
  lastActivityTime = millis();  // Reset inactivity timer
  Serial.println("Selecting DOWN duration.");
}

//=========================================================
// Confirm countdown selection and start counting down
void confirmCountdownSelection() {
  initialCountdownValue = countdownValue;
  currentState = COUNTING_DOWN;
  isCounting = true;
  lastActivityTime = millis();  // Reset inactivity timer
  Serial.print("Counting DOWN started with "); Serial.print(countdownValue); Serial.println(" minutes.");
}

//=========================================================
// Stop counting up and return to menu
void stopCountingUp() {
  flowMinutes += elapsedMinutes;
  successAnimation();
  currentState = MENU;
  isCounting = false;
  Serial.println("Counting UP stopped. Returning to MENU.");
}

//=========================================================
// Stop counting down and return to menu
void stopCountingDown() {
  flowMinutes += (initialCountdownValue - countdownValue);
  successAnimation();
  currentState = MENU;
  isCounting = false;
  Serial.println("Counting DOWN stopped. Returning to MENU.");
}

//=========================================================
// Reset the total flow minutes counter to 0
void resetFlowMinutes() {
  flowMinutes = 0;
  Serial.println("Flow minutes reset to 0.");
  updateDisplay();  // Update the display to show the reset value
}

//=========================================================
// Handle counting up or down logic
void handleCounting(unsigned long currentMillis) {
  if (!isCounting || (currentMillis - previousMillis < 60000)) return;

  previousMillis = currentMillis;
  
  if (currentState == COUNTING_UP) {
    elapsedMinutes++;
    updateDisplay();
    Serial.print("Counting UP: "); Serial.println(elapsedMinutes);
  } else if (currentState == COUNTING_DOWN) {
    countdownValue--;
    if (countdownValue <= 0) {
      flowMinutes += initialCountdownValue;
      successAnimation();
      currentState = MENU;
      isCounting = false;
      Serial.println("Countdown finished, returning to MENU.");
    }
    updateDisplay();
    Serial.print("Counting DOWN: "); Serial.println(countdownValue);
  }
}

//=========================================================
// Success animation when a session ends
void successAnimation() {
  display.clearDisplay();
  int centerX = 64, centerY = 32;

  for (int radius = 2; radius <= 30; radius += 2) {
    display.drawCircle(centerX, centerY, radius, WHITE);
    display.display();
    delay(5);

    if (radius % 4 == 0) {
      display.clearDisplay();
      display.display();
      delay(1);
    }
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(20, 20);
  display.print("SUCCESS!");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
}

//=========================================================
// Handle rotary input for menu and countdown selection
void handleRotaryInput() {
  if (rotation / 4 != 0) {
    int rotation_value = rotation / 4;

    if (currentState == IDLE) {
      currentState = MENU;
      if (displayOff) {
        display.ssd1306_command(SSD1306_DISPLAYON);
        displayOff = false;
      }
    } else if (currentState == MENU) {
      menuIndex = (menuIndex + rotation_value + 3) % 3;
    } else if (currentState == SELECTING_DOWN_DURATION) {
      countdownValue = max(1, countdownValue + rotation_value);
    }
    
    rotation = 0;
    updateDisplay();
  }
}

//=========================================================
// Handle inactivity and switch to IDLE if necessary
void handleInactivity(unsigned long currentMillis) {
  if ((currentState == MENU || currentState == SELECTING_DOWN_DURATION) && 
      (currentMillis - lastActivityTime > inactivityLimit)) {
    if (currentState != IDLE) {
      currentState = IDLE;
      idleStartTime = millis();
      updateDisplay();
    }
  }

  if (currentState == IDLE && !displayOff && (currentMillis - idleStartTime > displayOffTimeLimit)) {
    displayOff = true;
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }
}

//=========================================================
// Interrupt Service Routines

void ICACHE_RAM_ATTR handleEncoder() {
  static int8_t transitions[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  uint8_t state = (digitalRead(CLK) << 1) | digitalRead(DT);
  int8_t transition = transitions[(last_state << 2) | state];
  if (transition) {
    rotation += transition;
  }
  last_state = state;
  activity_in_isr = true;
}

void ICACHE_RAM_ATTR handleButton() {
  if (millis() - lastButtonPress > buttonDebounceDelay) {
    buttonPressedFlag = true;
    lastButtonPress = millis();
    activity_in_isr = true;
  }
}

