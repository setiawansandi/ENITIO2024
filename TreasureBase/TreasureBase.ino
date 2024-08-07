#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define hp_pxl_bar_width 100 // [pxl/HP]

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...

// the font height for font size 1 is 8
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Enito logo, 128 x 64px
const unsigned char enitioLogo[] PROGMEM = {
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
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// down arrow, 10 x 9px
const unsigned char downArrow_bitmap[] PROGMEM = {
    0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x7f, 0x80, 0x3f, 0x00, 0x1e, 0x00,
    0x0c, 0x00};

// heart, 11 x 11px
const unsigned char heart_bitmap [] PROGMEM = {
	0x00, 0x00, 0x31, 0x80, 0x7b, 0xc0, 0xff, 0xe0, 0xff, 0xe0, 0x7f, 0xc0, 0x3f, 0x80, 0x1f, 0x00, 
	0x0e, 0x00, 0x04, 0x00, 0x00, 0x00
};

// bao, 11x11px
const unsigned char bao_bitmap [] PROGMEM = {
	0x00, 0x00, 0x1f, 0x00, 0x6a, 0xc0, 0xdb, 0x60, 0xbf, 0xa0, 0xfb, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 
	0xff, 0xe0, 0x7f, 0xc0, 0x00, 0x00
};

// lED RGB values
// Aqua
#define R_DEPOSITED 6
#define G_DEPOSITED 62
#define B_DEPOSITED 42

// Orange
#define R_ATTACKED 255
#define G_ATTACKED 42
#define B_ATTACKED 0

// Orange
#define R_DESTROYED 255
#define G_DESTROYED 42
#define B_DESTROYED 0

struct RGB
{
  int R;
  int G;
  int B;
};

RGB clanColoursRGB[6] = { 
    {220, 80, 0}, // INVICTA 
    {225, 0, 0},   // DYNARI 
    {0, 255, 0},   // EPHILIA 
    {0, 0, 255},   // AKRONA 
    {54, 1, 63},  // SOLARIS 
    {0, 0, 0},     // UNKNOWN 
};

#define EEPROM_SIZE 11

#define ID_BOUND 50

#define ENABLE_add 0 // 0 means Base has not been initialized, 1 means base active, 2 means base destroyed
#define ID_add 1
#define HP_add 2
#define INVICTA_add 3
#define DYNARI_add 4
#define EPHILIA_add 5
#define AKRONA_add 6
#define SOLARIS_add 7
#define ONLINE_mode_add 8
#define CLAN_add 9
#define CLAN_enable_add 10 // 0 means clan has not been set, 1 otherwise

#define DOUBLE_CLICK_LENGTH 1000 // [ms]

bool AdminFunction = false;
int ID;

int failed_kill_feedback = 0;
int failed_kill_CLAN[50] = {};
int failed_kill_ID[50] = {};
int current_failed_save_pointer = 0;
int current_failed_read_pointer = 0;
bool uploadInProgress = 0; // boolean required to ensure treasure doesn't recover while WiFi is transmitting
bool can_upload_fail = 1;
int WIFI_ON;
bool parameters_updated = false;
int AKRONA_TREAS = 0;
int INVICTA_TREAS = 0;
int DYNARI_TREAS = 0;
int EPHILIA_TREAS = 0;
int SOLARIS_TREAS = 0;

int action = 0;
int BASE_CLAN_VALUE;
int currentTreasureDeposited = 0;

bool clicked_once = 0;
unsigned long last_clicked = 0;

// Constants
const int TREASURE_BASE_MAX_HP = 25;
const int TREASURE_BASE_ACTION_RECV_WAIT = 1500;
const int TREASURE_BASE_RECOVER_DURATION = 30000;
const int DISPLAY_HP_DURATION = 5000;
const int HP_RECOVERY_RATE = 20000; // 1HP / 20s

const int NOTI_SOUND_DURATION = 300;
unsigned long tempNoti_start = 0;

// stub functions
void StartUpDisplay();
void clearEEPROM();

#include <EEPROM.h>
#include "ENITIO_ir.h"
#include "ENITIO_enums.h"
#include "ENITIO_ESPNOW.h"
#include "ENITIO_NeoPixel.h"
#include "ENITIO_joystick.h"
#include "ENITIO_buzzer.h"
#include "Admin.h"

void StartUpDisplay()
{
  display.clearDisplay();
  display.setTextSize(1); // Draw SIZE

  display.setTextColor(SSD1306_WHITE);

  display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE);

  display.setCursor(0, 56);
  display.println("   Please wait ...   ");
  display.display();
};

