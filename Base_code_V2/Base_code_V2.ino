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

#define EEPROM_SIZE 20

#define ID_BOUND 50

#define hp_pxl_bar_width 100 // [pxl/HP]

#define ENABLE_add 0  // 0 means Treasure has not been initialized, 1 means already initialized
#define ID_add 1
#define HP_add 2
#define INVICTA_add 3
#define DYNARI_add 4
#define EPHILIA_add 5
#define AKRONA_add 6
#define SOLARIS_add 7
#define ONLINE_mode_add 8

#define R_ON 0
#define G_ON 0
#define B_ON 0
#define DOUBLE_CLICK_LENGTH 1000 // [ms]

bool AdminFunction = false;
int ID;

int failed_kill_feedback = 0;
int failed_kill_CLAN[50] = {};
int failed_kill_ID[50] = {};
int current_failed_save_pointer = 0;
int current_failed_read_pointer = 0;
bool uploadInProgress = 0;  // boolean required to ensure treasure doesn't recover while WiFi is transmitting
bool can_upload_fail = 1;
int WIFI_ON;
bool parameters_updated = false;
int AKRONA_TREAS=0;
int INVICTA_TREAS=0;
int DYNARI_TREAS=0;
int EPHILIA_TREAS=0;
int SOLARIS_TREAS=0;
int Treas_Deposit_;
int Treas_Deposit=0;
int action=0;

// stub functions
void StartUpDisplay();
void clearEEPROM();

#include <EEPROM.h>
#include "ENITIO_ir.h"
#include "ENITIO_enums.h"
#include "ENITIO_ESPNOW.h"
#include "ENITIO_NeoPixel.h"
#include "ENITIO_joystick.h"
#include "Admin.h"

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

// Constants
int TREASURE_BASE_INITIAL_HP;
int TREASURE_BASE_ACTION_RECV_WAIT;
int TREASURE_BASE_RECOVER_DURATION;

void StartUpDisplay() {
  display.clearDisplay();
  display.setTextSize(1); // Draw SIZE

  display.setTextColor(SSD1306_WHITE);

  display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE);

  display.setCursor(0, 56);
  display.println("   Please wait ...   ");
  display.display();
};

bool clicked_once = 0;
unsigned long last_clicked = 0;

void handleJoystick() {
  joystick_pos joystick_pos = TreasureBase_joystick.read_Joystick();
  if (TreasureBase_joystick.get_state() == 0) {
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
        TreasureBase_joystick.set_state();
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
        TreasureBase_joystick.set_state();
        break;
    }
  }
}

class TreasureBase
{
  private:
    int HP;

    int this_recover_duration = TREASURE_BASE_RECOVER_DURATION;
    unsigned long lastOpenedTime = 0;
    unsigned long lastActionReceived = 0;

  public:

    int CLAN_, ID_, En_, MULTIPLIER_, action_, channel_;

    void init_treasure() {
      EEPROM.write(ENABLE_add, 1);
      EEPROM.commit();
      HP = TREASURE_BASE_INITIAL_HP;
    };

