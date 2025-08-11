# 🚀 MTOS (Alpha) v1

This is the **Alpha release** of a modular ESP32 firmware loader featuring a touchscreen interface, SD card access, and OTA update support. Built for experimentation, debugging, and refinement.

---

## ✨ Alpha Features

- 📁 **SD Card Firmware Loading**  
  Reads `.bin` files from SD card via SPI and prepares them for flashing.

- 🖱️ **Touchscreen UI**  
  Basic touch-driven interface with centered layout and minimal navigation.

- 🔄 **OTA Update Support**  
  Accepts direct URLs to firmware binaries and flashes them over-the-air.

- 📊 **Download Progress Screen**  
  Displays real-time progress with percentage and visual feedback.

- 🧪 **Debug-Friendly Architecture**  
  Modular codebase with clear separation of display, input, and update logic.

---

## ⚙️ How It Works

1. **Initialization**  
   - Sets up TFT display, touchscreen, and SD card on shared SPI bus.  
   - Handles SPI conflicts and calibrates peripherals.

2. **Boot Menu**  
   - Presents options to load firmware from SD or initiate OTA update.  
   - Touch input drives selection.

3. **Firmware Flashing**  
   - Loads selected `.bin` file into memory.  
   - Validates and flashes using OTA partition logic.

4. **Progress Feedback**  
   - Shows download progress with visual indicators.  
   - Handles errors gracefully with retry prompts.

---

## 🧭 Notes

- This Alpha version is intended for testing and refinement.  
- Expect occasional bugs or incomplete features.  
- Feedback and contributions are welcome!
- It does NOT use the v1paths.txt, which is only for the beta and later versions.
