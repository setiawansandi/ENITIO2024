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

#define EEPROM_SIZE 11

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
#define CLAN_add 9
#define PROFILE_enable_add 10

#define R_ON 0
#define G_ON 0
#define B_ON 0


#define DOUBLE_CLICK_LENGTH 1000 // [ms]   
#define MainMenuProcess 0
#define AdminProcess 1

bool AdminFunction = false;
int ID;

int failed_kill_feedback = 0;
int failed_kill_CLAN[50] = {};
int failed_kill_ID[50] = {};
int current_failed_save_pointer = 0;
int current_failed_read_pointer = 0;
bool uploadInProgress = 0;  // boolean required to ensure treasure doesn't recover while WiFi is transmitting
bool can_upload_fail = 1;
bool base_destroyed=false;
bool showBaseHp=false;
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
int currentMenu = MainMenuProcess;
int display_nav_bar=0;
int clan_pointer=0;
int BaseClanValue;



bool clicked_once = 0;
unsigned long last_clicked = 0;
unsigned long lastRecoveredTime=0;
const unsigned long Attacked_Display_Duration=15000;
unsigned long lastDisplayUpdateTime = 0;
bool ProfileFunction = false;

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
#include "ENITIO_Profile.h"

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

// bool clicked_once = 0;
// unsigned long last_clicked = 0;



