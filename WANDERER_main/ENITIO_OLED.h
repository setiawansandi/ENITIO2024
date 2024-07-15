#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

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
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x40, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x60, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf1, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf2, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe4, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x88, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x90, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf8, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf9, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x80, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x41, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x80, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x80, 0x00, 0x00, 0x00, 0x07, 0xf8, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xfc, 0x00, 0x01, 0xfc, 0x9e, 0x3f, 0xff, 0xff, 0xe7, 0xf9, 0x3f, 0xff, 0xe0, 
	0x1f, 0xff, 0xff, 0xff, 0x00, 0x03, 0xfd, 0xfe, 0xff, 0xff, 0xff, 0xef, 0xf1, 0xff, 0xff, 0xfc, 
	0x7f, 0xff, 0xff, 0x9f, 0xc0, 0x03, 0xfd, 0xfe, 0xff, 0xff, 0xff, 0x9f, 0xe7, 0xff, 0xff, 0xfe, 
	0xff, 0xff, 0xff, 0x1f, 0xf0, 0x03, 0xfd, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0xef, 0xff, 0xff, 0xff, 
	0x7f, 0xff, 0xfc, 0x3f, 0xfc, 0x03, 0xfd, 0xfe, 0xff, 0xff, 0xfc, 0x7f, 0xcf, 0xff, 0xff, 0xff, 
	0x7f, 0x00, 0x00, 0xbf, 0xff, 0x03, 0xfd, 0xfe, 0x40, 0x7f, 0x01, 0x7f, 0x9f, 0xe0, 0x00, 0x7f, 
	0x7f, 0x10, 0x00, 0x1f, 0xff, 0x83, 0xfd, 0xfe, 0x00, 0xff, 0x40, 0xff, 0x9f, 0xe8, 0x01, 0x7f, 
	0x7f, 0xff, 0xfc, 0x1f, 0xff, 0xe3, 0xfd, 0xfe, 0x00, 0xff, 0x40, 0xff, 0x2f, 0xe8, 0x01, 0x7f, 
	0xff, 0xff, 0xf8, 0x9f, 0xff, 0xfb, 0xfd, 0xfe, 0x00, 0xff, 0x41, 0xfe, 0x47, 0xe8, 0x01, 0x7f, 
	0xff, 0xff, 0xf8, 0x9f, 0xcf, 0xff, 0xfd, 0xfe, 0x00, 0xff, 0x43, 0xfc, 0x47, 0xe8, 0x01, 0x7f, 
	0xff, 0x00, 0x00, 0x9f, 0xc7, 0xff, 0xfd, 0xfe, 0x00, 0xff, 0x43, 0xfc, 0x87, 0xe0, 0x01, 0x7f, 
	0x7f, 0xff, 0xf8, 0x9f, 0xc5, 0xff, 0xfd, 0xfe, 0x00, 0xff, 0x47, 0xf9, 0x07, 0xf7, 0xff, 0x7f, 
	0xff, 0xff, 0xfe, 0x9f, 0xc4, 0x7f, 0xfd, 0xfe, 0x40, 0xff, 0x4f, 0xf1, 0x07, 0xff, 0xff, 0xff, 
	0x7f, 0xff, 0xff, 0xbf, 0xc4, 0x3f, 0xfd, 0xf8, 0x80, 0xff, 0x5f, 0xf2, 0x07, 0xff, 0xff, 0xff, 
	0x3f, 0xff, 0xff, 0xff, 0xc4, 0x0f, 0xfd, 0xe3, 0x00, 0xff, 0x1f, 0xe4, 0x03, 0xff, 0xff, 0xf8, 
	0x07, 0xff, 0xff, 0xff, 0xc4, 0x03, 0xfd, 0x84, 0x00, 0xff, 0x7f, 0xc4, 0x00, 0xff, 0xff, 0xe4, 
	0x00, 0x00, 0x00, 0x08, 0x0c, 0x00, 0xfc, 0x90, 0x00, 0xff, 0x7f, 0xc8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x80, 0x00, 0xfe, 0xff, 0x90, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x80, 0x00, 0xfc, 0xff, 0x20, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0xf3, 0xff, 0x20, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0xfe, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0x00, 0xfc, 0x07, 0x60, 0x38, 0x13, 0xfc, 0x9f, 0xc0, 0xee, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x37, 0x00, 0xd8, 0x07, 0x60, 0x18, 0x07, 0xf8, 0x96, 0x80, 0x6e, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0c, 0x01, 0xf0, 0x07, 0xe0, 0x18, 0x0f, 0xf9, 0x06, 0x00, 0xfc, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x39, 0x00, 0xcc, 0x06, 0xe0, 0x18, 0x07, 0xf2, 0x06, 0x00, 0xec, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0x80, 0xfc, 0x06, 0x60, 0x38, 0x07, 0xc6, 0x07, 0x00, 0xee, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const int SetGLFunction = 0;
