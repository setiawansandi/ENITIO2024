// upload this code whenever you want to start testing
// comment out the clearEEPROM() function when used on real game

#include <EEPROM.h>
#include "ENITIO_ir.h"
#include "ENITIO_enums.h"
#include "ENITIO_ESPNOW.h"
#include "ENITIO_NeoPixel.h"
#include "ENITIO_joystick.h"

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

#define ENABLE_add 0  // 0 means Treasure has not been initialized, 1 means already initialized

#define R_ON 255
#define G_ON 0
#define B_ON 0


int REVIVAL_STATION_INITIAL_HP;
int REVIVAL_STATION_ACTION_RECV_WAIT;
int REVIVAL_STATION_RECOVER_DURATION;
int REVIVAL_MULTIPLIER;


class RevivalStation
{
  private:
    int HP;
    unsigned long last_revival_request = 0;
    unsigned long timeleftToRecover = 0;
    
  public:
    
    int CLAN_, ID_, En_, action_; 

    void init_treasure(){
      EEPROM.write(ENABLE_add, 1);
      EEPROM.commit();
      HP = REVIVAL_STATION_INITIAL_HP;
    };
    
    // void receiveAction() {
    //   int currStatus = EEPROM.read(ENABLE_add);
    //   unsigned long currTime = millis();
    //   if (currStatus == 1 && HealingStation_IR.available()) {
    //       ir_signal IRsignal_ = HealingStation_IR.read();

    //       if (currTime - last_revival_request >= HEALING_STATION_ACTION_RECV_WAIT){
    //           Serial.println("Initiate Healing Request");
    //           CLAN_ = IRsignal_.address.digit2;
    //           ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);
        
    //           En_ = IRsignal_.command.digit1;
    //           action_ = IRsignal_.command.digit0;
        
    //           Serial.printf("%d %d %d %d \n", action_, En_, ID_, CLAN_);
      
    //           if ((action_ == collect) || (action_ == revive)) {
    //               heal_player(CLAN_, ID_);
    //               HP--;
    //               Serial.print(HP); Serial.println(" Before Station Closes");
    //               if (HP == 0) {
    //                   Serial.println("Closing Healing Station");
    //                   HealingStation_NeoPixel.off_FRONT();
    //                   HealingStation_NeoPixel.off_TOP();
    //                   healing_station.display_no_hp();
    //                   EEPROM.write(ENABLE_add, 2);
    //                   EEPROM.commit();
    //               }
    //           }
    //           last_revival_request = currTime;
    //       }
    //   } else if (currStatus == 2) {
    //       timeleftToRecover = max(int(HEALING_STATION_RECOVER_DURATION - (currTime - last_revival_request)), 0);
    //       if (timeleftToRecover == 0) {
    //         Serial.println("Reopening Healing Station");
    //         HP = HEALING_STATION_INITIAL_HP;
    //         EEPROM.write(ENABLE_add, 1);
    //         EEPROM.commit();
    //         HealingStation_NeoPixel.displayRGB_FRONT(R_ON, B_ON, G_ON);
    //         HealingStation_NeoPixel.displayRGB_TOP(R_ON, B_ON, G_ON);
    //       }
    //   }
    // };

    void RevivalLoop(){
      int currStatus = EEPROM.read(ENABLE_add);
      unsigned long currTime = millis();
      if (currStatus == 1){
        handleJoystick();
      }
      else if (currStatus == 2){
        timeleftToRecover = max(int(REVIVAL_STATION_RECOVER_DURATION - (currTime - last_revival_request)), 0);
        display_no_hp(timeleftToRecover);
        if (timeleftToRecover == 0) {
          Serial.println("Reopening REVIVAL Station");
          HP = REVIVAL_STATION_INITIAL_HP;
          EEPROM.write(ENABLE_add, 1);
          EEPROM.commit();
          RevivalStation_NeoPixel.displayRGB_FRONT(R_ON, B_ON, G_ON);
          RevivalStation_NeoPixel.displayRGB_TOP(R_ON, B_ON, G_ON);
        }
      }
    }

