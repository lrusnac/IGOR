#include "display.h"
#include "Arduino.h"

// Declare the display object
Adafruit_SSD1306 display(128, 64, &Wire);

void display_init() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay(); 
  Serial.println("Display initialized.");
}

void display_update(int flowMinutes, int elapsedMinutes, int countdownValue, int countdownSeconds, int initialCountdownValue, int menuIndex, AppState currentState, bool displayOff, String menuOptions[]) {
  display.setTextColor(WHITE);
  display.clearDisplay();

  // Display top row
  String topRowText;
  
  if (currentState == COUNTING_UP) { // COUNTING_UP
    topRowText = "Focus! \x18";  // Focus with upward triangle for counting UP
  } else if (currentState == COUNTING_DOWN) { // COUNTING_DOWN
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
  
  if (currentState == MENU) { // MENU
    mainRowText = menuOptions[menuIndex];  // Display UP, DOWN, or Reset in the menu
  } else if (currentState == 1) { // COUNTING_UP
    mainRowText = String(elapsedMinutes);  // Display counting up minutes
  } else if (currentState == COUNTING_DOWN || currentState == SELECTING_DOWN_DURATION) { // COUNTING_DOWN or SELECTING_DOWN_DURATION
    mainRowText = String(countdownValue);  // Display countdown minutes
  } else if (currentState == IDLE) { // IDLE
    mainRowText = "IDLE?";
  }
  
  int mainRowTextWidth = mainRowText.length() * 24;  // TextSize 4, so 24 pixels per char
  int mainRowX = (128 - mainRowTextWidth) / 2;  // Calculate centered X position

  display.setTextSize(4);  // Larger size for main row
  display.setCursor(mainRowX, 30);  // Centered on main row
  display.print(mainRowText);

  // Progress bar for countdown
  if (currentState == COUNTING_DOWN) {
    int progress = (int)(((float)countdownSeconds / (initialCountdownValue * 60)) * 128);
    display.fillRect(0, 62, progress, 2, WHITE);
  }
  
  display.display();  // Show the updated display
}

void display_success_animation() {
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

void display_on() {
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void display_off() {
  display.ssd1306_command(SSD1306_DISPLAYOFF);
}