const int FactoryResetFunction = 1;
const int ToggleServerConnectivityFunction = 2;
const int OverwritePlayerIDFunction = 3;
const int CheckScore = 4;
const int ExitFunction = 5;

void StartUpDisplay() {
  display.clearDisplay();
  display.setTextSize(1); // Draw SIZE

  display.setTextColor(SSD1306_WHITE);

  display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE);

  display.setCursor(0, 56);
  display.println("   Please wait ...   ");
  display.display();
}

void CreditsDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setCursor(0, 0);
  display.println(F("  ENITIO Tech Team  "));

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1); // Draw SIZE
  display.setCursor(0, 10);
  display.println("Quan Tran   Ming Yuan");
  display.setCursor(0, 19);
  display.println("Kah Leong     Shirsho");
  display.setCursor(0, 28);
  display.println("Siddhangana      Yash");
  display.setCursor(0, 37);
  display.println("Edmund      Yan Sheng");
  display.setCursor(0, 46);
  display.println("              Winston");

  display.setCursor(10, 56);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.println("(Press to go back)");
  display.display();
}

void MACAddressDisplay()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setCursor(0, 0);
  display.println(F(" Device MAC Address "));
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1); // Draw SIZE
  display.setCursor(0, 22);
  // char mac_str[18];
  // sprintf(mac_str, "MAC: %02X:%02X:%02X:%02X:%02X:%02X", my_MAC_address[0], my_MAC_address[1], my_MAC_address[2], my_MAC_address[3], my_MAC_address[4], my_MAC_address[5]);
  display.println(my_MAC_address);
  display.setCursor(10, 56);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.println("(Press to go back)");
  display.display();
}

class Admin_OLED {
  private:

