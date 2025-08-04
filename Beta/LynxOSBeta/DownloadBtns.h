#ifndef DOWNLOADBTNS_H
#define DOWNLOADBTNS_H

#include "SetupProgram.h"
#include "WIFIFunc.h"
#include <vector>


struct Entry {
    String url;
    String displayName;
    String downloadName;
    bool isOS;
};


class DownloadBtns
{
  public:
    DownloadBtns();
    static std::vector<Entry> parseManifest(const String& content);

    void begin(TFT_eSPI* _tft, fs::FS* _sd);
    void downloadTxtAndParse(String path, String downloadname);
    void displayOptions(); // just draws the items on teh screen leaves actually figuiring out what to do to the handleInput
    void handleInput(); //Uses the bool to figure out whether to run the display options again with the new set of items or to ask the user to save and flash, just flash or just save

  private:
    TFT_eSPI* tft;
    fs::FS* sd;
    bool page = false; //0 = displaying version selector (do not flash file because it is just a txt), 1 = displaying actual bin files (do ask flash if user wants)
    std::vector<Entry> Entries; // holds teh entries so that the display options can show the name and the url and download things for the handle input

};

#endif