    void receiveAction() {
      int currStatus = EEPROM.read(ENABLE_add);
      if (TreasureBase_IR.available()) {
        ir_signal IRsignal_ = TreasureBase_IR.read();
        unsigned long currTime = millis();
        if ((currStatus == 1) && (currTime - lastActionReceived > TREASURE_BASE_ACTION_RECV_WAIT)) {
          Serial.printf("RECV %d %d %d %d | %d %d %d %d \n", IRsignal_.address.digit3, IRsignal_.address.digit2, IRsignal_.address.digit1, IRsignal_.address.digit0, IRsignal_.command.digit3, IRsignal_.command.digit2, IRsignal_.command.digit1, IRsignal_.command.digit0);
          CLAN_ = IRsignal_.address.digit2;
          ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);

          channel_ = IRsignal_.command.digit2;
          Treas_Deposit_= IRsignal_.command.digit1 >>4; //Extracting the upper 4 bits
          MULTIPLIER_ = IRsignal_.command.digit1 & 0x0f;// Extracting the lower 4 bits
          action_ = IRsignal_.command.digit0;
          Serial.printf("Action ID and  Treasure deposit, multiplier", action_);
          Serial.println(" ");
          Serial.printf("Treasure Deposit %d", Treas_Deposit);
          Serial.println(" ");
          Serial.printf("Multiplier %d", MULTIPLIER_);
          Serial.println(" ");
          lastActionReceived = currTime;

          if (action_ == collect) {
            lastOpenedTime = millis();
            handleAction(CLAN_,ID_,action_,Treas_Deposit_,MULTIPLIER_,channel_);
          }
        }
      }
    };

    void handleAction(int CLAN_, int ID_, int action_, int Treas_Deposit_, int MULTIPLIER_, int channel_) {
      Serial.print("Action ID: "); Serial.println(action_);
      //action_= action;
      Treas_Deposit=Treas_Deposit_;
      switch (action_)
      {
        case collect:
        Treas_Deposit= Treas_Deposit_ / 16;
        handle_Collected(Treas_Deposit);
         break; 
         
        default:
          break;
      }

        
    }

    

