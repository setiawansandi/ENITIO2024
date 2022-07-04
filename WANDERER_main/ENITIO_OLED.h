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

const unsigned char enitioLogo [] PROGMEM = { 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x1f, 0x80, 0x0c, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0x9f, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x07, 0x9f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0xf8, 0x00, 0x00, 0x78, 0x1f, 0x9f, 0x0f, 0x80, 0x07, 0x8f, 0x80, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x7c, 0x1f, 0x9f, 0x0f, 0x80, 0x7f, 0x8f, 0xf0, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0x07, 0xf8, 0x00, 0x7c, 0x1f, 0x9f, 0x0f, 0x81, 0xff, 0x8f, 0xfc, 0x00, 
  0x01, 0xfc, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x7c, 0x1f, 0x9f, 0x0f, 0x83, 0xfe, 0x07, 0xfe, 0x00, 
  0x01, 0xfc, 0x40, 0x00, 0x00, 0x7f, 0x80, 0x7c, 0x1f, 0x9f, 0x0f, 0x87, 0xf8, 0x00, 0xff, 0x00, 
  0x01, 0xfc, 0x7f, 0xf8, 0x04, 0x1f, 0xe0, 0x7c, 0x1f, 0x9f, 0x0f, 0x87, 0xf0, 0x00, 0x7f, 0x00, 
  0x01, 0xfc, 0x7f, 0xf8, 0x07, 0x07, 0xf8, 0x3c, 0x1f, 0x9f, 0x0f, 0x87, 0xf0, 0x00, 0x3f, 0x00, 
  0x01, 0xfc, 0x7f, 0xf8, 0x07, 0xc1, 0xfe, 0x0c, 0x1f, 0x9f, 0x0f, 0x87, 0xf0, 0x00, 0x7f, 0x00, 
  0x01, 0xfc, 0x00, 0x00, 0x07, 0xe0, 0x7f, 0x80, 0x1f, 0x9f, 0x0f, 0x83, 0xfc, 0x01, 0xfe, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0xc7, 0xe0, 0x1f, 0xe0, 0x1f, 0x9f, 0x0f, 0x81, 0xff, 0x8f, 0xfc, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0xc7, 0xe0, 0x07, 0xf8, 0x1f, 0x9f, 0x0f, 0x80, 0x7f, 0x8f, 0xf8, 0x00, 
  0x01, 0xf8, 0x7f, 0xff, 0xc7, 0xe0, 0x01, 0xfc, 0x1f, 0x9f, 0x0f, 0x80, 0x1f, 0x8f, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xc0, 0x30, 0x30, 0x07, 0xc0, 0x1c, 0x08, 0x1f, 0x02, 0x06, 0x03, 0x83, 0x80, 0x00, 
  0x00, 0x70, 0x60, 0x70, 0x30, 0x1c, 0x78, 0x1f, 0x0c, 0x1f, 0x0e, 0x06, 0x03, 0xef, 0x80, 0x00, 
  0x00, 0x60, 0x30, 0x70, 0x30, 0x38, 0x18, 0x1d, 0xcc, 0x1f, 0x0e, 0x06, 0x03, 0x3d, 0x80, 0x00, 
  0x00, 0x38, 0xe0, 0x38, 0x70, 0x3f, 0xf8, 0x18, 0x7c, 0x1f, 0x07, 0x0e, 0x03, 0x01, 0x80, 0x00, 
  0x00, 0x1f, 0xc0, 0x1f, 0xc0, 0x30, 0x10, 0x08, 0x1c, 0x1f, 0x03, 0xf8, 0x01, 0x01, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xc0, 0xf8, 0x18, 0x70, 0xff, 0x83, 0xfc, 0x1f, 0x02, 0x3f, 0xe1, 0xff, 0x00, 0x00, 
  0x00, 0x60, 0x03, 0x0c, 0x1f, 0xf0, 0xc0, 0x06, 0x00, 0x1f, 0x00, 0x00, 0x60, 0x03, 0x00, 0x00, 
  0x00, 0x67, 0xc3, 0xfe, 0x1b, 0x30, 0xfe, 0x03, 0xf8, 0x1f, 0x00, 0x1f, 0xc0, 0xfe, 0x00, 0x00, 
  0x00, 0x3f, 0xc3, 0x04, 0x18, 0x30, 0xff, 0x87, 0xf8, 0x0f, 0x00, 0x1f, 0xf1, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

void StartUpDisplay(){
  display.clearDisplay();
  display.setTextSize(1); // Draw SIZE

  display.setTextColor(SSD1306_WHITE); 

  display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE); 

  display.setCursor(0, 56);
  display.println("   Please wait ...   ");
  display.display();
}

