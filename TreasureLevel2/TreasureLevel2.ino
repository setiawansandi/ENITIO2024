// upload this code whenever you want to start testing
// comment out the clearEEPROM() function when used on real game

#include <EEPROM.h>
#include "ENITIO_ir.h"
#include "ENITIO_enums.h"
#include "ENITIO_bluetooth.h"
#include "ENITIO_wifi.h"

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

#define initialHP 20
#define hp_pxl_bar_width 5 // [pxl/HP]

#define ENABLE_add 0 // 0 means Treasure has not been initialized, 1 means already initialized
#define ID_add 1
#define HP_add 2
#define collectedOG_add 3

#define ACTION_RECV_WAIT 150 // [ms] 
#define RECOVER_Period 20 // [s]
#define INFECTION_time 10 // [s]

class TreasureLevel2
{
  private:
    int ID;
    int HP;
    bool _isVirus;

    unsigned long lastRecoveredTime = 0;
    unsigned long lastActionReceived = 0;
    unsigned long timeleftToRecover = 0;

  public:
    
    int OG_, ID_, En_, action_; 

    void setup_initial_state(int id){
      ID = id;
      if (ID >= 9) _isVirus = true;
      else _isVirus = false;

      if (EEPROM.read(ENABLE_add) == 0){
      EEPROM.write(ENABLE_add, 1);
      EEPROM.write(ID_add, ID);
      EEPROM.write(HP_add, initialHP);
      EEPROM.commit(); 
      HP = initialHP;}
      else{
        HP = EEPROM.read(HP_add);
        if (HP == 0) {
          OG_ = EEPROM.read(collectedOG_add);
        }
      }

      TreasureLevel2_Bluetooth.initialise(id);
    }

    void receiveAction() {
      if (HP > 0) {
        unsigned long currTime = millis();
        if (TreasureLevel2_IR.available()) {
           ir_signal IRsignal_ = TreasureLevel2_IR.read();

           if (currTime - lastActionReceived > ACTION_RECV_WAIT) {
             OG_ = IRsignal_.address.digit2;
             ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);
        
             En_ = IRsignal_.command.digit1;
             action_ = IRsignal_.command.digit0;
        
             Serial.printf("%d %d %d %d %d\n", action_, En_, ID_, OG_, HP);

             lastActionReceived = currTime;
      
             if (action_ == collect) {
               HP = max(HP-En_, 0);
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

    void recover(){
      if (HP == initialHP) {
        lastRecoveredTime = millis();
      }
      else if (HP > 0){
        unsigned long currTime = millis();
        timeleftToRecover = max(int(RECOVER_Period * 1000 - (currTime - lastRecoveredTime)), 0);
        if (timeleftToRecover == 0){
          HP++;
          lastRecoveredTime = currTime;
          EEPROM.write(HP_add, HP);
          EEPROM.commit(); 
        }
      }
      else if (HP == 0){
         // do nothing
      }    
    }

    void handle_Collected() {
      // no need to feedback everytime the player collecting the Treasure. Only feedback to the server and to the player when the treasure is fully collected, ie. HP == 0
      TreasureLevel2_Bluetooth.stopAdvertisingService(pAvailableService);
      
      // inform the server here ...
      int player_identifier = OG_ * pow(16, 2) + ID_;
      String player_mac_address = dbc.setTreasureAsOpened(TreasureLevel2_Bluetooth.getTreasureName(), player_identifier);
      // this code to save the info of the OG collected the treasure
      EEPROM.write(collectedOG_add, OG_); // save some sent variable to resend if required
      EEPROM.commit(); 
      Serial.print("Treasure opened by "); Serial.println(player_mac_address);
      
      // broadcast virus here ...
      if (_isVirus) {
        Serial.println("Broadcasting Virus...");
        TreasureLevel2_Bluetooth.startAdvertisingService(pVirusService);
        delay(INFECTION_time * 1000);
        TreasureLevel2_Bluetooth.stopAdvertisingService(pVirusService);
        Serial.println("Box Shutting down...");
      }
    }

    void display_not_playing_yet(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 2 Treasure  ")); 
          
      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("Hold on!!");
      display.setCursor(0, 30);
      display.println("The game has not been");
      display.println("started yet.");
      display.display();
    };
    
    void display_in_game(){
      if(HP != 0) {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 2 Treasure  ")); 

        display.setCursor(0, 16);
        display.setTextSize(2);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.print("HP ");
        display.fillRect(0, 34, HP*hp_pxl_bar_width, 4, SSD1306_WHITE);

        display.setCursor(110, 32);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.println(HP);

        display.setCursor(0, 48);
        if (timeleftToRecover > 0) {
          display.print("Recover in: ");
          display.println(int(timeleftToRecover/1000.0)+1);
        }
        display.display();
      }
      else {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 2 Treasure  ")); 

        display.setCursor(0, 16);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("  Congratulations!!  ");

        display.setCursor(0, 30);
        if (!_isVirus) {
          display.println("   It's a Treasure!  ");
        }
        else{
          display.println("    It's a Virus!   ");
        }   

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
      
    void display_game_finished(){};
};

TreasureLevel2 Treasure;
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
     if (gameStarted) {
        // start BLE functions
        TreasureLevel2_Bluetooth.startAdvertisingService(pAvailableService);
     }
     return gameStarted;
   } else return 1;
}

int receive_id(){
      // receive ID from server, need to wait for all the Treasure to subscribe to the server first then the server will randomly assign ID
      int id = 10; // temp
      return id;
}

int id;

void setup() {
  Serial.begin(115200);
  TreasureLevel2_IR.enable();
  EEPROM.begin(EEPROM_SIZE);
  clearEEPROM();  // For testing purposes
  if (EEPROM.read(ENABLE_add) == 0) {
    id = receive_id();
    // Serial.print("DEBUG ID:"); Serial.println(id);
    Treasure.setup_initial_state(id);
  }
  else 
    {
      id = EEPROM.read(ID_add);
      Treasure.setup_initial_state(id);
    }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }
  
  bool isWiFiConnected = dbc.connectToWiFi();
  while (!isWiFiConnected) {
    Serial.println("Reconnecting..");
    isWiFiConnected = dbc.connectToWiFi();
  }
}

void loop() {
  switch (get_game_state()){
    case 0:
      Treasure.display_not_playing_yet();
      break;
    case 1:
      Treasure.receiveAction();
      Treasure.recover();
      Treasure.display_in_game();
      break;
    case 2:
      Treasure.display_game_finished();
      EEPROM.end();
      break;
  }
}