void handleJoystick()
{
  joystick_pos joystick_pos = TreasureBase_joystick.read_Joystick();
  if (TreasureBase_joystick.get_state() == 0)
  {
    switch (joystick_pos)
    {
    case button:
      if (clicked_once)
      {
        AdminFunction = true;
        clicked_once = 0;
      }
      else
      {
        clicked_once = 1;
        last_clicked = millis();
      }
      TreasureBase_joystick.set_state();
      break;

    case idle:
      if (clicked_once)
      {
        unsigned long currTime = millis();
        if (currTime - last_clicked > DOUBLE_CLICK_LENGTH)
          clicked_once = 0;
      }
      break;

    default:
      if (clicked_once)
      {
        unsigned long currTime = millis();
        if (currTime - last_clicked > DOUBLE_CLICK_LENGTH)
          clicked_once = 0;
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
  int MaxHP;
  bool depositEvent = false;

  int recovery_time = 0;
  unsigned long lastActionReceived = INT_MIN;
  unsigned long lastAttackedTime = INT_MIN;
  unsigned long lastHpRecoverTime = 0;
  unsigned long cooldownStartTime = 0;

public:
  int CLAN_, ID_, En_, MULTIPLIER_, action_, channel_, Treas_Deposit_;

  void init_base()
  {
    if (EEPROM.read(CLAN_enable_add) == 0)
    {
      BASE_CLAN_VALUE = 5; // unknown clan
    }
    else
    {
      BASE_CLAN_VALUE = EEPROM.read(CLAN_add);
    }

    EEPROM.write(ENABLE_add, 1);
    EEPROM.commit();
    HP = TREASURE_BASE_MAX_HP;
    MaxHP = TREASURE_BASE_MAX_HP;
  }

  void receiveAction()
  {
    unsigned long currTime = millis();

    // Turn off LED
    if (currTime - lastActionReceived > TREASURE_BASE_ACTION_RECV_WAIT)
    {
      TreasureBase_NeoPixel.off_FRONT();
      TreasureBase_NeoPixel.off_TOP();

      depositEvent = false;
    }

    int currStatus = EEPROM.read(ENABLE_add);
    if (TreasureBase_IR.available())
    {
      ir_signal IRsignal_ = TreasureBase_IR.read();
      if ((currStatus == 1) && (currTime - lastActionReceived > TREASURE_BASE_ACTION_RECV_WAIT))
      {
        // Serial.printf("command digit one received %d\n",IRsignal_.command.digit1);
        Serial.printf("RECV %d %d %d %d | %d %d %d %d \n", IRsignal_.address.digit3, IRsignal_.address.digit2, IRsignal_.address.digit1, IRsignal_.address.digit0, IRsignal_.command.digit3, IRsignal_.command.digit2, IRsignal_.command.digit1, IRsignal_.command.digit0);
        CLAN_ = IRsignal_.address.digit2;
        ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);

        channel_ = IRsignal_.command.digit2;
        Treas_Deposit_ = IRsignal_.command.digit3; // Extracting the upper 4 bits
        MULTIPLIER_ = IRsignal_.command.digit1;    // Extracting the lower 4 bits
        action_ = IRsignal_.command.digit0;
        lastActionReceived = currTime;

        if (CLAN_ == BASE_CLAN_VALUE)
        {
          if (action_ == collect)
          {
            handleDeposit(CLAN_, ID_, Treas_Deposit_, MULTIPLIER_, channel_);
          }
        }
        else if (CLAN_ != BASE_CLAN_VALUE)
        {
          if (action_ == attack)
          {
            handleAttacked(CLAN_, ID_, MULTIPLIER_, channel_);
          }
        }
      }
    }
  }

  void handleDeposit(int CLAN_, int ID_, int Treas_Deposit_, int MULTIPLIER_, int channel_)
  {
    if (Treas_Deposit_ % 16 <= 0)
      return;

    currentTreasureDeposited = Treas_Deposit_ % 16;

    interim_collected_display();

    depositEvent = true;

    TreasureBase_NeoPixel.displayRGB_FRONT(R_DEPOSITED, G_DEPOSITED, B_DEPOSITED);
    TreasureBase_NeoPixel.displayRGB_TOP(R_DEPOSITED, G_DEPOSITED, B_DEPOSITED);

    Serial.printf("TREASURE%d deposited by CLAN %d ID %d\n", ID, CLAN_, ID_);

    switch (CLAN_)
    {
    case INVICTA:
      EEPROM.write(INVICTA_add, EEPROM.read(INVICTA_add) + currentTreasureDeposited);
      INVICTA_TREAS = EEPROM.read(INVICTA_add);
      break;

    case DYNARI:
      EEPROM.write(DYNARI_add, EEPROM.read(DYNARI_add) + currentTreasureDeposited); // EEPROM.write(address, value)
      DYNARI_TREAS = EEPROM.read(DYNARI_add);
      break;

    case EPHILIA:
      EEPROM.write(EPHILIA_add, EEPROM.read(EPHILIA_add) + currentTreasureDeposited);
      EPHILIA_TREAS = EEPROM.read(EPHILIA_add);
      break;

    case AKRONA:
      EEPROM.write(AKRONA_add, EEPROM.read(AKRONA_add) + currentTreasureDeposited);
      AKRONA_TREAS = EEPROM.read(AKRONA_add);
      break;

    case SOLARIS:
      EEPROM.write(SOLARIS_add, EEPROM.read(SOLARIS_add) + currentTreasureDeposited);
      SOLARIS_TREAS = EEPROM.read(SOLARIS_add);
      break;

    default:
      break;
    }
    EEPROM.commit();

    // to clear the inventory of the treasures collected by Wanderers
    sendDepositAction(CLAN_, ID_, MULTIPLIER_, action_, channel_);
  }

  void handleAttacked(int CLAN_, int ID_, int MULTIPLIER_, int channel_)
  {
    Serial.print("Attacked, HP: ");
    HP = max(HP - MULTIPLIER_, 0);
    EEPROM.write(HP_add, HP);
    Serial.println(HP);

    TreasureBase_Buzzer.sound(NOTE_B0);
    tempNoti_start = millis();
    TreasureBase_NeoPixel.displayRGB_FRONT(R_ATTACKED, G_ATTACKED, B_ATTACKED);
    TreasureBase_NeoPixel.displayRGB_TOP(R_ATTACKED, G_ATTACKED, B_ATTACKED);

    feedback_attack(CLAN_, ID_, channel_);

    if (HP == 0)
    {
      EEPROM.write(ENABLE_add, 2);
      EEPROM.commit();

      display_base_destroyed(false);

      for (int i = 0; i < 3; ++i)
      {
        TreasureBase_Buzzer.sound(NOTE_C2);
        TreasureBase_NeoPixel.displayRGB_FRONT(R_DESTROYED, G_DESTROYED, B_DESTROYED);
        TreasureBase_NeoPixel.displayRGB_TOP(R_DESTROYED, G_DESTROYED, B_DESTROYED);
        delay(600);

        TreasureBase_Buzzer.end_sound();
        TreasureBase_NeoPixel.off_FRONT();
        TreasureBase_NeoPixel.off_TOP();
        delay(600);
      }

      // set what the base recover duration
      cooldownStartTime = millis();
      recovery_time = TREASURE_BASE_RECOVER_DURATION + millis();
      Serial.print("Base will respawn at ");
      Serial.print(recovery_time);
      Serial.print(" Current time: ");
      Serial.println(millis());

      // destroy all the treasure collected
      destroyAllTreasure();
    }
    lastHpRecoverTime = millis();
    lastAttackedTime = millis();
  }

  void feedback_attack(int CLAN_, int ID_, int channel_)
  {
    bool killed = (HP == 0);
    TreasureBase_EspNOW.send_data_attacked(1, 6, CLAN_, ID_, BASE_CLAN_VALUE, killed, channel_);
  }

  void sendDepositAction(int CLAN_, int ID_, int MULTIPLIER_, int action_, int channel_)
  {
    uint16_hex_digits address_digits, command_digits;

    int This_Action_Multiplier = MULTIPLIER_; // Set the multiplier to the treasure deposit amount
    int ID = ID_;
    int CLAN = CLAN_;
    int action = action_;

    address_digits.digit0 = ID % 16;
    address_digits.digit1 = ID / 16;
    address_digits.digit2 = CLAN;

    command_digits.digit2 = channel_;               // Assuming WiFi channel or similar is not needed
    command_digits.digit1 = This_Action_Multiplier; // Set the multiplier for the deposit

    // Ensure the action is set to deposit
    switch (action_)
    {
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
    delay(100);
    TreasureBase_IR.send(send_signal, 1);
  }

  void recover()
  {
    // Base can recover after a fixed amt of time
    // currStatus: 1-ALIVE, 2-DEAD
    int currStatus = EEPROM.read(ENABLE_add);
    unsigned int currTime = millis();

    if (currStatus == 2 && currTime > recovery_time && !uploadInProgress)
    {
      Serial.print("Respawning Base.. | Current-time: ");
      Serial.println(millis());
      EEPROM.write(ENABLE_add, 1);
      HP = MaxHP;
    }
    else if (currStatus == 1)
    {
      regenerateHP();
    }
  };

  void regenerateHP()
  {
    unsigned long currTime = millis();

    if (HP < MaxHP && (currTime - lastHpRecoverTime >= HP_RECOVERY_RATE))
    {
      ++HP;
      EEPROM.write(HP_add, HP);
      lastHpRecoverTime = currTime;
    }
  }

  void destroyAllTreasure()
  {
    switch (BASE_CLAN_VALUE)
    {
    case INVICTA:
      EEPROM.write(INVICTA_add, 0);
      INVICTA_TREAS = EEPROM.read(INVICTA_add);
      break;

    case DYNARI:
      EEPROM.write(DYNARI_add, 0);
      DYNARI_TREAS = EEPROM.read(DYNARI_add);
      break;

    case EPHILIA:
      EEPROM.write(EPHILIA_add, 0);
      EPHILIA_TREAS = EEPROM.read(EPHILIA_add);
      break;

    case AKRONA:
      EEPROM.write(AKRONA_add, 0);
      AKRONA_TREAS = EEPROM.read(AKRONA_add);
      break;

    case SOLARIS:
      EEPROM.write(SOLARIS_add, 0);
      SOLARIS_TREAS = EEPROM.read(SOLARIS_add);
      break;

    default:
      break;
    }
    EEPROM.commit();
  }

  void display_not_playing_yet()
  {
    display.clearDisplay();
    display.setTextSize(1);                             // Draw SIZE
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
    display.setCursor(0, 0);
    display.println("    Treasure Base    ");

    display.setCursor(0, 16);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.println("      Hold on!!     ");
    display.setCursor(0, 30);
    display.println("  The game has not  ");
    display.println("    started yet.    ");
    display.display();
  };

  void interim_collected_display()
  {
    display.clearDisplay();
    display.setTextSize(1);                             // Draw SIZE
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
    display.setCursor(0, 0);
    display.println("    Treasure Base    ");

    display.setCursor(0, 16);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.println("    Please wait ...   ");
    display.display();
  }

  void display_default_screen()
  {
    int currStatus = EEPROM.read(ENABLE_add);
    if (currStatus == 1)
    {
      display.clearDisplay();
      display.setTextSize(1);                             // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println("    Treasure Base    ");

      display.setTextSize(2);              // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text

      BASE_CLAN_VALUE = EEPROM.read(CLAN_add);

      int totalTreasure = 0;

      int y_pos = 14;
      switch (BASE_CLAN_VALUE)
      {
      case INVICTA:
        display.setCursor(21, y_pos);
        display.print("INVICTA");
        totalTreasure = EEPROM.read(INVICTA_add);
        break;
      case DYNARI:
        display.setCursor(29, y_pos);
        display.print("DYNARI");
        totalTreasure = EEPROM.read(DYNARI_add);
        break;
      case EPHILIA:
        display.setCursor(22, y_pos);
        display.print("EPHILIA");
        totalTreasure = EEPROM.read(EPHILIA_add);
        break;
      case AKRONA:
        display.setCursor(29, y_pos);
        display.print("AKRONA");
        totalTreasure = EEPROM.read(AKRONA_add);
        break;
      case SOLARIS:
        display.setCursor(22, y_pos);
        display.print("SOLARIS");
        totalTreasure = EEPROM.read(SOLARIS_add);
        break;
      default:
        display.setCursor(21, y_pos);
        display.print("UNKNOWN");
        break;
      }


      display.setTextSize(1);

      // draw tables
      /*
          (x_start, y_start)
                    _____________
                    |           |
                    |           |
                    |           |
                    _____________
                          (x_end, y_end)
      */
      int x_start = 13, y_start = 33;
      int x_end = 114, y_end = 47;
      int mid_x = (x_start + x_end) / 2;

      display.drawLine(x_start, y_start, x_end, y_start, SSD1306_WHITE); // horizontal
      display.drawLine(x_start, y_end, x_end, y_end, SSD1306_WHITE); // horizontal
      display.drawLine(x_start, y_start, x_start, y_end, SSD1306_WHITE); // vertical
      display.drawLine(mid_x, y_start, mid_x, y_end, SSD1306_WHITE); // vertical
      display.drawLine(x_end, y_start, x_end, y_end, SSD1306_WHITE); // vertical

      display.drawBitmap(x_start + 3, y_start + 2, heart_bitmap, 11, 11, SSD1306_WHITE);
      display.drawBitmap(mid_x + 4, y_start + 2, bao_bitmap, 11, 11, SSD1306_WHITE);

      display.setCursor(x_start + 18, y_start + 4);
      display.printf("%-2d/25\n", HP);
      
      display.setCursor(mid_x + 19, y_start + 4);
      display.printf("x%4d\n", totalTreasure);


      display.setCursor(2, 54);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("  Hold   to Deposit  ");
      display.drawBitmap(42, 53, downArrow_bitmap, 12, 15, WHITE);

      display.display();
    }
  }

  int lastHP = -1;
  void display_hp_screen()
  {
    unsigned long currentMillis = millis();

    if (HP != lastHP)
    {
      lastHP = HP;

      display.clearDisplay();
      display.setTextSize(1);                             // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println("    Treasure Base    ");

      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("   BASE ATTACKED!!   ");
      display.display();

      // Display HP as text
      display.setCursor(0, 32);
      display.print("HP: ");
      display.print(HP);

      // Display HP bar
      int barWidth = map(HP, 0, MaxHP, 0, 128);             // map(value, min_input, max_input, min_output, max_output)
      display.drawRect(0, 48, 128, 16, SSD1306_WHITE);      // Draw outline
      display.fillRect(0, 48, barWidth, 16, SSD1306_WHITE); // Fill based on HP

      display.display();
    }
  }

  void display_base_destroyed(bool showCooldown)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 16);
    display.println(" Base");
    display.println(" Destroyed");

    unsigned long elapsed = millis() - cooldownStartTime;

    if (showCooldown && elapsed < TREASURE_BASE_RECOVER_DURATION)
    {
      unsigned long remainingTime = (TREASURE_BASE_RECOVER_DURATION - (millis() - cooldownStartTime)) / 1000;
      display.setTextSize(1);
      display.setCursor(12, 54);
      display.printf("Respawning in %ds", remainingTime);
    }

    display.display();
  }

  void display_deposited()
  {
    display.clearDisplay();
    display.setTextSize(1);                             // Draw SIZE
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
    display.setCursor(0, 0);
    display.println("    Treasure Base    ");

    display.setTextColor(SSD1306_WHITE); // Draw white text

    display.setTextSize(1);
    display.setCursor(36, 17);
    display.println("Deposited");

    display.setTextSize(2);
    if (currentTreasureDeposited < 10)
    {
      display.setCursor(58, 32);
    }
    else
    {
      display.setCursor(51, 32);
    }
    display.print(currentTreasureDeposited);

    display.setTextSize(1);
    display.setCursor(32, 54);
    display.println("treasure(s)");

    display.display();
  }

  void display_in_game()
  {
    unsigned long currentMillis = millis();

    if (depositEvent)
    {
      display_deposited();
    }
    else if (HP <= 0)
    {
      display_base_destroyed(true);
    }
    else if (currentMillis - lastAttackedTime < DISPLAY_HP_DURATION)
    {
      display_hp_screen();
    }
    else
    {
      TreasureBase_NeoPixel.displayRGB_FRONT(clanColoursRGB[BASE_CLAN_VALUE].R,
                                             clanColoursRGB[BASE_CLAN_VALUE].G,
                                             clanColoursRGB[BASE_CLAN_VALUE].B);

      TreasureBase_NeoPixel.displayRGB_TOP(clanColoursRGB[BASE_CLAN_VALUE].R,
                                           clanColoursRGB[BASE_CLAN_VALUE].G,
                                           clanColoursRGB[BASE_CLAN_VALUE].B);

      display_default_screen();
    }
  }
};

TreasureBase treasureBase; // use OLED to input ID
bool gameStarted = 0;

void clearEEPROM()
{
  int i;
  for (i = 0; i < EEPROM_SIZE; i++)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void update_sound()
{
  unsigned long currTime = millis();
  if (currTime - tempNoti_start >= NOTI_SOUND_DURATION)
  {
    TreasureBase_Buzzer.end_sound();
  }
}

bool setUpDone = 0;
int get_game_state()
{
  // Directly set game parameters without connecting to WiFi or checking server
  if (!parameters_updated)
  {
    Serial.println("[BACKGROUND] Setting game parameters locally...");

    // Set your game parameters directly

    Serial.println("[BACKGROUND] Parameters Set Locally");
  }

  // Assuming the game has started
  if (gameStarted)
  {
    treasureBase.init_base();
    return 1; // once the game has started then we don't need to check anymore
  }
  else
  {
    gameStarted = true; // Directly set gameStarted to true

    Serial.println("[BACKGROUND] Game has started! Initialising Treasure..");
    treasureBase.init_base();
    setUpDone = 1;

    // No need to disconnect WiFi as we're not using it
    // WiFi.disconnect();

    return gameStarted;
  }
}

TaskHandle_t backgroundTask;

void backgroundTaskCode(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    if (!WIFI_ON)
    {
      // offline mode
      if (!setUpDone)
      {
        treasureBase.init_base();
        setUpDone = 1;
      }
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
    else
    {
      get_game_state();

      if (gameStarted && can_upload_fail && failed_kill_feedback > 0)
      {
        Serial.println("[BACKGROUND] Initiating Failed ESPNOW Feedback Upload...");
        uploadInProgress = 1; // prevent Treasure from recovering
        dbc.connectToWiFi();
        for (int i = failed_kill_feedback; i > 0; i--)
        {
          int this_ID = failed_kill_ID[current_failed_read_pointer];
          int this_CLAN = failed_kill_CLAN[current_failed_read_pointer];
          Serial.printf("[BACKGROUND] Upload Failed ESPNOW Feedback from CLAN %d ID %d\n", this_CLAN, this_ID);
          dbc.uploadFailedFeedback("TREASURE" + String(ID), this_CLAN, this_ID); // if still unsuccessful, it will be ignored. TODO: Fix
          current_failed_read_pointer++;
          if (current_failed_read_pointer > 50)
            current_failed_read_pointer -= 50;
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

void treasureBaseLoop()
{
  treasureBase.receiveAction();
  treasureBase.display_in_game();
  treasureBase.recover();
  update_sound();
  EEPROM.commit();
}

void setup()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  StartUpDisplay();
  Serial.begin(115200);
  TreasureBase_NeoPixel.initialize();

  TreasureBase_IR.enable();
  TreasureBase_EspNOW.enable();
  EEPROM.begin(EEPROM_SIZE);
  ID = EEPROM.read(ID_add);
  WIFI_ON = EEPROM.read(ONLINE_mode_add);

  HTTP_TIMEOUT = 30000;

  xTaskCreatePinnedToCore(
      backgroundTaskCode, /* Task function. */
      "backgroundTask",   /* name of task. */
      10000,              /* Stack size of task */
      NULL,               /* parameter of the task */
      2,                  /* priority of the task */
      &backgroundTask,    /* Task handle to keep track of created task */
      0);
}

void loop()
{

  if (AdminFunction)
  {
    TreasureBase_Admin.AdminLoop();
  }
  else if (!WIFI_ON)
  {
    // offline mode
    handleJoystick();
    treasureBaseLoop();
  }
  else
  {
    // online mode
    handleJoystick();
    if (setUpDone)
    {
      treasureBaseLoop();
    }
    else
    {
      treasureBase.display_not_playing_yet();
    }
  }
}