  public:
    void WrongPwdDisplay() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 28);

      display.println("    Wrong Password   ");
      display.display();
    }

    void RejectRegisterGLDisplay() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 22);

      display.println("Unable to Register GL");

      display.setCursor(0, 34);
      display.println("  after game started ");
      display.display();
    }

    void ConfirmSettingGLDisplay(int isGL) {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 22);

      display.println("    Registered as    ");

      display.setCursor(0, 34);
      if (isGL) display.println("     Group Leader    ");

      else display.println("       Freshman      ");
      display.display();
    }

    void ConfirmPlayerIDDisplay(int clan, int currentIDnum) {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 20);
      display.println("    Registered as   ");

      display.setCursor(0, 30);
      switch (clan) {
        case INVICTA:
          display.println("    CLAN: Invicta   ");
          break;
        case DYNARI:
          display.println("    CLAN: Dynari    ");
          break;
        case EPHILIA:
          display.println("    CLAN: Ephilia   ");
          break;
        case AKRONA:
          display.println("    CLAN: Akrona    ");
          break;
        case SOLARIS:
          display.println("    CLAN: Solaris   ");
          break;
      }

      display.setCursor(0, 40);
      display.printf("        ID %d      ", currentIDnum);
      
      display.display();
    }

    void ConfirmConnectivityToggle(int connectivityState) {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 22);

      display.println(" Set WANDERER Mode: ");

      display.setCursor(0, 34);
      if (connectivityState) display.println("       Online       ");
      else display.println("       Offline      ");
      display.display();
    }

    void display_EnteringPwd(int currentDigitIndex, int currentDigit, int enteringPwdNav, int *user_key_in_pwd) {
      display.clearDisplay();

      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("Please Enter Password"));

      int i;
      for (i = 0; i < 6; i++) {
        display.setCursor(5 + 15 * (i + 1), 28);
        if (currentDigitIndex < i) {
          display.setTextColor(SSD1306_WHITE);
          display.print("*");
        }
        else if (currentDigitIndex == i) {
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(currentDigit);
        }
        else {
          display.setTextColor(SSD1306_WHITE);
          display.print(user_key_in_pwd[i]);
        }
      }

      display.setCursor(0, 56);
      switch (enteringPwdNav)
      {
        case 0:
          display.setTextColor(SSD1306_WHITE);
          display.print(" ");
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print("Back to Main Menu");
          display.setTextColor(SSD1306_WHITE);
          display.print(" >");
          break;

        case 1:
          display.setTextColor(SSD1306_WHITE);
          display.print(" <      Enter      > ");
          break;

        case 2:
          display.setTextColor(SSD1306_WHITE);
          display.print(" <          ");
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print("Backspace");
          break;

        default:
          break;
      }

      display.display();
    }

    void display_MainAdmin(int FunctionNav) {
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("        Admin        "));

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 10);
      display.println("Register Role");
      display.setCursor(10, 19);
      display.println("Factory Reset");
      
      display.setCursor(10, 28);
      if (EEPROM.read(ONLINE_mode_add) == 0) {
        display.println("Set Online Mode");
      } else display.println("Set Offline Mode");
      
      display.setCursor(10, 37);
      display.println("Overwrite ID");

      display.setCursor(10, 46);
      display.println("Check Scores");

      display.setCursor(10, 55);
      display.println("Back to Main Menu");

      switch (FunctionNav)
      {
        case SetGLFunction:
          display.setCursor(2, 10);
          display.println(">");
          break;

        case FactoryResetFunction:
          display.setCursor(2, 19);
          display.println(">");
          break;

        case ToggleServerConnectivityFunction:
          display.setCursor(2, 28);
          display.println(">");
          break;

        case OverwritePlayerIDFunction:
          display.setCursor(2, 37);
          display.println(">");
          break;

        case CheckScore:
          display.setCursor(2, 46);
          display.println(">");
          break;

        case ExitFunction:
          display.setCursor(2, 55);
          display.println(">");
          break;

        default:
          break;
      }

      // display.setCursor(14, 56);
      // display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      // display.println("(Press to choose)");

      display.display();
    }

    void display_ConfirmingReset(int ConfirmingResetNav) {
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F(" Confirm to Reset?   "));
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 12);
      display.println("NO");
      display.setCursor(10, 22);
      display.println("YES");

      switch (ConfirmingResetNav)
      {
        case 0:
          display.setCursor(2, 12);
          display.println(">");
          break;

        case 1:
          display.setCursor(2, 22);
          display.println(">");
          break;

        default:
          break;
      }

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)");

      display.display();
    }

    void display_SettingGL(int isGLNav) {
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F(" Choose your Role... "));
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 12);
      display.println("Freshman (default)");
      display.setCursor(10, 22);
      display.println("Group Leader");

      switch (isGLNav)
      {
        case 0:
          display.setCursor(2, 12);
          display.println(">");
          break;

        case 1:
          display.setCursor(2, 22);
          display.println(">");
          break;

        default:
          break;
      }

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)");

      display.display();
    }

    void display_SettingID(int previousID, int currentIDnum) {
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F(" Setting Player ID "));
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 12);
      display.print("Old ID: "); display.println(previousID);

      display.setCursor(0, 22);
      display.print("New ID: "); 
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println(currentIDnum);

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)");
      
      display.display();
    }

    void display_Checkingscore(int invicta_score, int dynari_score, int ephilia_score,
                                  int akrona_score, int solaris_score) {
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("     Clan Scores     "));

      display.setTextColor(SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(5, 10);
      display.print("Invicta: "), display.println(invicta_score);
      display.setCursor(5, 19);
      display.print("Dynari: "), display.println(dynari_score);
      display.setCursor(5, 28);
      display.print("Ephilia: "), display.println(ephilia_score);
      display.setCursor(5, 37);
      display.print("Akrona: "), display.println(akrona_score);
      display.setCursor(5, 46);
      display.print("Solaris: "), display.println(solaris_score);

      display.setCursor(10, 55);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to go back)");
      display.display();
    }
};

Admin_OLED Admin_OLED;

