#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...

// the font height for font size 1 is 8
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int mainPage = 0;
const int infoPage = 1;
const int achievementPage = 2;
const int leaderboardPage = 3;
const int exitPage = 4;

class TreasureHunt_OLED {
  private:
    String lastNoti;
    
  public:
    
    void display_mainPage(int HP, int En, String noti, int pageNav) {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      if (noti.length() == 0)
      {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("  Treasure Hunt Game ")); 
      }
      else 
      {
        display.setTextColor(SSD1306_WHITE); 
        display.setCursor(0, 0);
        display.println(noti); 
      }
      
    
      display.setCursor(0, 12);
      display.setTextSize(2);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
    
      int i;
      if (HP <= 12){
        display.println("HP ");
        for (i = 0; i < HP; i ++) {
          display.fillRect(34+i*8, 12, 4, 16, SSD1306_WHITE);
        }
      }
      else {
        display.print("HP ");
        display.println(HP);
      }
            
      display.setCursor(0, 32);
    
      if (En <= 12) {
        display.println("EN ");
        for (i = 0; i < En; i ++) {
          display.fillRect(34+i*8, 32, 4, 16, SSD1306_WHITE);
        }
      }
      else {
        display.print("EN ");
        display.println(En);
      }
    
      display.setTextSize(1);

      switch (pageNav){
        case mainPage:
          display.setCursor(44, 56);
          display.println(F("< Main >"));
          break;

         case infoPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Info"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

         case achievementPage:
          display.setCursor(20, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Achievement"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

         case leaderboardPage:
          display.setCursor(20, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Leaderboard"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

         case exitPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Exit"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;
      }
      display.display();
    }

    void display_infoPage(int OG, int ID, int MANA, int MaxEn, String noti, int pageNav)
    {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      if (noti.length() == 0)
      {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("  Treasure Hunt Game ")); 
      }
      else 
      {
        display.setTextColor(SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(noti); 
      }

        display.setCursor(0, 10);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text      

        switch (OG){
          case ALATAR:
            display.println("OG: Alatar ");
            break;
          case DRACHEN:
            display.println("OG: Drachen ");
            break;
          case EVA:
            display.println("OG: Eva ");
            break;
          case INVICTA:
            display.println("OG: Invicta ");
            break;
        }

        display.setCursor(0, 20);
        display.print("ID: ");
        display.println(ID);

        display.setCursor(0, 30);
        display.print("MANA: ");
        display.println(MANA);

        display.setCursor(0, 40);
        display.print("Max En: ");
        display.println(MaxEn);

        switch (pageNav){
        case mainPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Main"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;
          
         case infoPage:
          display.setCursor(44, 56);
          display.println(F("< Info >"));
          break;

         case achievementPage:
          display.setCursor(20, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Achievement"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

         case leaderboardPage:
          display.setCursor(20, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Leaderboard"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

         case exitPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Exit"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;
      }
      display.display();
    }

    void display_achievementPage(int numKilled, int numL1Treasure, int numL2Treasure, String noti, int pageNav){};
    void display_leaderboardPage(){};
    void display_notStartYet(){};
};

TreasureHunt_OLED TreasureHunt_OLED;