void sendDepositAction(int CLAN_, int ID_, int TREAS_DEPOSIT, int action_, int channel_) {
    uint16_hex_digits address_digits, command_digits;
  
    int This_Action_Multiplier = MULTIPLIER; // Set the multiplier to the treasure deposit amount
    int ID=ID_;
    int CLAN=CLAN_;
    int action=action_;
    
    address_digits.digit0 = ID % 16;
    address_digits.digit1 = ID / 16;
    address_digits.digit2 = CLAN;
    
    command_digits.digit2 = channel_; // Assuming WiFi channel or similar is not needed
    command_digits.digit1 = This_Action_Multiplier; // Set the multiplier for the deposit

    // Ensure the action is set to deposit
    switch (action_) {
        case collect:
            action = deposit;
            Serial.printf("New action ID %d\n", action);
             command_digits.digit0 = action;
            break;
        default:
            break;
    }
    // Set the action code

    ir_signal send_signal;
    send_signal.address = address_digits;
    send_signal.command = command_digits;

    // Print the signal for debugging
    Serial.printf("SEND %d %d %d %d | %d %d %d %d \n", 
                  address_digits.digit3, address_digits.digit2, address_digits.digit1, address_digits.digit0, 
                  command_digits.digit3, command_digits.digit2, command_digits.digit1, command_digits.digit0);
    
    // Send the IR signal
    TreasureBase_IR.send(send_signal, 1);
}


    // void feedback_collectL1(int CLAN_, int ID_, int channel_) {
    //   unsigned long currTime = millis();
    //   int powerup_ID = random(1, 6);
    //   TreasureBase_EspNOW.send_data(2, CLAN_, ID_, ID, true, powerup_ID, channel_);
    // } ;

    void handle_Collected(int Treas_Deposit) {
      interim_collected_display();

      // inform the server here ...
      TreasureBase_NeoPixel.off_FRONT();
      TreasureBase_NeoPixel.off_TOP();

      Serial.printf("Hello World IR Passed through");

      Serial.printf("TREASURE%d opened by CLAN %d ID %d\n", ID, CLAN_, ID_);
      //feedback_collectL1(CLAN_, ID_, channel_);
      //int player_identifier = CLAN_ * pow(16, 2) + ID_;

      // this code to save the info of the CLAN collected the treasure
      this_recover_duration = TREASURE_BASE_RECOVER_DURATION * random(1, 9);


      EEPROM.write(ENABLE_add, 2);
      switch (CLAN_)
      {
        case INVICTA:
          {EEPROM.write(INVICTA_add, EEPROM.read(INVICTA_add) + Treas_Deposit);
          INVICTA_TREAS=EEPROM.read(INVICTA_add);}
          break;

        case DYNARI:
          {EEPROM.write(DYNARI_add, EEPROM.read(DYNARI_add) + Treas_Deposit); // EEPROM.write(address, value)
          DYNARI_TREAS=EEPROM.read(INVICTA_add);}
          break;

        case EPHILIA:
          {EEPROM.write(EPHILIA_add, EEPROM.read(EPHILIA_add) + Treas_Deposit);
          EPHILIA_TREAS=EEPROM.read(INVICTA_add);}
          break;

        case AKRONA:
          {EEPROM.write(AKRONA_add, EEPROM.read(AKRONA_add) + Treas_Deposit);
          AKRONA_TREAS=EEPROM.read(AKRONA_add);}
          break;

        case SOLARIS:
          {EEPROM.write(SOLARIS_add, EEPROM.read(SOLARIS_add) + Treas_Deposit);
          SOLARIS_TREAS=EEPROM.read(SOLARIS_add);}
          break;

        default:
          break;
      }
      EEPROM.commit();
     sendDepositAction(CLAN_, ID_, MULTIPLIER, action_,channel_);

      // if (WIFI_ON) {
      //   dbc.setTreasureAsOpened("TREASURE" + String(ID), CLAN_, ID_);
      // }
    };

    void recover() {
      // Level1Treasures can recover after a fixed amt of time
      int currStatus = EEPROM.read(ENABLE_add);
      unsigned int currTime = millis();

      if (currStatus == 2 && currTime - lastOpenedTime > this_recover_duration && !uploadInProgress) {
        Serial.println("Reopening Treasure..");
        EEPROM.write(ENABLE_add, 1);
        HP = TREASURE_BASE_INITIAL_HP;
        TreasureBase_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
        TreasureBase_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
      }
    };

    void display_not_playing_yet() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 1 Treasure  "));

      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("      Hold on!!     ");
      display.setCursor(0, 30);
      display.println("  The game has not  ");
      display.println("    started yet.    ");
      display.display();
    };

    void interim_collected_display() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Treasure Base  "));

      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("    Please wait ...   ");
      display.display();
    }

    void display_in_game() {
      int currStatus = EEPROM.read(ENABLE_add);
      if (currStatus == 1) {
        display.clearDisplay(); \
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Tressure Base  "));

        display.setCursor(0, 24);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("    Press Down to      ");
        display.println("       Deposit!   ");

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
        display.println(F("   Treasure Base  "));

        display.setCursor(0, 12);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        //display.println("  Treasure is Opened  ");

        //display.setCursor(0, 22);
        //display.println("  Resetting soon...  ");

        display.setCursor(0, 10);
        display.setTextSize(1);
        display.println("   Clan Deposited:  ");

        display.setCursor(0, 20); // sets the row and the column
        display.setTextSize(1); // set the size of the text 
        if (CLAN_ == INVICTA)
          {display.println("      INVICTA       ");
          display.println("      ");
          display.println("  Total Tressure  ");
          display.println("      ");
          display.println(INVICTA_TREAS);}

        else if (CLAN_ == DYNARI)
          {display.println("       DYNARI       ");
          display.println("      ");
          display.println("   Total Tressure  ");
          display.print("      ");
          display.println(DYNARI_TREAS);}


        else if (CLAN_ == EPHILIA)
          {display.println("      EPHILIA       ");
          display.println("      ");
          display.println("     Total Tressure  ");
          display.print("      ");
          display.println(DYNARI_TREAS);}

        else if (CLAN_ == AKRONA)
          {display.println("       AKRONA       ");
          display.println("      ");
          display.println("     Total Tressure  ");
          display.print("      ");
          display.println(AKRONA_TREAS);}

        else if (CLAN_ == SOLARIS)
          {display.println("      SOLARIS       ");
          display.println("      ");
          display.println("     Total Tressure  ");
          display.print("      ");
          display.println(SOLARIS_TREAS);}

        if (!WIFI_ON) {
          display.setCursor(0, 56);
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.println("    OFFLINE MODE    ");
        }

        display.display();
      };
    }
};

TreasureBase treasureBase;  // use OLED to input ID
bool gameStarted = 0;


