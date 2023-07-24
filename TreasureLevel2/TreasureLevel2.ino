#include <EEPROM.h>
#include "ENITIO_ir.h"
#include "ENITIO_enums.h"
#include "ENITIO_EspNOW.h"
#include "ENITIO_NeoPixel.h"
#include "ENITIO_joystick.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define R_ON 255
#define G_ON 255
#define B_ON 255

int id;
int num_poison;

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

void StartUpDisplay() {
  display.clearDisplay();
  display.setTextSize(1); // Draw SIZE

  display.setTextColor(SSD1306_WHITE);

  display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE);

  display.setCursor(0, 56);
  display.println("   Please wait ...   ");
  display.display();
};

#define EEPROM_SIZE 20

#define hp_pxl_bar_width 2 // [pxl/HP]

#define ENABLE_add 0 // 0 means Treasure has not been initialized, 1 means already initialized
#define ID_add 1
#define HP_add 2
#define collectedCLAN_add 3
#define collectedID_add 4
#define uploadSuccess_add 5
#define ONLINE_mode_add 6

#define DOUBLE_CLICK_LENGTH 1000 // [ms]

int TREASURE_LEVEL2_INITIAL_HP;
int NUM_L2TREASURES;
int TREASURE_LEVEL2_ACTION_RECV_WAIT;
int TREASURE_LEVEL2_RECOVER_PERIOD;
int TREASURE_POISON_THRESHOLD;
bool parameters_updated = false;
bool WIFI_ON;

class TreasureLevel2 {
  private:
    int HP;
    bool _isPoison = true;

    unsigned long lastRecoveredTime = 0;
    unsigned long lastActionReceived = 0;
    unsigned long timeleftToRecover = 0;

  public:
    int CLAN_, ID_, En_, MULTIPLIER_, action_;

    void setup_initial_state() {
      if (id > TREASURE_POISON_THRESHOLD) _isPoison = true;

      if (EEPROM.read(ENABLE_add) == 0) {
        EEPROM.write(ENABLE_add, 1);
        EEPROM.write(HP_add, TREASURE_LEVEL2_INITIAL_HP);
        EEPROM.commit();
        HP = TREASURE_LEVEL2_INITIAL_HP;
      } else {
        HP = EEPROM.read(HP_add);
        if (HP == 0) CLAN_ = EEPROM.read(collectedCLAN_add);
      }

      if (HP == 0) {
        TreasureLevel2_NeoPixel.off_FRONT();
        TreasureLevel2_NeoPixel.off_TOP();
      } else {
        TreasureLevel2_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
        TreasureLevel2_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
      }
    }

    void feedback_collectL2(int CLAN_, int ID_, int channel_) {
      bool killed = (HP == 0);
      int powerup_received;
      if (killed && _isPoison) powerup_received = 6;
      else powerup_received = 0;
      TreasureLevel2_EspNOW.send_data(3, CLAN_, ID_, id, killed, powerup_received, channel_);
    } ;

