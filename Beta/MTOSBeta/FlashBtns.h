#ifndef FLASHBUTTONMANAGER_H
#define FLASHBUTTONMANAGER_H

#include <vector>
#include <FS.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "SetupProgram.h"

struct FlashButton {
    String label;
    int x, y, w, h;
};

class FlashButtonManager {
public:
    FlashButtonManager(TFT_eSPI* display, XPT2046_Touchscreen* touch)
        : tft(display), ts(touch) {}

    void begin();
    void scanSD(fs::FS& sd);
    void scanFirmwareFiles(fs::FS& sd, Setup* setup);
    void setupFlashButtons(Setup* setup);
    void draw();
    String checkTouch();
    void handleBootMenu(Setup* setup);
    std::vector<FlashButton> buttons;
private:
    
    TFT_eSPI* tft;
    XPT2046_Touchscreen* ts;
};

#endif