void clearEEPROM() {
  int i;
  for (i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

bool setUpDone = 0;
int get_game_state() {
  // Directly set game parameters without connecting to WiFi or checking server
  if (!parameters_updated) {
    Serial.println("[BACKGROUND] Setting game parameters locally...");
    
    // Set your game parameters directly
    HTTP_TIMEOUT = 5000; // Example value
    TREASURE_BASE_INITIAL_HP = 100; // Example value
    TREASURE_BASE_ACTION_RECV_WAIT = 3000; // Example value
    TREASURE_BASE_RECOVER_DURATION = 60000; // Example value
    parameters_updated = true;

    Serial.println("[BACKGROUND] Parameters Set Locally");
  }

  // Assuming the game has started
  if (gameStarted) {
    return 1;  // once the game has started then we don't need to check anymore
  } else {
    gameStarted = true;  // Directly set gameStarted to true

    Serial.println("[BACKGROUND] Game has started! Initialising Treasure..");
    TreasureBase_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
    TreasureBase_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
    treasureBase.init_treasure();
    setUpDone = 1;

    // No need to disconnect WiFi as we're not using it
    // WiFi.disconnect();
    
    return gameStarted;
  }
}

TaskHandle_t backgroundTask;

void backgroundTaskCode(void * pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for ( ; ; ) {
    if (!WIFI_ON) {
      // offline mode
      if (!setUpDone) {
        TreasureBase_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
        TreasureBase_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
        treasureBase.init_treasure();
        setUpDone = 1;
      }
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));

    } else {
      get_game_state();

      if (gameStarted && can_upload_fail && failed_kill_feedback > 0) {
        Serial.println("[BACKGROUND] Initiating Failed ESPNOW Feedback Upload...");
        uploadInProgress = 1;  // prevent Treasure from recovering
        dbc.connectToWiFi();
        for (int i = failed_kill_feedback; i > 0; i--) {
          int this_ID = failed_kill_ID[current_failed_read_pointer];
          int this_CLAN = failed_kill_CLAN[current_failed_read_pointer];
          Serial.printf("[BACKGROUND] Upload Failed ESPNOW Feedback from CLAN %d ID %d\n", this_CLAN, this_ID);
          dbc.uploadFailedFeedback("TREASURE" + String(ID), this_CLAN, this_ID);  // if still unsuccessful, it will be ignored. TODO: Fix
          current_failed_read_pointer++;
          if (current_failed_read_pointer > 50) current_failed_read_pointer -= 50;
          failed_kill_feedback--;
        }
        Serial.println("[BACKGROUND] Failed ESPNOW Feedback completed");
        WiFi.disconnect();
        uploadInProgress = 0;
      }
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
  TreasureBase_NeoPixel.initialize();
  TreasureBase_NeoPixel.off_FRONT();
  TreasureBase_NeoPixel.off_TOP();

  TreasureBase_IR.enable();
  TreasureBase_EspNOW.enable();
  EEPROM.begin(EEPROM_SIZE);
  ID = EEPROM.read(ID_add);
  WIFI_ON = EEPROM.read(ONLINE_mode_add);

  // Hardcoded constants, in case there is no WiFi to update
  HTTP_TIMEOUT = 30000;
  TREASURE_BASE_INITIAL_HP = 1;
  TREASURE_BASE_ACTION_RECV_WAIT = 3000;
  TREASURE_BASE_RECOVER_DURATION = 20000;

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
  if (AdminFunction) {
    TreasureBase_Admin.AdminLoop();
  } else if (!WIFI_ON) {
    // offline mode
    handleJoystick();
    treasureBase.display_in_game();
    treasureBase.receiveAction();
    //tressureBase.sendDepositAction();
    treasureBase.recover();
  } else {
    // online mode
    handleJoystick();
    if (setUpDone) {
      treasureBase.display_in_game();
      treasureBase.receiveAction();
      treasureBase.recover();
    } else {
      treasureBase.display_not_playing_yet();
    }
  }
}
