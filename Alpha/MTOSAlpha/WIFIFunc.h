#ifndef WIFIFUNC_H
#define WIFIFUNC_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <FS.h>
#include <TFT_eSPI.h>

class WIFIFunc {
public:
    WIFIFunc();
    bool connectToWiFi(const char* ssid, const char* password, fs::FS* sdRef, TFT_eSPI* tftRef);
    bool downloadBinToSD(const char* url, const char* localPath);

private:
    fs::FS* SDCard;
    TFT_eSPI* tft;
};

#endif  // WIFIFUNC_H