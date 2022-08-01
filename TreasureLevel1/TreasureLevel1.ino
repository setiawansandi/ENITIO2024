// upload this code whenever you want to start testing
// comment out the clearEEPROM() function when used on real game

#include <EEPROM.h>
#include "ENITIO_ir.h"
#include "ENITIO_enums.h"
#include "ENITIO_ESPNOW.h"
#include "ENITIO_NeoPixel.h"

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

#define EEPROM_SIZE 10

#define hp_pxl_bar_width 100 // [pxl/HP]

#define ENABLE_add 0  // 0 means Treasure has not been initialized, 1 means already initialized

#define ID_add 1
#define HP_add 2
#define collectedOG_add 3

#define R_ON 0
#define G_ON 0
#define B_ON 255

// Constants
int TREASURE_LEVEL1_INITIAL_HP;
int TREASURE_LEVEL1_ACTION_RECV_WAIT;
int TREASURE_LEVEL1_RECOVER_DURATION;

class TreasureLevel1
{
  private:
    int ID;
    int HP;

    unsigned long lastOpenedTime = 0;
     unsigned long lastActionReceived = 0;

  public:
    
    int OG_, ID_, En_, MANA_, action_; 

    TreasureLevel1(int id){
      ID = id;
    }

    void init_treasure(){
      EEPROM.write(ENABLE_add, 1);
      EEPROM.commit();
      HP = TREASURE_LEVEL1_INITIAL_HP;
    };
    
    void receiveAction() {
      int currStatus = EEPROM.read(ENABLE_add);
      if (TreasureLevel1_IR.available()) {
         ir_signal IRsignal_ = TreasureLevel1_IR.read();
         unsigned long currTime = millis();

         if ((currStatus == 1) && (currTime - lastActionReceived > TREASURE_LEVEL1_ACTION_RECV_WAIT)){
          OG_ = IRsignal_.address.digit2;
         ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);

  
           MANA_ = IRsignal_.command.digit1;
           action_ = IRsignal_.command.digit0;
           lastActionReceived = currTime;
      
           Serial.printf("%d %d %d %d \n", action_, MANA_, ID_, OG_);
    
           if (action_ == collect) {
            lastOpenedTime = millis();
            handle_Collected();
           }
          }
         }
      };

    void feedback_collectL1(int OG_, int ID_){
      unsigned long currTime = millis();
      // randomSeed(currTime);
      int powerup_ID = random(1,6);

       TreasureLevel1_EspNOW.send_data(2, OG_, ID_, ID, powerup_ID);

//       while(!last_send_status){
//         delay(200);
//         TreasureLevel1_EspNOW.send_data(2, OG_, ID_, ID, powerup_ID);
//       }

//      TreasureLevel1_EspNOW.send_data(2, OG_, ID_, ID, powerup_ID);
    } ;

    void handle_Collected() {
      interim_collected_display();
      // inform the server here ...
      TreasureLevel1_NeoPixel.off_FRONT();
      TreasureLevel1_NeoPixel.off_TOP();

      feedback_collectL1(OG_, ID_);
      
      int player_identifier = OG_ * pow(16, 2) + ID_;
      Serial.printf("TREASURE%d opened by OG %d ID %d\n", ID, OG_, ID_);
      String player_mac_address = dbc.setTreasureAsOpened("TREASURE" + String(ID), OG_, ID_);
      // this code to save the info of the OG collected the treasure
      EEPROM.write(ENABLE_add, 2);
      EEPROM.commit();
    };

    void recover() {
        // Level1Treasures can recover after a fixed amt of time
        int currStatus = EEPROM.read(ENABLE_add);
        unsigned int currTime = millis();
        if (currStatus == 2 && currTime - lastOpenedTime > TREASURE_LEVEL1_RECOVER_DURATION) {
            Serial.println("Reopening Treasure..");
            EEPROM.write(ENABLE_add, 1);
            HP = TREASURE_LEVEL1_INITIAL_HP;
            TreasureLevel1_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
            TreasureLevel1_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
        }
    };

    void display_not_playing_yet(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 1 Treasure  ")); 
          
      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("Hold on!!");
      display.setCursor(0, 30);
      display.println("The game has not");
      display.println("started yet.");
      display.display();
    };

    void interim_collected_display(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 1 Treasure  ")); 
          
      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("    Please wait ...   ");
      display.display();
    }

    void display_in_game(){
      int currStatus = EEPROM.read(ENABLE_add);
      if(currStatus == 1) {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 1 Treasure  ")); 

        display.setCursor(0, 24);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("   Collect this to   ");
        display.println("   get a power-up!   ");
      
        display.display();
      }
      else {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 1 Treasure  ")); 

        display.setCursor(0, 16);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("  Treasure is Opened  ");

        display.setCursor(0, 30);
        display.println("  Resetting soon...  ");

        display.setCursor(0, 46);
        if (OG_ == ALATAR)
          display.println("OG collected: ALATAR");
        else if (OG_ == DRACHEN)
          display.println("OG collected: DRACHEN");
        else if (OG_ == EVA)
          display.println("OG collected: EVA");
        else if (OG_ == INVICTA)
          display.println("OG collected: INVICTA");
        
        display.display();
      };
    }

};

TreasureLevel1 Treasure(8);  // use OLED to input ID 
bool gameStarted = 0;

void clearEEPROM(){
  int i;
  for (i=0; i<EEPROM_SIZE; i++){
    EEPROM.write(i,0);
  }
}

int get_game_state(){
      // retrieve game state from server
      // 0 mean game did not start
      // 1 mean in game
      // once the game has started then we dunnid to check anymore
   
   if (!gameStarted) {
     gameStarted = dbc.hasGameStarted();
     if(gameStarted){
      TreasureLevel1_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
      TreasureLevel1_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
     }
     return gameStarted;
   } else return 1;
}

void setup() {
  Serial.begin(115200);
  TreasureLevel1_NeoPixel.initialize();
  TreasureLevel1_NeoPixel.off_FRONT();
  TreasureLevel1_NeoPixel.off_TOP();

  TreasureLevel1_IR.enable();
  EEPROM.begin(EEPROM_SIZE);
  clearEEPROM();
  if (EEPROM.read(ENABLE_add) == 0) {
    Treasure.init_treasure();
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }

  TreasureLevel1_EspNOW.enable();
  bool isWiFiConnected = dbc.connectToWiFi();
  while (!isWiFiConnected) {
    Serial.println("Reconnecting..");
    isWiFiConnected = dbc.connectToWiFi();
  }

  GAME_CONSTANTS game_consts = dbc.getGameConstants();
  HTTP_TIMEOUT = game_consts.HTTP_TIMEOUT;
  TREASURE_LEVEL1_INITIAL_HP = game_consts.TREASURE_LEVEL1_INITIAL_HP;
  TREASURE_LEVEL1_ACTION_RECV_WAIT = game_consts.TREASURE_LEVEL1_ACTION_RECV_WAIT;
  TREASURE_LEVEL1_RECOVER_DURATION = game_consts.TREASURE_LEVEL1_RECOVER_DURATION;
  
}

void loop() {
  // First check if ESP is connected to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    bool isWiFiConnected = dbc.connectToWiFi();
    if (!isWiFiConnected) {
        // timeout
        ESP.restart();
    }
  }
  switch (get_game_state()){
    case 0:
      Treasure.display_not_playing_yet();
      delay(10000);
      break;
    case 1:
      Treasure.receiveAction();
      Treasure.recover();
      Treasure.display_in_game();
      break;
  }
}