    void receiveAction() {
      int channel_;
      if (HP > 0) {
        unsigned long currTime = millis();
        if (TreasureLevel2_IR.available()) {
          ir_signal IRsignal_ = TreasureLevel2_IR.read();

          if (currTime - lastActionReceived > TREASURE_LEVEL2_ACTION_RECV_WAIT) {
            Serial.printf("RECV %d %d %d %d | %d %d %d %d \n", IRsignal_.address.digit3, IRsignal_.address.digit2, IRsignal_.address.digit1, IRsignal_.address.digit0, IRsignal_.command.digit3, IRsignal_.command.digit2, IRsignal_.command.digit1, IRsignal_.command.digit0);
            CLAN_ = IRsignal_.address.digit2;
            ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);

            channel_ = IRsignal_.command.digit2;
            MULTIPLIER_ = IRsignal_.command.digit1;
            action_ = IRsignal_.command.digit0;

            lastActionReceived = currTime;

            if (action_ == collect) {
              HP = max(HP - MULTIPLIER_, 0);
              feedback_collectL2(CLAN_, ID_, channel_);
              EEPROM.write(HP_add, HP);
              EEPROM.commit();
              Serial.printf("Current HP: %d\n", HP);
              unsigned long currTime = millis();
              lastRecoveredTime = currTime;
              if (HP == 0) handle_Collected();
            }
          }
        }
      }
    }

    void recover() {
      if (HP == TREASURE_LEVEL2_INITIAL_HP) {
        lastRecoveredTime = millis();
      }
      else if (HP > 0) {
        unsigned long currTime = millis();
        timeleftToRecover = max(int(TREASURE_LEVEL2_RECOVER_PERIOD - (currTime - lastRecoveredTime)), 0);
        if (timeleftToRecover == 0) {
          HP++;
          lastRecoveredTime = currTime;
          EEPROM.write(HP_add, HP);
          EEPROM.commit();
        }
      }
      else if (HP == 0) {
        // do nothing
      }
    }

    void handle_Collected() {
      interim_collected_display();
      TreasureLevel2_NeoPixel.off_FRONT();
      TreasureLevel2_NeoPixel.off_TOP();

      // no need to feedback everytime the player collecting the Treasure. Only feedback to the server and to the player when the treasure is fully collected, ie. HP == 0
      Serial.printf("CLAN: %d ID: %d\n", CLAN_, ID_);
      // this code to save the info of the player collected the treasure to resend if required
      EEPROM.write(collectedCLAN_add, CLAN_);
      EEPROM.write(collectedID_add, ID_);
      EEPROM.commit();

      // upload to server
      if (WIFI_ON) {
        dbc.connectToWiFi();
        int curr_upload_fail_counter = wifi_timeout_or_refused_counter;
        String treasureName = "TREASURE" + String(id);
        String player_mac_address = dbc.setTreasureAsOpened(treasureName, CLAN_, ID_);
        int new_upload_fail_counter = wifi_timeout_or_refused_counter;
        if (new_upload_fail_counter == curr_upload_fail_counter) {
          EEPROM.write(uploadSuccess_add, 1);
          EEPROM.commit();
        }
      }
    }

    void resend_treasure_collection() {
      int upload_status = EEPROM.read(uploadSuccess_add);
      if (HP == 0 && upload_status == 0) {
        CLAN_ = EEPROM.read(collectedCLAN_add);
        ID_ = EEPROM.read(collectedID_add);
        String treasureName = "TREASURE" + String(id);
        String returned_player_mac_address = dbc.setTreasureAsOpened(treasureName, CLAN_, ID_);
        String player_mac_address = "04:08:01:0" + String(CLAN_) + ":" + String(ID_ / 16) + String(ID_ % 16) + ":00";
        if (player_mac_address == returned_player_mac_address) {
          EEPROM.write(uploadSuccess_add, 1);
          EEPROM.commit();
        }
      }
    }

    void display_not_playing_yet() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 2 Treasure  "));

      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("      Hold on!!     ");
      display.setCursor(0, 30);
      display.println("  The game has not  ");
      display.println("    started yet.    ");
      display.display();
    };

    void interim_collected_display() {
      display.clearDisplay(); \
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 2 Treasure  "));

      display.setCursor(0, 14);
      display.setTextSize(2);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.print("HP ");

      display.setCursor(110, 28);
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.println(HP);

      display.setCursor(0, 45);
      display.println("    Please wait ...   ");
      display.display();
    }

    void display_in_game() {
      if (HP != 0) {
        display.clearDisplay(); \
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 2 Treasure  "));

        display.setCursor(0, 14);
        display.setTextSize(2);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.print("HP ");
        display.fillRect(0, 34, HP * hp_pxl_bar_width, 4, SSD1306_WHITE);

        display.setCursor(110, 28);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.println(HP);

        display.setCursor(0, 45);
        if (timeleftToRecover > 0) {
          display.print("Recover in: ");
          display.println(int(timeleftToRecover / 1000.0) + 1);
        }

        if (!WIFI_ON) {
          display.setCursor(0, 56);
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.println("    OFFLINE MODE    ");
        }
        display.display();
      }
      else {
        display.clearDisplay(); \
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 2 Treasure  "));

        display.setCursor(0, 12);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("  Congratulations!!  ");

        display.setCursor(0, 23);
        if (_isPoison) display.println("   It's a Poison!   ");
        else display.println("  It's a Treasure!  ");

        display.setCursor(0, 36);
        display.println("   Clan Collected:  ");

        display.setCursor(0, 46);
        if (CLAN_ == INVICTA)
          display.println("      INVICTA       ");
        else if (CLAN_ == DYNARI)
          display.println("       DYNARI       ");
        else if (CLAN_ == EPHILIA)
          display.println("      EPHILIA       ");
        else if (CLAN_ == AKRONA)
          display.println("       AKRONA       ");
        else if (CLAN_ == SOLARIS)
          display.println("      SOLARIS       ");

        if (!WIFI_ON) {
          display.setCursor(0, 56);
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.println("    OFFLINE MODE    ");
        }
        display.display();
      };
    }
};

TreasureLevel2 Treasure;
bool gameStarted = 0;
bool initiateTreasureDone = 0;

