#include "SetupProgram.h"
#include "DownloadBtns.h"
#include "WIFIFunc.h"
#include "FlashBtns.h"

Setup systemSetup;
DownloadBtns downloader;
WIFIFunc wifi;
FlashButtonManager* buttonManager;

bool inWifiDownload = true;
bool scanSD = true;

void setup() {
    Serial.begin(115200);

    // 🔧 System initialization
    systemSetup.begin();
    systemSetup.initTFT();
    systemSetup.initSD();
    systemSetup.initTouch();

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
}

void loop() {
    downloader.update();
    
    // 🖱️ Handle touch input
    uint16_t x, y;
    if ((systemSetup.touch.tirqTouched() && systemSetup.touch.touched())) {
        TS_Point p = systemSetup.touch.getPoint();
        // Calibrate Touchscreen points with map function to the correct width and height
        x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
        y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
        systemSetup.switchCS(true);
        downloader.touch(x,y);
        systemSetup.switchCS(false);
    }
}