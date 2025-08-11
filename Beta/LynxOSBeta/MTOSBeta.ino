#include "SetupProgram.h"
#include "DownloadBtns.h"
#include "WIFIFunc.h"
#include "FlashBtns.h"

Setup systemSetup;
DownloadBtns downloader;
WIFIFunc wifi;
FlashButtonManager* buttonManager;

enum class AppMode {
    None,
    WifiDownload,
    BootMenu
};

AppMode currentMode = AppMode::None;
bool scanSD = true;

void setup() {
    Serial.begin(115200);

    // 🔧 System initialization
    systemSetup.begin();
    systemSetup.initTFT();
    systemSetup.initSD();
    systemSetup.initTouch();

    systemSetup.switchCS(true);
    delay(100);
    systemSetup.switchCS(false);
    
    drawStartupButtons();
    waitForModeSelection();

    if (currentMode == AppMode::WifiDownload) {
        // 🌐 Connect to Wi-Fi
        systemSetup.switchCS(true);
        delay(10);
        wifi.connectToWiFi("JaiGanesha2.4", "2202@Home", &SD, &systemSetup.tft);

        buttonManager = new FlashButtonManager(&systemSetup.tft, &systemSetup.touch);
        buttonManager->begin();

        systemSetup.switchCS(false);
        delay(10);
        systemSetup.initTouch();

        // 📥 Launch download menu
        systemSetup.switchCS(true);
        downloader.begin(&systemSetup.tft, &wifi, &SD);
        systemSetup.switchCS(false);
    } else if (currentMode == AppMode::BootMenu) {
        buttonManager = new FlashButtonManager(&systemSetup.tft, &systemSetup.touch);
        buttonManager->begin();

        // 📂 Scan and prepare boot menu
        buttonManager->scanFirmwareFiles(SD, &systemSetup);
        buttonManager->setupFlashButtons(&systemSetup);
    }
}

void loop() {
    if (currentMode == AppMode::WifiDownload) {
        downloader.update();

        // 🖱️ Handle touch input for download menu
        uint16_t x, y;
        if ((systemSetup.touch.tirqTouched() && systemSetup.touch.touched())) {
            TS_Point p = systemSetup.touch.getPoint();
            x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
            y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);

            systemSetup.switchCS(true);
            downloader.touch(x, y);
            systemSetup.switchCS(false);
        }
    } else if (currentMode == AppMode::BootMenu) {
        // 🚀 Handle boot menu interaction
        buttonManager->handleBootMenu(&systemSetup);
    }
}

void drawStartupButtons() {
    systemSetup.tft.fillScreen(TFT_BLACK);
    systemSetup.tft.setTextSize(2);
    systemSetup.tft.setTextColor(TFT_WHITE);

    // 🔘 Wi-Fi Download Button
    systemSetup.tft.fillRect(40, 60, 200, 40, TFT_BLUE);
    systemSetup.tft.setCursor(60, 70);
    systemSetup.tft.print("Wi-Fi Download");

    // 🔘 Boot Menu Button
    systemSetup.tft.fillRect(40, 120, 160, 40, TFT_GREEN);
    systemSetup.tft.setCursor(70, 130);
    systemSetup.tft.print("Boot Menu");
}

void waitForModeSelection() {
    while (currentMode == AppMode::None) {
        if (systemSetup.touch.tirqTouched() && systemSetup.touch.touched()) {
            TS_Point p = systemSetup.touch.getPoint();
            uint16_t x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
            uint16_t y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);

            if (x >= 40 && x <= 200 && y >= 60 && y <= 100) {
                currentMode = AppMode::WifiDownload;
            } else if (x >= 40 && x <= 200 && y >= 120 && y <= 160) {
                currentMode = AppMode::BootMenu;
            }

            delay(300);  // Debounce
        }
    }

    systemSetup.tft.fillScreen(TFT_BLACK);  // Clear screen after selection
}