class MainMenu_OLED {
  private:

  public:
    void display_menu(int ProcessNav) {
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

        case AdminProcess:
          display.setCursor(40, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Admin"));
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

        case MACAddressProcess:
          display.setCursor(15, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("MAC Address"));
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
    void display_CompleteProfilePage(int CLAN, int isGL) {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("     Your Profile    "));

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 12);
      display.println("ENITIO 2024");

      display.setCursor(0, 24);

      if (isGL == 0) {
        display.println("Freshman");
      }
      else {
        display.println("Group Leader");
      }

      display.setCursor(0, 36);
      switch (CLAN) {
        case INVICTA:
          display.println("CLAN: Invicta ");
          break;
        case DYNARI:
          display.println("CLAN: Dynari ");
          break;
        case EPHILIA:
          display.println("CLAN: Ephilia ");
          break;
        case AKRONA:
          display.println("CLAN: Akrona ");
          break;
        case SOLARIS:
          display.println("CLAN: Solaris ");
          break;
      }

      display.setCursor(10, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to go back)");
      display.display();
    }

    void display_CLANregisteringPage(int CLAN_pointer) {
      display.clearDisplay();
      // change text display here
      display.setTextSize(1);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);

      display.setCursor(0, 0);
      // menu title
      display.println(" Choose your CLAN... ");
      //---------------------------------

      display.setTextColor(SSD1306_WHITE);

      display.setCursor(10, 10);

      display.println("INVICTA");

      display.setCursor(10, 19);

      display.println("DYNARI");

      display.setCursor(10, 28);

      display.println("EPHILIA");

      display.setCursor(10, 37);

      display.println("AKRONA");

      display.setCursor(10, 46);

      display.println("SOLARIS");

