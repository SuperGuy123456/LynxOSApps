#include "FlashBtns.h"

void FlashButtonManager::begin() {
    buttons.clear();
    tft->fillScreen(TFT_BLACK);
    tft->println("📋 Boot menu initialized");
}

// 📁 Raw SD scan (internal)
void FlashButtonManager::scanSD(fs::FS& sd) {
    buttons.clear();

    File root = sd.open("/");
    if (!root || !root.isDirectory()) {
        tft->println("❌ SD root not a directory");
        return;
    }

    int y = 10;
    File entry = root.openNextFile();
    while (entry) {
        String name = entry.name();
        tft->print("📁 Found file: ");
        tft->println(name);

        if (!entry.isDirectory() && name.endsWith(".bin")) {
            buttons.push_back({name, 10, y, 200, 30});
            y += 40;
        }

        entry.close();
        entry = root.openNextFile();
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "✅ Total buttons: %d", buttons.size());
    tft->println(buf);
}

// 🔧 Public wrapper with SPI switching
void FlashButtonManager::scanFirmwareFiles(fs::FS& sd, Setup* setup) {
    setup->switchCS(true);
    delay(10);
    scanSD(sd);
    setup->switchCS(false);
    delay(10);
}

// 🖼️ Draws buttons on screen
void FlashButtonManager::draw() {
    tft->fillScreen(TFT_BLACK);
    for (const auto& b : buttons) {
        tft->fillRoundRect(b.x, b.y, b.w, b.h, 5, TFT_BLUE);
        tft->drawRoundRect(b.x, b.y, b.w, b.h, 5, TFT_WHITE);
        tft->setTextFont(1);
        tft->setTextSize(2);
        tft->setTextColor(TFT_WHITE);

        uint16_t tw = tft->textWidth(b.label);
        uint16_t th = 8 * 2;

        tft->setCursor(b.x + (b.w - tw) / 2, b.y + (b.h - th) / 2);
        tft->print(b.label);
    }
}

// 🧼 Public wrapper with SPI switching
void FlashButtonManager::setupFlashButtons(Setup* setup) {
    setup->switchCS(true);
    delay(10);
    draw();
    setup->switchCS(false);
    delay(10);
    setup->initTouch();
}

// 🖲️ Detects touch and returns selected label
String FlashButtonManager::checkTouch() {
    if (!ts->touched()) return "";

    TS_Point p = ts->getPoint();
    int tx = map(p.x, 200, 3700, 1, 320);
    int ty = map(p.y, 240, 3800, 1, 240);

    for (const auto& b : buttons) {
        if (tx >= b.x && tx <= b.x + b.w &&
            ty >= b.y && ty <= b.y + b.h) {

            while (ts->touched()) delay(50);  // debounce
            return b.label;
        }
    }

    return "";
}

// 🚀 Handles boot menu logic
void FlashButtonManager::handleBootMenu(Setup* setup) {
    setup->switchCS(false);
    tft->drawCircle(10, 10, 5, TFT_RED);  // Touch indicator

    String selected = checkTouch();
    if (selected != "") {
        char buf[64];
        snprintf(buf, sizeof(buf), "🔘 Launching firmware: %s", selected.c_str());
        tft->println(buf);

        setup->switchCS(true);
        delay(100);
        setup->flashFromSD(("/" + selected).c_str());
    }

    delay(100);
}