void handleJoystick() {
  joystick_pos joystick_pos = TreasureBase_joystick.read_Joystick();
  if (TreasureBase_joystick.get_state() == 0) {
    switch (joystick_pos) 
    {
      case right:
        display_nav_bar++;
        display_nav_bar = min(display_nav_bar, 2); // Limit display_nav_bar to 1 for Admin
        TreasureBase_joystick.set_state();
        break;

        case left:
        display_nav_bar=max(display_nav_bar-1,0); // Always show Main Menu when moving left
        TreasureBase_joystick.set_state();
        break;

      case button:
        handleButtonPress();
        TreasureBase_joystick.set_state();
        break;

      case idle:
        if (clicked_once)
         {
          unsigned long currTime = millis();
          if (currTime - last_clicked > DOUBLE_CLICK_LENGTH) clicked_once = 0;
          TreasureBase_joystick.get_state();
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
};

void handleButtonPress(){
  unsigned long current_time = millis();
  switch(display_nav_bar)
  {
  case 0:
    AdminFunction = false;
    ProfileFunction= false;
    break;
  case 1:
      AdminFunction = true;
      ProfileFunction = false;
      last_clicked = current_time;
      break;
  case 2:
    ProfileFunction = true;
    AdminFunction = false;
    last_clicked=current_time;
 }
}

class TreasureBase
{
  private:
    int HP;

    int this_recover_duration = TREASURE_BASE_RECOVER_DURATION;
    unsigned long lastOpenedTime = 0;
    unsigned long lastActionReceived = 0;
    unsigned long lastAttackedTime = 0;

  public:

    int CLAN_, ID_, En_, MULTIPLIER_, action_, channel_;

    void init_treasure() {
      EEPROM.write(ENABLE_add, 1);
      EEPROM.commit();
      HP = TREASURE_BASE_INITIAL_HP;
    }

    void receiveAction() {
      int currStatus = EEPROM.read(ENABLE_add);
      if (TreasureBase_IR.available()) {
        ir_signal IRsignal_ = TreasureBase_IR.read();
        unsigned long currTime = millis();
        if ((currStatus == 1) && (currTime - lastActionReceived > TREASURE_BASE_ACTION_RECV_WAIT)) 
        {
          //Serial.printf("command digit one received %d\n",IRsignal_.command.digit1);
          Serial.printf("RECV %d %d %d %d | %d %d %d %d \n", IRsignal_.address.digit3, IRsignal_.address.digit2, IRsignal_.address.digit1, IRsignal_.address.digit0, IRsignal_.command.digit3, IRsignal_.command.digit2, IRsignal_.command.digit1, IRsignal_.command.digit0);
          CLAN_ = IRsignal_.address.digit2;
          ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);

          channel_ = IRsignal_.command.digit2;
          Treas_Deposit_= IRsignal_.command.digit3; //Extracting the upper 4 bits
          MULTIPLIER_ = IRsignal_.command.digit1;// Extracting the lower 4 bits
          action_ = IRsignal_.command.digit0;
          lastActionReceived = currTime;
          // Serial.printf("Action ID and  Treasure deposit, multiplier", action_);
          // Serial.println(" ");
          // Serial.printf("Treasure Deposit %d", Treas_Deposit_);
          // Serial.println(" ");
          // Serial.printf("Multiplier %d", MULTIPLIER_);
          // Serial.println(" ");
        if(CLAN_ == BaseClanValue){
          if (action_ == collect) {
            lastOpenedTime = millis();
            handleDeposit(CLAN_,ID_,action_,Treas_Deposit_,MULTIPLIER_,channel_);
          }
        }else if (CLAN_!= BaseClanValue){
            if(action_ == attack){
            //lastAttackedTime = millis();
            handleAttacked(CLAN_,ID_,action_,MULTIPLIER_,channel_);
            }
        }
      }
    }
  }
    void handleDeposit(int CLAN_, int ID_, int action_, int Treas_Deposit_, int MULTIPLIER_, int channel_) {
      Serial.print("Action ID: "); Serial.println(action_);
      switch (action_)
      {
        case collect:
        Treas_Deposit= Treas_Deposit_ % 16;
        Serial.printf("Treasure caculated %d\n ", Treas_Deposit),
        handle_Collected(Treas_Deposit);
         break; 
        default:
          break;
      }   
    }
  
  
  void handleAttacked(int CLAN_, int ID_,int action_,int MULTIPLIER_, int channel_){
      switch(action_){  
        case attack:
        HP=max(HP-MULTIPLIER_,0);
        EEPROM.write(HP_add,HP);
        EEPROM.commit();
        Serial.println(HP);
        if (HP==0)
        {
          base_destroyed=true;
          
        }
        lastAttackedTime = millis();
        showBaseHp=true;
        lastDisplayUpdateTime = lastAttackedTime;
        break;
      }
    feedback_attackBase(CLAN_,ID_,BaseClanValue, channel_, base_destroyed);
}

void displayBaseHp(int HP)
{     
      if(HP>0){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Treasure Base  "));

      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("  Base Attacked !! ");
      display.display();

        // Display HP as text
        display.setCursor(0, 32);
        display.print("HP: ");
        display.print(HP);

        // Display HP bar
        int barWidth = map(HP, 0, 10, 0, 128); // Assuming max HP is 100 and screen width is 128
        display.drawRect(0, 48, 128, 16, SSD1306_WHITE); // Draw outline
        display.fillRect(0, 48, barWidth, 16, SSD1306_WHITE); // Fill based on HP

        display.display();

      } else {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 16);
        display.println(" Base");
        display.println("Destroyed!");
        display.display();
    }
}

    
void feedback_attackBase(int CLAN_, int ID_,int BaseClanValue, int channel_, bool base_destroyed) {
unsigned long currTime = millis();
TreasureBase_EspNOW.send_data(4, CLAN_, ID_,BaseClanValue, channel_, base_destroyed);
} ;

void sendDepositAction(int CLAN_, int ID_, int MULTIPLIER_, int action_, int channel_) {
    uint16_hex_digits address_digits, command_digits;
  
    int This_Action_Multiplier = MULTIPLIER_; // Set the multiplier to the treasure deposit amount
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
    delay(100);
    TreasureBase_IR.send(send_signal, 1);
    
}


    void handle_Collected(int Treas_Deposit) {
      interim_collected_display();

      // inform the server here ...
      TreasureBase_NeoPixel.off_FRONT();
      TreasureBase_NeoPixel.off_TOP();

      //Serial.printf("Hello World IR Passed through");

      Serial.printf("TREASURE%d opened by CLAN %d ID %d\n", ID, CLAN_, ID_);
      //feedback_collectL1(CLAN_, ID_, channel_);
      //int player_identifier = CLAN_ * pow(16, 2) + ID_;

      // this code to save the info of the CLAN collected the treasure
      this_recover_duration = TREASURE_BASE_RECOVER_DURATION * random(1, 5);


      EEPROM.write(ENABLE_add, 2);
      switch (CLAN_)
      {
        case INVICTA:
          {EEPROM.write(INVICTA_add, EEPROM.read(INVICTA_add) + Treas_Deposit);
          INVICTA_TREAS=EEPROM.read(INVICTA_add);}
          break;

        case DYNARI:
          {EEPROM.write(DYNARI_add, EEPROM.read(DYNARI_add) + Treas_Deposit); // EEPROM.write(address, value)
          DYNARI_TREAS=EEPROM.read(DYNARI_add);}
          break;

        case EPHILIA:
          {EEPROM.write(EPHILIA_add, EEPROM.read(EPHILIA_add) + Treas_Deposit);
          EPHILIA_TREAS=EEPROM.read(EPHILIA_add);}
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
     sendDepositAction(CLAN_, ID_, MULTIPLIER_, action_,channel_);

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
       display.clearDisplay();
      if (currStatus == 1) {
        display.clearDisplay(); \
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Tressure Base  "));

        display.setCursor(40, 13);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        //Serial.println(selectedProfile);
      BaseClanValue = EEPROM.read(CLAN_add);
      //Serial.println(BaseClanValue);
        switch(BaseClanValue){
          case 0:
            display.print("INVICTA");
            break;
          case 1:
            display.print("DYNARI");
            break;
          case 2:
            display.print("EPHILIA");
            break;
          case 3:
            display.print("AKRONA");
            break;
          case 4:
            display.print("SOLARIS");
            break;
      default:
            display.print("Unknon Clan");
        break;
      }

        display.setCursor(0, 24);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("    Press Down to      ");
        display.println("       Deposit!   ");
        Display_Nav_Bar();

        // if (!WIFI_ON) {  
        //   display.setCursor(0, 56);
        //   display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        //   display.println(" OFFLINE MODE ");
        //   display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        // }

        display.display();
      }
      else {
        display.clearDisplay(); \
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Treasure Base    "));

        //display.setCursor(0, 12);
        //display.setTextSize(1);      // Normal 1:1 pixel scale
        //display.println("  Treasure is Opened  ");

        //display.setCursor(0, 22);
        //display.println("  Resetting soon...  ");

        display.setCursor(0, 12);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("   Clan Deposited:  ");

        display.setCursor(0, 25); // sets the row and the column
        display.setTextSize(1); // set the size of the text 
        if (CLAN_ == INVICTA)
        
          {
          display.println("      INVICTA       ");
          display.println("  ");
          display.println("    Total Tressure  ");
          display.setCursor(50, 55);
          display.println(INVICTA_TREAS);}

        else if (CLAN_ == DYNARI)

          {display.println(" ");
          display.print("       DYNARI       ");
          display.println("  ");
          display.println("    Total Tressure  ");
          display.setCursor(50, 55);
          display.println(DYNARI_TREAS);}


        else if (CLAN_ == EPHILIA)
          {display.println(" ");
          //display.setCursor(20, 30); // sets the row and the column
          display.println("      EPHILIA       ");
          display.println("    Total Tressure  ");
          display.setCursor(50, 55);
          display.println(EPHILIA_TREAS);}

        else if (CLAN_ == AKRONA)
          {display.println("       AKRONA       ");
          display.println("  ");
          display.println("    Total Tressure  ");
          display.setCursor(50, 55);
          display.println(AKRONA_TREAS);}

        else if (CLAN_ == SOLARIS)
          {display.println("      SOLARIS       ");
          display.println("      ");
          display.println("    Total Tressure  ");
          display.setCursor(55, 55 );
          display.println(SOLARIS_TREAS);}

      // Display based on the state
        }
      } 
      
    display.display();
    }
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
    TREASURE_BASE_INITIAL_HP = 10; // Example value
    TREASURE_BASE_ACTION_RECV_WAIT = 5000; // Example value
    TREASURE_BASE_RECOVER_DURATION = 6000; // Example value
    parameters_updated = true;

    Serial.println("[BACKGROUND] Parameters Set Locally");
  }

  // Assuming the game has started
  if (gameStarted) {
    TreasureBase_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
    TreasureBase_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
    treasureBase.init_treasure();
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


void Display_Nav_Bar()
{

  switch (display_nav_bar)
  {
    case 0:
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(26, 56);
      display.println(F("< Main Menu >"));
      break;
    
    case 1:
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(26, 56);
      display.println(F("<   Admin   >"));
      break;

    case 2:
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(20, 56);
      display.println(F("< Base Profile  "));
      break;

      default:
      break;
  }
  display.display();
}

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
  //TreasureBase_NeoPixel.off_FRONT();
  //TreasureBase_NeoPixel.off_TOP();

  TreasureBase_IR.enable();
  TreasureBase_EspNOW.enable();
  EEPROM.begin(EEPROM_SIZE);
  ID = EEPROM.read(ID_add);
  WIFI_ON = EEPROM.read(ONLINE_mode_add);

  // Hardcoded constants, in case there is no WiFi to update
  HTTP_TIMEOUT = 30000;
  TREASURE_BASE_INITIAL_HP = 10;
  TREASURE_BASE_ACTION_RECV_WAIT = 5000;
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


void displayProfile() {
    display.clearDisplay();
    display.setTextSize(1);

    const char* clans[] = {"Invicta", "Dyanri", "Ephilia", "Akrona", "Solaris"};

    if (!TreasureBase_profile.isRegistered()) {
        // Step 1: Display title
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(" Choose your CLAN... ");

        // Step 2: Display clan selection menu
        display.setCursor(0, 13);
        display.setTextColor(SSD1306_WHITE);
        for (int i = 0; i < 5; ++i) {
            if (i == TreasureBase_profile.getClanPointer()) {
                display.print("> ");
            } else {
                display.print("  ");
            }
            display.println(clans[i]);
        }
        display.setCursor(0, 56);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println(" < Press to confirm > ");

        // Step 3: Handle clan selection and registration
        if (TreasureBase_joystick.read_Joystick() == button) {
            TreasureBase_profile.handleJoystickRegistered();
            // The next iteration of this function will show the registered clan
        }
    } else {
        // Step 4: Display registered clan info
        display.setCursor(0, 0);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println("     Treasure Base   ");
        display.setCursor(0, 15);
        display.setTextColor(SSD1306_WHITE);
        display.println("Your registered clan:");
        display.setTextSize(1);
        display.println(" ");
        display.println(clans[TreasureBase_profile.getRegisteredClan()]);
        display.setCursor(0, 56);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println(" Back to Main Menu >");

        // Step 5: Check for button press to return to main menu
        if (TreasureBase_joystick.read_Joystick() == button) {
            ProfileFunction = false;  // Return to main menu
            display_nav_bar = 0;
            TreasureBase_profile.CLANRegistered();
        }
    }

    display.display();
}


void loop() {

  if (AdminFunction) 
  {
    TreasureBase_Admin.AdminLoop();
  } else if(ProfileFunction)
    {
      delay(100);
      displayProfile();
      TreasureBase_profile.handleJoystickRegistered();
    }
  else if (!WIFI_ON)
   {
    // offline mode
    handleJoystick();
    treasureBase.display_in_game();
    treasureBase.receiveAction();
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
