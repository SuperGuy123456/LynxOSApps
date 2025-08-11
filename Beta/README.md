# 🚀 MTOS (Beta) v2
This is the Beta release of LynxOS—an ESP32 firmware loader featuring refined touchscreen controls, SD card scanning, OTA update support, and a startup selector. Built for smoother UX and scalable embedded workflows.

___

## ✨ Beta Features

- 📁 SD Card Firmware Scanning
Automatically detects .bin files and presents them in a boot menu for direct flashing.
- 🖱️ Touchscreen Multi-Button UI
Startup screen includes options for Wi-Fi Download and Boot Menu access.
- 🔄 OTA Update Support
Loads firmware over-the-air from pre-defined URLs stored on SD and flashes it securely.
- ⚡ Improved SPI Coordination
Manual chip-select toggling for SD and touch components prevents bus conflicts.
- 🔧 Modular System Structure
Encapsulated logic across setup, Wi-Fi, downloader, and boot menu classes.

___

## ⚙️ How It Works

- Startup Selection
- Two touchscreen buttons: Wi-Fi Download and Boot Menu.
- Touch detection maps raw input to screen coordinates and debounces interaction.
- Wi-Fi Download Mode
- Initializes Wi-Fi and reads OTA download paths from v1paths.txt.
- Begins firmware download with visual progress feedback.
- Boot Menu Mode
- Scans SD card for valid .bin files and presents flash options.
- Uses FlashButtonManager to coordinate user interaction and file management.
- Touch Input & SPI Handling
- Touch and SD share SPI bus—CS toggling avoids contention.
- Peripherals are initialized cleanly based on selected mode.
  
___

## 🧭 Notes

- switchPartition() has been removed in this version.
- OTA paths must be defined in the vpaths files on the SD card.
- This version is reliable for day-to-day flashing and update cycles.
- Expect smoother behavior across SPI and fewer initialization issues than Alpha.
