#include "SetupProgram.h"

Setup systemSetup;

void setup() {
    Serial.begin(115200);
    Serial.println("Hello World!");

    systemSetup.begin();        // Initializes SPI, pins, etc.
    systemSetup.initTFT();     // Initializes display
    systemSetup.initSD();      // Initializes SD card
    systemSetup.initTouch();   // Initializes touch controller

    systemSetup.tft.setCursor(0,0);
    systemSetup.tft.print("Hello World!");
}

void loop() {
    systemSetup.tft.fillScreen(TFT_BLACK);  // Clear screen

    systemSetup.switchCS(false);            // Enable touch
    if (systemSetup.touch.touched()) {
        TS_Point p = systemSetup.touch.getPoint();

        if (!systemSetup.switchToPreviousFirmware()) {
            Serial.println("Unable to revert to OS!");
        }
    }
    systemSetup.switchCS(true);             // Re-enable SD
    delay(100);
}