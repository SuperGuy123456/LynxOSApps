#include "DownloadBtns.h"

DownloadBtns::DownloadBtns() {}

void DownloadBtns::begin(TFT_eSPI* _tft, WIFIFunc* wifiPtr, fs::FS* sdPtr) {
    tft = _tft;
    wifi = wifiPtr;
    sd = sdPtr;

    tft->fillScreen(TFT_BLACK);
    drawCenteredText("Loading binpaths...", 60);

    downloadBinPaths();
    downloadVPathFiles();
    displayVPathButtons();
}

void DownloadBtns::downloadBinPaths() {
    const char* url = "https://raw.githubusercontent.com/SuperGuy123456/MTOSApps/main/binpaths.txt";
    const char* localPath = "/binpaths.txt";

    if (!wifi->downloadBinToSD(url, localPath)) {
        drawCenteredText("Failed to load binpaths", 100);
        return;
    }

    File file = sd->open(localPath, FILE_READ);
    if (!file) {
        drawCenteredText("Missing binpaths file", 100);
        return;
    }

    String content = file.readString();
    file.close();

    parseBinPaths(content);
}

void DownloadBtns::parseBinPaths(const String& content) {
    vpathEntries.clear();

    String cleanContent = content;
    cleanContent.trim();

    int start = 0;
    while (start < cleanContent.length()) {
        int sep1 = cleanContent.indexOf('#', start);
        if (sep1 == -1) break;
        int sep2 = cleanContent.indexOf('#', sep1 + 1);
        if (sep2 == -1) break;
        int sep3 = cleanContent.indexOf('#', sep2 + 1);
        if (sep3 == -1) break;
        int sep4 = cleanContent.indexOf('!', sep3 + 1);
        if (sep4 == -1) break;

        String url = cleanContent.substring(start, sep1);
        String displayName = cleanContent.substring(sep1 + 1, sep2);
        String downloadAs = cleanContent.substring(sep2 + 1, sep3);
        String isOS = cleanContent.substring(sep3 + 1, sep4);  // Not used yet

        url.trim();
        displayName.trim();
        downloadAs.trim();

        VPathEntry entry;
        entry.displayName = displayName;
        entry.vpathUrl = url;
        entry.localPath = "/vpaths/" + downloadAs;
        vpathEntries.push_back(entry);

        start = sep4 + 1;
    }

    Serial.println("✅ Parsed binpaths:");
    for (const auto& entry : vpathEntries) {
        Serial.println(" - " + entry.displayName + " | " + entry.vpathUrl + " → " + entry.localPath);
    }

    if (vpathEntries.empty()) {
        drawCenteredText("No entries found", 100);
    }
}
void DownloadBtns::downloadVPathFiles() {
    for (const auto& entry : vpathEntries) {
        wifi->downloadBinToSD(entry.vpathUrl.c_str(), entry.localPath.c_str());
        Serial.println("Waiting for second download...");
        delay(1000);
    }
}

void DownloadBtns::displayVPathButtons() {
    tft->fillScreen(TFT_BLACK);
    int y = 0;
    int itemHeight = 30;

    for (int i = 0; i < vpathEntries.size(); ++i) {
        tft->setCursor(10, y);
        tft->setTextColor(TFT_WHITE);
        tft->setTextSize(2);
        tft->print(vpathEntries[i].displayName);
        y += itemHeight;
    }

    showingBinOptions = false;
}

bool DownloadBtns::touch(uint16_t x, uint16_t y) {
    int itemHeight = 30;
    int index = y / itemHeight;
    bool justpressed = false;
    if (!showingBinOptions) {
        if (index >= 0 && index < vpathEntries.size()) {
            selectedVPathIndex = index;

            File file = sd->open(vpathEntries[index].localPath, FILE_READ);
            if (!file) {
                drawCenteredText("Missing vpath file", 100);
                return false;
            }

            String content = file.readString();
            file.close();

            parseVPathFile(content);
            displayBinOptions();
            justpressed = true;
            delay(1000);
            return false;
        }
    } else {
        if (index >= 0 && index < binEntries.size() and justpressed == false) {
            drawCenteredText("Downloading " + binEntries[index].displayName, 100);
            wifi->downloadBinToSD(binEntries[index].vpathUrl.c_str(), binEntries[index].localPath.c_str());
            return true;
        }
    }
    return false;
}

void DownloadBtns::parseVPathFile(const String& content) {
    binEntries.clear();

    String cleanContent = content;
    cleanContent.trim();

    int start = 0;
    while (start < cleanContent.length()) {
        int sep1 = cleanContent.indexOf('#', start);
        if (sep1 == -1) break;
        int sep2 = cleanContent.indexOf('#', sep1 + 1);
        if (sep2 == -1) break;
        int sep3 = cleanContent.indexOf('#', sep2 + 1);
        if (sep3 == -1) break;
        int sep4 = cleanContent.indexOf('!', sep3 + 1);
        if (sep4 == -1) break;

        String url = cleanContent.substring(start, sep1);
        String displayName = cleanContent.substring(sep1 + 1, sep2);
        String downloadAs = cleanContent.substring(sep2 + 1, sep3);
        String isOS = cleanContent.substring(sep3 + 1, sep4);  // Not used yet

        url.trim();
        displayName.trim();
        downloadAs.trim();

        BinEntry entry;
        entry.displayName = displayName;
        entry.vpathUrl = url;
        entry.localPath = "/" + downloadAs;
        binEntries.push_back(entry);

        start = sep4 + 1;
    }

    Serial.println("✅ Parsed bins:");
    for (const auto& entry : binEntries) {
        Serial.println(" - " + entry.displayName + " | " + entry.vpathUrl + " → " + entry.localPath);
    }

    if (binEntries.empty()) {
        drawCenteredText("No entries found", 100);
    }
    showingBinOptions = true;
}
void DownloadBtns::displayBinOptions() {
    tft->fillScreen(TFT_BLACK);
    int y = 0;
    int itemHeight = 30;

    for (int i = 0; i < binEntries.size(); ++i) {
        tft->setCursor(10, y);
        tft->setTextColor(TFT_GREEN);
        tft->setTextSize(2);
        tft->print(binEntries[i].displayName);
        y += itemHeight;
    }

    showingBinOptions = true;
}

void DownloadBtns::drawCenteredText(const String& text, int y) {
    int textWidth = text.length() * 12;  // Approximate width per character
    int x = (tft->width() - textWidth) / 2;
    tft->setCursor(x, y);
    tft->setTextSize(2);
    tft->setTextColor(TFT_WHITE);
    tft->print(text);
}

void DownloadBtns::update() {
    // Optional: animations or refresh logic
}