class MainMenu_OLED {
  private:

  public:
    void display_menu(int ProcessNav){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE); 
  
      display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE); 

      switch (ProcessNav) {
        case MainMenuProcess:
          display.setCursor(26, 56);
          display.println(F("< Main Menu >"));
          break;

        case ProfileProcess:
          display.setCursor(34, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Profile"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case TreasureHuntProcess:
          display.setCursor(15, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Treasure Hunt"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case SnakeGameProcess:
          display.setCursor(40, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Snake"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case MusicProcess:
          display.setCursor(40, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Music"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case FactoryResetProcess:
          display.setCursor(15, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Factory Reset"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case CreditProcess:
          display.setCursor(37, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Credit"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;
      }
      display.display();
    }
};

MainMenu_OLED MainMenu_OLED;

class Profile_OLED {
  private:

  public:
    void display_CompleteProfilePage(int OG, int isGL, String name){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("     Your Profile    ")); 

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 12);
      display.print("Name: ");
      display.println(name);

      display.setCursor(0, 24);

      if (isGL == 0){
        display.println("Freshman"); 
      }
      else {
        display.println("Group Leader");
      }

      display.setCursor(0, 36);
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

      display.setCursor(10, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to go back)"); 
      display.display();
    }

    void display_OGregisteringPage(int OG_pointer){
      display.clearDisplay();
      // change text display here
      display.setTextSize(1);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);

      display.setCursor(0,0);
      // menu title
      display.println(" Choose your OG...   ");
      //---------------------------------

      display.setTextColor(SSD1306_WHITE);

      display.setCursor(10, 12);
      
      display.println("ALATAR");

      display.setCursor(10, 22);
      
      display.println("DRACHEN");

      display.setCursor(10, 32);
      
      display.println("EVA");

      display.setCursor(10, 42);
      
      display.println("INVICTA");
      
      // prints the cursor to highlight menu items
      display.setCursor(2, (OG_pointer * 10) + 12);
      display.println(">");

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)"); 

      display.display();
    }

    void display_isGLregisteringPage(int isGL_pointer){
      display.clearDisplay();
      // change text display here
      display.setTextSize(1);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);

      display.setCursor(0,0);
      // menu title
      display.println(" Are you a GL?       ");
      //---------------------------------

      display.setTextColor(SSD1306_WHITE);

      display.setCursor(10, 12);
      
      display.println("NO");

      display.setCursor(10, 22);
      
      display.println("YES");
      
      // prints the cursor to highlight menu items
      display.setCursor(2, (isGL_pointer * 10) + 12);
      display.println(">");

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)"); 

      display.display();
    }
};

Profile_OLED Profile_OLED;

const int mainPage = 0;
const int powerupPage = 1;
const int infoPage = 2;
const int achievementPage = 3;
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

         case powerupPage:
          display.setCursor(30, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Power-Up"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
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

    void display_powerupPage(){
      
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

          case powerupPage:
            display.setCursor(30, 56);
            display.print(F("< "));
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(F("Power-Up"));
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

    void display_WaitingPage(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("  Treasure Hunt Game ")); 

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 24);
      display.println("   The game was not  ");
      display.println("     started yet.    "); 

      display.setCursor(10, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to go back)"); 
      display.display();
    }
};

TreasureHunt_OLED TreasureHunt_OLED;