      // prints the cursor to highlight menu items
      display.setCursor(2, (CLAN_pointer * 9) + 10);
      display.println(">");

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)");

      display.display();
    }

    void interim_registering_display() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("     Your Profile    "));

      display.setCursor(0, 28);
      display.setTextColor(SSD1306_WHITE);
      display.println(F("   Please wait ...   "));
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
      if (HP <= 12) {
        display.println("HP ");
        for (i = 0; i < HP; i ++) {
          display.fillRect(34 + i * 8, 12, 4, 16, SSD1306_WHITE);
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
          display.fillRect(34 + i * 8, 32, 4, 16, SSD1306_WHITE);
        }
      }
      else {
        display.print("EN ");
        display.println(En);
      }

      display.setTextSize(1);

      switch (pageNav) {
        case mainPage:
          display.setCursor(44, 56);
          display.println(F("< Main >"));
          break;

        case powerupPage:
          display.setCursor(26, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Inventory"));
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

    void display_powerupPage(int num_bonus6HP,
                             int num_bonus1MaxEn,
                             int num_bonus1MULTIPLIER,
                             int num_fiveminx2EnRegen,
                             int num_bomb,
                             int num_poison,
                             String noti,
                             int PowerUpNav) {

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

      display.setCursor(0, 12);
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text

      switch (PowerUpNav)
      {
        case bonus6HP:
          display.println((" Give 6 HP instantly "));
          display.setCursor(0, 36);
          display.print(("You have "));
          display.print(num_bonus6HP);
          display.println((" of this!"));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.setCursor(0, 56);
          display.println("    Press to use!    ");
          break;

        case bonus1MaxEn:
          display.println(("  Bonus 1 Max Energy "));
          display.setCursor(0, 36);
          display.print(("You have "));
          display.print(num_bonus1MaxEn);
          display.println((" of this!"));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.setCursor(0, 56);
          display.println("    Press to use!    ");
          break;

        case bonus1MULTIPLIER:
          display.println(("  Bonus 1 MULTIPLIER    "));
          display.setCursor(0, 36);
          display.print(("You have "));
          display.print(num_bonus1MULTIPLIER);
          display.println((" of this!"));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.setCursor(0, 56);
          display.println("    Press to use!    ");
          break;

        case fiveminx2EnRegen:
          display.println((" Double Energy Regen "));
          display.setCursor(0, 22);
          display.println(("       in 5 min      "));
          display.setCursor(0, 36);
          display.print(("You have "));
          display.print(num_fiveminx2EnRegen);
          display.println((" of this!"));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.setCursor(0, 56);
          display.println("    Press to use!    ");
          break;

        case bomb:
          display.println(("        Bomb!!       "));
          display.setCursor(0, 22);
          display.println(("  Damage surrounding "));
          display.setCursor(0, 36);
          display.print(("You have "));
          display.print(num_bomb);
          display.println((" of this!"));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.setCursor(0, 56);
          display.println("    Press to use!    ");
          break;

        case poison:
          display.println(("        Poison!!       "));
          display.setCursor(0, 22);
          display.println(("     Lethal Attack!    "));
          display.setCursor(0, 36);
          display.print(("You have "));
          display.print(num_poison);
          display.println((" of this!"));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.setCursor(0, 56);
          display.println("    Press to use!    ");
          break;

        case 0:
          display.println(("  Use Left and Right "));
          display.setCursor(0, 22);
          display.println(("      to Navigate    "));
          display.setCursor(0, 32);
          display.println(("through each power-up"));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.setCursor(0, 56);
          display.println("  Press to go back!  ");

        default:
          break;
      }
      display.display();
    }

    void display_inventoryPage(int numL1Treasure, String noti, int pageNav)
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

      display.setCursor(20, 16);
      display.print("You are holding");
      display.setCursor(60, 28);
      display.print(numL1Treasure);
      display.setCursor(34, 40);
      display.print("Treasure(s)");

      switch (pageNav) {
        case mainPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Main"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case powerupPage:
          display.setCursor(26, 56);
          // display.print(F("Power-Up"));
          display.print(F("< Inventory >"));
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

    void display_infoPage(int CLAN, int ID, int MULTIPLIER, int MaxEn, String noti, int pageNav)
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

      switch (CLAN) {
        case INVICTA:
          display.println("CLAN: Invicta ");
          break;
        case DYNARI:
          display.println("CLAN: Dynari ");
          break;
        case EPHILIA:
          display.println("CLAN: Ephilia ");
          break;
        case AKRONA:
          display.println("CLAN: Akrona ");
          break;
        case SOLARIS:
          display.println("CLAN: Solaris ");
          break;
      }

      display.setCursor(0, 20);
      display.print("ID: ");
      display.println(ID);

      display.setCursor(0, 30);
      display.print("MULTIPLIER: ");
      display.println(MULTIPLIER);

      display.setCursor(0, 40);
      display.print("Max En: ");
      display.println(MaxEn);

      switch (pageNav) {
        case mainPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Main"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case powerupPage:
          display.setCursor(26, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          // display.print(F("Power-Up"));
          display.print(F("Inventory"));
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

    void display_achievementPage(int numKilled, int numL1Treasure, int numL2Treasure, String noti, int pageNav) {
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

      display.setCursor(0, 12);
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text

      display.print("Killed: ");
      display.println(numKilled);

      display.setCursor(0, 24);
      display.print("L1 Treasure: ");
      display.println(numL1Treasure);

      display.setCursor(0, 36);
      display.print("L2 Treasure: ");
      display.println(numL2Treasure);

      switch (pageNav) {
        case mainPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Main"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case powerupPage:
          display.setCursor(26, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          // display.print(F("Power-Up"));
          display.print(F("Inventory"));
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
          display.println(F("< Achievement >"));
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
    };

    void display_achievementPage_new(int numKilled, int numDeath, int treasure, String noti, int pageNav) {
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

      display.setCursor(0, 12);
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text

      display.print("Killed: ");
      display.println(numKilled);

      display.setCursor(0, 24);
      display.print("Death: ");
      display.println(numDeath);

      display.setCursor(0, 36);
      display.print("Tres. deposited: ");
      display.println(treasure);

      switch (pageNav) {
        case mainPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Main"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case powerupPage:
          display.setCursor(26, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          // display.print(F("Power-Up"));
          display.print(F("Inventory"));
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
          display.println(F("< Achievement >"));
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

    void display_WaitingPage() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("  Treasure Hunt Game "));

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 24);
      display.println("   The game has not  ");
      display.println("     started yet.    ");

      display.setCursor(10, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to go back)");
      display.display();
    }
};

TreasureHunt_OLED TreasureHunt_OLED;
