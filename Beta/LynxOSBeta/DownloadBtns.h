#pragma once
#include "SetupProgram.h"
#include "WIFIFunc.h"

class DownloadBtns {
public:
    DownloadBtns();

    void begin(TFT_eSPI* _tft, WIFIFunc* wifi, fs::FS* sd);
    void update();
    bool touch(uint16_t x, uint16_t y);

private:
    TFT_eSPI* tft;
    WIFIFunc* wifi;
    fs::FS* sd;

    struct VPathEntry {
        String displayName;
        String vpathUrl;
        String localPath;
    };

    struct BinEntry {
        String displayName;
        String vpathUrl;
        String localPath;
    };

    std::vector<VPathEntry> vpathEntries;
    std::vector<BinEntry> binEntries;

    bool showingBinOptions = false;
    int selectedVPathIndex = -1;

    void downloadBinPaths();
    void parseBinPaths(const String& content);
    void downloadVPathFiles();
    void parseVPathFile(const String& content);
    void displayVPathButtons();
    void displayBinOptions();
    void drawCenteredText(const String& text, int y);
};