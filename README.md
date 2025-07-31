# 🦊 LynxOS

## 📖 About the Project

**LynxOS** is essentially a bootloader for the ESP32 that can download `.bin` files from the web (like those hosted in this repo) and store them on a microSD card. Using a touchscreen interface, it displays the available files, allowing the user to select one to flash.

Each `.bin` file follows a generic structure that includes a function to reflash the core OS — a protected `.bin` file that cannot be removed. This project is actively developed and tested on the **ESP32-2432S028R** (commonly known as the "Cheap Yellow Display"), but it should work on any ESP32 with proper configuration.

## 🚀 Trying It Out

Unless you own an **ESP32-2432S028R** with the exact same pin configuration required by this project, you'll need to download the `.ino` file and modify the pin assignments and possibly the libraries.

For those using the ESP32-2432S028R, you can simply download the `.ino` file, flash it to your ESP32, and place the OS `.bin` file onto the microSD card.

## 📌 Features

- Downloads `.bin` files from a remote HTTP/HTTPS server  
- Saves the downloaded file to an SD card  
- Displays available files via touchscreen interface  
- Reads and flashes the selected binary from the SD card  
- Automatically reboots into the new firmware  
- Optional version tracking via JSON  

## 🧰 Requirements

- ESP32-2432S028R ("Cheap Yellow Display")  
- MicroSD card formatted as FAT32 (recommended size < 16GB)  
- Wi-Fi access  
- `.bin` files hosted online  

## 🔄 Update Flow

1. ESP32 connects to Wi-Fi  
2. It downloads the `.bin` file from a specified URL  
3. The file is saved to the SD card  
4. The ESP32 reads the file from SD and flashes it using the `Update` class  
5. If flashing succeeds, the device reboots into the new firmware  

## 🧪 Example Usage

```cpp
const char* binURL = "https://yourdomain.com/firmware/appname.bin";
downloadToSD(binURL, "/update.bin");
flashFromSD("/update.bin");
```

## 🧑‍⚖️ License

**This project is protected by the Creative Commons Attribution-NonCommercial 4.0 International License**

*© 2025 Manomay*
____