    void handleJoystick(){
      joystick_pos joystick_pos = RevivalStation_joystick.read_Joystick();
      if (RevivalStation_joystick.get_state() == 0) {
        switch (joystick_pos){
            case button:
              revive_player();
              RevivalStation_joystick.set_state();
              HP--;
              Serial.print(HP); Serial.println(" Before Station Closes");
              if (HP == 0) {
                  Serial.println("Closing Revival Station");
                  RevivalStation_NeoPixel.off_FRONT();
                  RevivalStation_NeoPixel.off_TOP();
                  EEPROM.write(ENABLE_add, 2);
                  EEPROM.commit();
                  last_revival_request = millis();
              }
              break;

            case idle:
              break;

            default:
              RevivalStation_joystick.set_state();
              break;
          }
      }
    }

    void revive_player(){
      uint16_hex_digits address_digits, command_digits;

      address_digits.digit0 = 0;
      address_digits.digit2 = 0;

      command_digits.digit0 = revive;

      command_digits.digit1 = REVIVAL_MULTIPLIER;

      ir_signal send_signal;

      send_signal.address = address_digits;
      send_signal.command = command_digits;

      RevivalStation_IR.send(send_signal, 1);
    } ;

    void display_not_playing_yet(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Revival Station  ")); 
          
      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("Hold on!!");
      display.setCursor(0, 30);
      display.println("The game has not");
      display.println("started yet.");
      display.display();
    };

    void display_in_game(){
      int currStatus = EEPROM.read(ENABLE_add);
      if(currStatus == 1) {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Revival Station  ")); 

        display.setCursor(0, 20);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("  Put your WANDERer  ");
        display.setCursor(0, 32);
        display.println(" towards the Station ");
        display.setCursor(0, 44);
        display.println("      and PRESS!     ");
      
        display.display();
      }
    };

    void display_no_hp(int this_timeleftToRecover){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Revival Station  ")); 
          
      display.setCursor(0, 24);
      display.setTextColor(SSD1306_WHITE); // Draw white text              |  
      display.println("     Out of HP      ");

      display.setCursor(0, 40);
      display.print("Come back in: ");
      display.println(int(this_timeleftToRecover/1000.0)+1);
      display.display();
    };

};

RevivalStation revival_station;
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
     Serial.print("Game Status: "); Serial.println(gameStarted);
     if(gameStarted) {
       RevivalStation_NeoPixel.displayRGB_FRONT(R_ON, B_ON, G_ON);
       RevivalStation_NeoPixel.displayRGB_TOP(R_ON, B_ON, G_ON);
       revival_station.init_treasure();
     }
     return gameStarted;
   } else return 1;
}

void setup() {
  RevivalStation_NeoPixel.initialize();
  RevivalStation_NeoPixel.off_FRONT();
  RevivalStation_NeoPixel.off_TOP();
  Serial.begin(115200);
  RevivalStation_IR.enable();
  EEPROM.begin(EEPROM_SIZE);
  clearEEPROM();

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }

  RevivalStation_EspNOW.enable();
  
  bool isWiFiConnected = dbc.connectToWiFi();
  while (!isWiFiConnected) {
    Serial.println("Reconnecting..");
    isWiFiConnected = dbc.connectToWiFi();
  }

  GAME_CONSTANTS game_consts = dbc.getGameConstants();
  HTTP_TIMEOUT = game_consts.HTTP_TIMEOUT;
  REVIVAL_STATION_INITIAL_HP = game_consts.REVIVAL_STATION_INITIAL_HP;
  REVIVAL_STATION_ACTION_RECV_WAIT = game_consts.REVIVAL_STATION_ACTION_RECV_WAIT;
  REVIVAL_STATION_RECOVER_DURATION = game_consts.REVIVAL_STATION_RECOVER_DURATION;
  REVIVAL_MULTIPLIER = game_consts.REVIVAL_STATION_MULTIPLIER;
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
      revival_station.display_not_playing_yet();
      delay(10000);
      break;
    case 1:
      revival_station.RevivalLoop();
      revival_station.display_in_game();
      break;
  }
}
