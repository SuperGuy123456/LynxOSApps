#include "SetupProgram.h"
#include "FlashBtns.h"
#include "WIFIFunc.h"

Setup systemSetup;
FlashButtonManager* buttonManager;
WIFIFunc wifi;

void setup() {
    Serial.begin(115200);

    // 🔧 Individual system initializations
    systemSetup.begin();
    systemSetup.initTFT();
    systemSetup.initSD();
    systemSetup.initTouch();

    // 🧠 Initialize button manager
    systemSetup.switchCS(true);
    delay(10);
    buttonManager = new FlashButtonManager(&systemSetup.tft, &systemSetup.touch);
    buttonManager->begin();
    systemSetup.switchCS(false);
    delay(10);
    systemSetup.initTouch();

    // 📁 Scan SD and draw buttons

    systemSetup.switchCS(true);
    wifi.connectToWiFi("JaiGanesha2.4", "2202@Home", &SD, &systemSetup.tft);

    const char* url = "https://raw.githubusercontent.com/SuperGuy123456/LynxOSApps/main/hello.bin";
    const char* localPath = "/hi.bin";

    wifi.downloadBinToSD(url, localPath);

    systemSetup.switchCS(false);
    buttonManager->scanFirmwareFiles(SD, &systemSetup);
    buttonManager->setupFlashButtons(&systemSetup);
}

void loop() {
    // 🚀 Handle boot menu interaction
    buttonManager->handleBootMenu(&systemSetup);


}