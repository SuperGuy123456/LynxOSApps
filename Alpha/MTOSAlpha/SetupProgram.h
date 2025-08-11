#ifndef SETUPPROGRAM_H
#define SETUPPROGRAM_H

#include <SPI.h>
#include <TFT_eSPI.h>
#include <SD.h>
#include <XPT2046_Touchscreen.h>
#include <Update.h>
#include "esp_ota_ops.h"

// HSPI for TFT
#define TFT_CS     15
#define TFT_DC      2
#define TFT_RST    -1
#define TFT_MOSI   13
#define TFT_MISO   12
#define TFT_SCLK   14

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define FONT_SIZE     2

// VSPI for SD
#define SD_CS        5
#define SD_MOSI     23
#define SD_MISO     19
#define SD_SCLK     18

// VSPI for Touch
#define TOUCH_CS     33
#define TOUCH_IRQ    36
#define TOUCH_MOSI   32
#define TOUCH_MISO   39
#define TOUCH_SCLK   25

class Setup {
public:
    SPIClass hspi;
    SPIClass SDvspi;
    SPIClass Touchvspi;
    XPT2046_Touchscreen touch = XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);
    TFT_eSPI tft = TFT_eSPI();  // Uses User_Setup.h

    Setup();
    void begin();
    void initTFT();
    void initSD();
    void initTouch();
    void switchCS(bool usingSD);
    bool flashFromSD(const char* path);
    bool switchToPreviousFirmware();
};

#endif
