#include "display.h"
#include "Arduino.h"

Adafruit_SSD1306 display(128, 64, &Wire);

String idleMessages[] = {
  "Still here?\nGet to work!",
  "Time is ticking...\nProductivity calls!",
  "Productivity\nawaits!",
  "Don't just\nsit there!",
  "Your goals\nmiss you.",
  "Is this really\nhelping?",
  "Still scrolling?\nTasks await!",
  "Your tasks\nmiss you.",
  "Clock is\nticking...",
  "Are you sure\nabout this?",
  "Productivity is\ncalling.",
  "Don't let your\ndreams be dreams.",
  "Not a break,\njust a pause.",
  "Future self\nwill thank you.",
  "Just one\nmore thing...",
  "Get back\nto it!",
  "Another minute\ngone.",
  "Are you stuck?\nI can help.",
  "Your potential\nis waiting.",
  "Don't waste\nthis moment.",
  "Focus, focus,\nfocus!",
  "The world\nmoves on.",
  "Make it count.\nNow.",
  "What's next?\nDo it.",
  "No time\nfor idle.",
  "Your project\nneeds you."
};
int currentMessageIndex = 0;
unsigned long lastMessageChangeTime = 0;
const unsigned long messageChangeInterval = 60000; // 60 seconds

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
  int mainRowTextWidth;
  int mainRowX;

  if (currentState == AppState::MENU) {
    mainRowText = menuOptions[menuIndex];
    mainRowTextWidth = mainRowText.length() * 24;
    mainRowX = (128 - mainRowTextWidth) / 2;
    display.setTextSize(4);
    display.setCursor(mainRowX, 30);
    display.print(mainRowText);
  } else if (currentState == AppState::COUNTING_UP) {
    mainRowText = String(elapsedMinutes);
    mainRowTextWidth = mainRowText.length() * 24;
    mainRowX = (128 - mainRowTextWidth) / 2;
    display.setTextSize(4);
    display.setCursor(mainRowX, 30);
    display.print(mainRowText);
  } else if (currentState == AppState::COUNTING_DOWN || currentState == AppState::SELECTING_DOWN_DURATION) {
    mainRowText = String(countdownValue);
    mainRowTextWidth = mainRowText.length() * 24;
    mainRowX = (128 - mainRowTextWidth) / 2;
    display.setTextSize(4);
    display.setCursor(mainRowX, 30);
    display.print(mainRowText);
  } else if (currentState == AppState::IDLE) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastMessageChangeTime >= messageChangeInterval) {
      lastMessageChangeTime = currentMillis;
      int newIndex = random(sizeof(idleMessages) / sizeof(idleMessages[0]));
      while (newIndex == currentMessageIndex) {
        newIndex = random(sizeof(idleMessages) / sizeof(idleMessages[0]));
      }
      currentMessageIndex = newIndex;
    }
    mainRowText = idleMessages[currentMessageIndex];

    int newlineIndex = mainRowText.indexOf('\n');

    if (newlineIndex == -1) { // Single line
      mainRowTextWidth = mainRowText.length() * 6; // 6 pixels per character for size 1
      mainRowX = (128 - mainRowTextWidth) / 2;
      display.setTextSize(1);
      display.setCursor(mainRowX, 34); // Vertically centered for single line
      display.print(mainRowText);
    } else { // Two lines
      String line1 = mainRowText.substring(0, newlineIndex);
      String line2 = mainRowText.substring(newlineIndex + 1);

      // Print first line
      mainRowTextWidth = line1.length() * 6;
      mainRowX = (128 - mainRowTextWidth) / 2;
      display.setTextSize(1);
      display.setCursor(mainRowX, 28); // Adjusted for top line of two
      display.print(line1);

      // Print second line
      mainRowTextWidth = line2.length() * 6;
      mainRowX = (128 - mainRowTextWidth) / 2;
      display.setTextSize(1);
      display.setCursor(mainRowX, 44); // Adjusted for bottom line of two
      display.print(line2);
    }
  }

  // The display.print(mainRowText); at the end is now handled within the IDLE block
  // for other states, it's handled in their respective blocks

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