int get_game_state() {
  // retrieve game state from server
  // 0 mean game did not start
  // 1 mean in game

  if (!parameters_updated) {
    Serial.println("[BACKGROUND] Connecting to WiFi to retrieve latest parameters..");
    bool isWiFiConnected = dbc.connectToWiFi();
    if (isWiFiConnected) {
      GAME_CONSTANTS game_consts = dbc.getGameConstants();
      HTTP_TIMEOUT = game_consts.HTTP_TIMEOUT;
      TREASURE_LEVEL2_INITIAL_HP = game_consts.TREASURE_LEVEL2_INITIAL_HP;
      NUM_L2TREASURES = game_consts.NUM_L2TREASURES;
      TREASURE_LEVEL2_ACTION_RECV_WAIT = game_consts.TREASURE_LEVEL2_ACTION_RECV_WAIT;
      TREASURE_LEVEL2_RECOVER_PERIOD = game_consts.TREASURE_LEVEL2_RECOVER_PERIOD;
      TREASURE_POISON_THRESHOLD = game_consts.TREASURE_POISON_THRESHOLD;
      parameters_updated = true;
      Serial.println("[BACKGROUND] Parameters Retrieved");
    } else {
      Serial.println("[BACKGROUND] Update Unsuccessful");
      return 0;
    }
  }
  if (gameStarted) return 1;  // once the game has started then we dunnid to check anymore
  else {
    gameStarted = dbc.hasGameStarted();
    if (gameStarted) {
      Serial.println("[BACKGROUND] Game has started! Initialising Treasure..");
      Treasure.setup_initial_state();
      initiateTreasureDone = 1;
      WiFi.disconnect();
    }
    return gameStarted;
  }
}

bool AdminFunction = false;
bool clicked_once = 0;
unsigned long last_clicked = 0;

#include "Admin.h"

void handleJoystick() {
  joystick_pos joystick_pos = TreasureLevel2_joystick.read_Joystick();
  if (TreasureLevel2_joystick.get_state() == 0) {
    switch (joystick_pos) {
      case button:
        if (clicked_once) {
          AdminFunction = true;
          clicked_once = 0;
        }
        else {
          clicked_once = 1;
          last_clicked = millis();
        }
        TreasureLevel2_joystick.set_state();
        break;

      case idle:
        if (clicked_once) {
          unsigned long currTime = millis();
          if (currTime - last_clicked > DOUBLE_CLICK_LENGTH) clicked_once = 0;
        }
        break;

      default:
        if (clicked_once) {
          unsigned long currTime = millis();
          if (currTime - last_clicked > DOUBLE_CLICK_LENGTH) clicked_once = 0;
        }
        TreasureLevel2_joystick.set_state();
        break;
    }
  }
}

TaskHandle_t backgroundTask;

void backgroundTaskCode(void * pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for ( ; ; ) {
    if (!WIFI_ON) {
      // offline mode
      if (!initiateTreasureDone) {
        Treasure.setup_initial_state();
        initiateTreasureDone = 1;
      }
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    } else {
      get_game_state();
      delay(10000);
    }
  }
};

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  StartUpDisplay();
  Serial.begin(115200);
  TreasureLevel2_IR.enable();
  TreasureLevel2_NeoPixel.initialize();
  TreasureLevel2_EspNOW.enable();

  EEPROM.begin(EEPROM_SIZE);
  id = EEPROM.read(ID_add);
  WIFI_ON = EEPROM.read(ONLINE_mode_add);

  // Hardcoded constants, in case there is no WiFi to update
  HTTP_TIMEOUT = 30000;
  TREASURE_LEVEL2_INITIAL_HP = 50;
  NUM_L2TREASURES = 20;
  TREASURE_LEVEL2_ACTION_RECV_WAIT = 150;
  TREASURE_LEVEL2_RECOVER_PERIOD = 20000;
  TREASURE_POISON_THRESHOLD = 17;

  xTaskCreatePinnedToCore(
    backgroundTaskCode,   /* Task function. */
    "backgroundTask",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    2,           /* priority of the task */
    &backgroundTask,      /* Task handle to keep track of created task */
    0);
}

void loop() {
  switch (initiateTreasureDone) {
    case 0:
      if (!AdminFunction) {
        handleJoystick();
        Treasure.display_not_playing_yet();
      }
      else {
        TreasureLevel2_Admin.AdminLoop();
      }
      break;
    case 1:
      if (!AdminFunction) {
        handleJoystick();
        Treasure.display_in_game();
      }
      else {
        TreasureLevel2_Admin.AdminLoop();
      }
      Treasure.receiveAction();
      Treasure.recover();
      if (!WIFI_ON) Treasure.resend_treasure_collection();
      break;
  }
}
