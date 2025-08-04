#include "DownloadBtns.h"

DownloadBtns::DownloadBtns(){};

void DownloadBtns::begin(TFT_eSPI *_tft, fs::FS *_sd)
{
  tft = _tft;
  sd = _sd;
}

// <URL>#<Display Name>#<Download as name>#<isOS y or n>!
std::vector<Entry> parseManifest(const String& content) {
    std::vector<Entry> entries;
    int start = 0;
    while (true) {
        int end = content.indexOf('!', start);
        if (end == -1) break;
        String line = content.substring(start, end);
        start = end + 1;

        int p1 = line.indexOf('#');
        int p2 = line.indexOf('#', p1 + 1);
        int p3 = line.indexOf('#', p2 + 1);

        if (p1 == -1 || p2 == -1 || p3 == -1) continue;

        Entry e;
        e.url = line.substring(0, p1);
        e.displayName = line.substring(p1 + 1, p2);
        e.downloadName = line.substring(p2 + 1, p3);
        e.isOS = line.substring(p3 + 1) == "y";
        entries.push_back(e);
    }
    return entries;
}