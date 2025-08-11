#include "SetupProgram.h"

Setup::Setup()
    : hspi(HSPI), SDvspi(VSPI), Touchvspi(VSPI), touch(TOUCH_CS, TOUCH_IRQ)
{
    Serial.println("Setup class initialized.");
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
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("TFT_eSPI on HSPI");
}

void Setup::initSD() {
    SDvspi.begin(18, 19, 23);  // VSPI default pins
    switchCS(true);
    if (!SD.begin(SD_CS, SDvspi)) {
        Serial.println("SD init failed!");
    } else {
        Serial.println("SD init OK");
    }
    File root = SD.open("/");
while (true) {
  File entry = root.openNextFile();
  if (!entry) break;
  Serial.print("Found file: ");
  Serial.println(entry.name());
  entry.close();
}
}

void Setup::initTouch() {
    Touchvspi.begin(TOUCH_SCLK, TOUCH_MISO, TOUCH_MOSI);
    switchCS(false);
    touch.begin(Touchvspi);
    touch.setRotation(1);
    Serial.println("Touch initialized");
}

void Setup::switchCS(bool usingSD) {
    if (usingSD) {
        digitalWrite(TOUCH_CS, HIGH);
        digitalWrite(SD_CS, LOW);
    } else {
        digitalWrite(SD_CS, HIGH);
        digitalWrite(TOUCH_CS, LOW);
    }
    delayMicroseconds(5);  // Allow bus to settle
}

bool Setup::flashFromSD(const char* path) {
    // Re-init SD SPI
    SDvspi.begin(18, 19, 23, SD_CS);
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
        tft.drawCentreString("Failed to open firmware file!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
        return false;
    }

    size_t fileSize = firmwareFile.size();
    Serial.printf("📦 Firmware file size: %u bytes\n", fileSize);
    if (fileSize == 0 || fileSize > ESP.getFlashChipSize()) {
        Serial.println("❌ Invalid file size!");
        tft.fillScreen(TFT_WHITE);
        tft.drawCentreString("Invalid firmware file!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
        firmwareFile.close();
        return false;
    }

    // 🔍 Show current running partition (OS)
    const esp_partition_t* runningPartition = esp_ota_get_running_partition();
    Serial.printf("🟢 Currently running partition: %s @ 0x%08X\n", runningPartition->label, runningPartition->address);

    // 🔍 Get next update partition
    const esp_partition_t* updatePartition = esp_ota_get_next_update_partition(NULL);
    Serial.printf("📝 Target update partition: %s @ 0x%08X\n", updatePartition->label, updatePartition->address);
    Serial.printf("📐 Partition size: %u bytes\n", updatePartition->size);

    // 🖥️ Prepare TFT screen
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(FONT_SIZE);
    tft.setCursor(10, 30);
    tft.println("Update in Progress...");
    tft.setCursor(10, 60);
    tft.println("Writing firmware...");

    // Draw static background bar
    tft.fillRect(0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, 10, TFT_LIGHTGREY);

    esp_ota_handle_t otaHandle;
    esp_err_t err = esp_ota_begin(updatePartition, OTA_SIZE_UNKNOWN, &otaHandle);
    if (err != ESP_OK) {
        Serial.println("❌ esp_ota_begin() failed!");
        tft.drawCentreString("OTA begin failed!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
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
            Serial.println("❌ esp_ota_write() failed!");
            esp_ota_end(otaHandle);
            firmwareFile.close();
            tft.drawCentreString("Write error!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
            return false;
        }
        written += len;

        // 🎯 Update progress bar
        int progress = (written * SCREEN_WIDTH) / fileSize;
        tft.fillRect(0, SCREEN_HEIGHT - 20, progress, 10, TFT_GREEN);

        // 🎯 Update percentage text only if changed
        int percent = (written * 100) / fileSize;
        if (percent != lastPercent) {
            lastPercent = percent;
            char percentStr[10];
            sprintf(percentStr, "%d%%", percent);
            tft.fillRect(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT - 40, 60, 20, TFT_WHITE);  // Clear previous
            tft.setTextColor(TFT_BLACK);
            tft.setCursor(SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT - 40);
            tft.print(percentStr);
        }
    }
    firmwareFile.close();
    Serial.printf("✅ Buffered write complete: %u bytes\n", written);

    err = esp_ota_end(otaHandle);
    if (err != ESP_OK) {
        Serial.println("❌ esp_ota_end() failed!");
        tft.drawCentreString("Update end failed!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
        return false;
    }

    err = esp_ota_set_boot_partition(updatePartition);
    if (err != ESP_OK) {
        Serial.println("❌ Failed to set boot partition!");
        tft.drawCentreString("Boot switch failed!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
        return false;
    }

    Serial.printf("✅ Boot partition set to: %s @ 0x%08X\n", updatePartition->label, updatePartition->address);
    Serial.println("🔁 Restarting to apply update...");
    tft.fillScreen(TFT_WHITE);
    tft.drawCentreString("Update successful! Restarting...", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE);
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
        Serial.println("Other OTA partition not found!");
        return false;
    }

    if (esp_ota_set_boot_partition(other) != ESP_OK) {
        Serial.println("Failed to switch boot partition!");
        return false;
    }

    Serial.printf("Switched to %s. Restarting...\n", other->label);
    delay(1000);
    ESP.restart();
    return true;
}