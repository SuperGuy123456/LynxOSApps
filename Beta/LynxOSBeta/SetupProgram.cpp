#include "SetupProgram.h"

Setup::Setup()
    : hspi(HSPI), SDvspi(VSPI), Touchvspi(VSPI), touch(TOUCH_CS, TOUCH_IRQ) {
    Serial.println("✅ Setup class initialized.");
}

void Setup::begin() {
    pinMode(SD_CS, OUTPUT);
    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    digitalWrite(TOUCH_CS, HIGH);
}

void Setup::initTFT() {
    hspi.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(FONT_SIZE);
    tft.setCursor(10, 10);
    tft.println("TFT_eSPI on HSPI");
}

void Setup::initSD() {
    SDvspi.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    switchCS(true);
    if (!SD.begin(SD_CS, SDvspi)) {
        Serial.println("❌ SD init failed!");
    } else {
        Serial.println("✅ SD init OK");
    }

    File root = SD.open("/");
    while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        Serial.print("📁 Found file: ");
        Serial.println(entry.name());
        entry.close();
    }
}

void Setup::initTouch() {
    Touchvspi.begin(TOUCH_SCLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
    switchCS(false);
    touch.begin();
    touch.setRotation(1);
    Serial.println("✅ Touch initialized");
}

void Setup::switchCS(bool usingSD) {
    if (usingSD) {
        Serial.println("🔄 Switching to SD SPI");

        // Deinit Touch SPI
        Touchvspi.end();  // Fully release SPI bus used by touch

        // Reinit SD SPI
        SDvspi.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
        if (!SD.begin(SD_CS, SDvspi)) {
            Serial.println("❌ SD init failed during switch!");
        } else {
            Serial.println("✅ SD ready");
        }

    } else {
        Serial.println("🔄 Switching to Touch SPI");

        // Deinit SD SPI
        SD.end();         // Unmount SD
        SDvspi.end();     // Release SPI bus used by SD

        // Reinit Touch SPI
        Touchvspi.begin(TOUCH_SCLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
        touch.begin();    // Reinitialize touch controller
        touch.setRotation(1);
        Serial.println("✅ Touch ready");
    }

    delay(10);  // Let SPI bus settle
}

bool Setup::flashFromSD(const char* path) {
    SDvspi.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, SDvspi)) {
        Serial.println("❌ SD card init failed!");
        tft.fillScreen(TFT_WHITE);
        tft.drawCentreString("SD init failed!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
        return false;
    }

    File firmwareFile = SD.open(path);
    if (!firmwareFile) {
        Serial.println("❌ Failed to open firmware file!");
        tft.fillScreen(TFT_WHITE);
        tft.drawCentreString("File open failed!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
        return false;
    }

    size_t fileSize = firmwareFile.size();
    if (fileSize == 0 || fileSize > ESP.getFlashChipSize()) {
        Serial.println("❌ Invalid file size!");
        tft.fillScreen(TFT_WHITE);
        tft.drawCentreString("Invalid file size!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
        firmwareFile.close();
        return false;
    }

    const esp_partition_t* updatePartition = esp_ota_get_next_update_partition(NULL);
    if (!updatePartition) {
        Serial.println("❌ No update partition!");
        firmwareFile.close();
        return false;
    }

    Serial.printf("📝 Target update partition: %s @ 0x%08X\n", updatePartition->label, updatePartition->address);
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(FONT_SIZE);
    tft.setCursor(10, 30);
    tft.println("Updating firmware...");
    tft.fillRect(0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, 10, TFT_LIGHTGREY);

    esp_ota_handle_t otaHandle;
    esp_err_t err = esp_ota_begin(updatePartition, OTA_SIZE_UNKNOWN, &otaHandle);
    if (err != ESP_OK) {
        Serial.println("❌ OTA begin failed!");
        firmwareFile.close();
        return false;
    }

    const size_t bufSize = 2048;
    uint8_t buf[bufSize];
    size_t written = 0;
    int lastPercent = -1;

    while (int len = firmwareFile.read(buf, bufSize)) {
        err = esp_ota_write(otaHandle, buf, len);
        if (err != ESP_OK) {
            Serial.println("❌ OTA write failed!");
            esp_ota_end(otaHandle);
            firmwareFile.close();
            return false;
        }
        written += len;

        int percent = (written * 100) / fileSize;
        if (percent != lastPercent) {
            lastPercent = percent;
            int px = (written * SCREEN_WIDTH) / fileSize;
            tft.fillRect(0, SCREEN_HEIGHT - 20, px, 10, TFT_GREEN);

            char percentStr[10];
            sprintf(percentStr, "%d%%", percent);
            tft.fillRect(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT - 40, 60, 20, TFT_WHITE);
            tft.setCursor(SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT - 40);
            tft.setTextColor(TFT_BLACK);
            tft.print(percentStr);
        }
    }

    firmwareFile.close();
    Serial.printf("✅ Firmware written: %u bytes\n", written);

    err = esp_ota_end(otaHandle);
    if (err != ESP_OK || !esp_ota_set_boot_partition(updatePartition)) {
        Serial.println("❌ OTA finalize or boot partition set failed!");
        return false;
    }

    tft.fillScreen(TFT_WHITE);
    tft.drawCentreString("Update done. Restarting...", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
    delay(2000);
    ESP.restart();
    return true;
}

bool Setup::switchToPreviousFirmware() {
    const esp_partition_t* current = esp_ota_get_running_partition();
    const esp_partition_t* other = nullptr;

    if (strcmp(current->label, "ota_0") == 0) {
        other = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
    } else {
        other = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    }

    if (!other) {
        Serial.println("❌ Rollback partition not found!");
        return false;
    }

    if (esp_ota_set_boot_partition(other) != ESP_OK) {
        Serial.println("❌ Failed to set rollback partition!");
        return false;
    }

    Serial.printf("🔁 Switched to %s. Restarting...\n", other->label);
    delay(1000);
    ESP.restart();
    return true;
}
