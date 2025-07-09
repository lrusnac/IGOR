#include "display.h"
#include "Arduino.h"

Adafruit_SSD1306 display(128, 64, &Wire);

void display_init() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  Serial.println("Display initialized.");
}

void display_update(int flowMinutes, int elapsedMinutes, int countdownValue, int countdownSeconds, int initialCountdownValue, MenuOption menuIndex, AppState currentState, bool displayOff, String menuOptions[]) {
  display.setTextColor(WHITE);
  display.clearDisplay();

  String topRowText;

  if (currentState == AppState::COUNTING_UP) {
    topRowText = "Focus! \x18";
  } else if (currentState == AppState::COUNTING_DOWN) {
    topRowText = "Focus! \x19";
  } else {
    topRowText = "Flow: " + String(flowMinutes);
  }

  int topRowTextWidth = topRowText.length() * 12;
  int topRowX = (128 - topRowTextWidth) / 2;

  display.setTextSize(2);
  display.setCursor(topRowX, 0);
  display.print(topRowText);

  String mainRowText;

  if (currentState == AppState::MENU) {
    mainRowText = menuOptions[menuIndex];
  } else if (currentState == AppState::COUNTING_UP) {
    mainRowText = String(elapsedMinutes);
  } else if (currentState == AppState::COUNTING_DOWN || currentState == AppState::SELECTING_DOWN_DURATION) {
    mainRowText = String(countdownValue);
  } else if (currentState == AppState::IDLE) {
    mainRowText = "IDLE?";
  }

  int mainRowTextWidth = mainRowText.length() * 24;
  int mainRowX = (128 - mainRowTextWidth) / 2;

  display.setTextSize(4);
  display.setCursor(mainRowX, 30);
  display.print(mainRowText);

  if (currentState == AppState::COUNTING_DOWN) {
    int progress = (int)(((float)countdownSeconds / (initialCountdownValue * 60)) * 128);
    display.fillRect(0, 62, progress, 2, WHITE);
  }

  display.display();
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