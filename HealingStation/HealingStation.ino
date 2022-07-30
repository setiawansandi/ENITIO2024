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

#define ENABLE_add 0  // 0 means Treasure has not been initialized, 1 means already initialized

#define ACTION_RECV_WAIT 200 // [ms] 

#define R_ON 255
#define G_ON 0
#define B_ON 0

class HealingStation
{
  private:
    unsigned long last_healing_request = 0;
    
  public:
    
    int OG_, ID_, En_, action_; 

    void init_treasure(){
      EEPROM.write(ENABLE_add, 1);
      EEPROM.commit();
    };
    
    void receiveAction() {
      int currStatus = EEPROM.read(ENABLE_add);
      unsigned long currTime = millis();
      if (currStatus == 1 && HealingStation_IR.available()) {
         ir_signal IRsignal_ = HealingStation_IR.read();

         if (currTime - last_healing_request >= ACTION_RECV_WAIT){
            Serial.println("Initiate Healing Request");
             OG_ = IRsignal_.address.digit2;
             ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);
        
             En_ = IRsignal_.command.digit1;
             action_ = IRsignal_.command.digit0;
        
             Serial.printf("%d %d %d %d \n", action_, En_, ID_, OG_);
      
             if ((action_ == collect) || (action_ == heal_request)) {
                 heal_player(OG_, ID_);
             }
             last_healing_request = currTime;
         }
      }
    };

    void heal_player(int OG_, int ID_){
      HealingStation_EspNOW.send_data(5, OG_, ID_, 0, 0);  // 0 as no powerup ID and healing station has no ID
    } ;

    void display_not_playing_yet(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Healing Station  ")); 
          
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
        display.println(F("   Healing Station  ")); 

        display.setCursor(0, 24);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("   Collect this to   ");
        display.println("    heal your HP!    ");
      
        display.display();
      }
    };

};

HealingStation healing_station;
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
     if(gameStarted){
      HealingStation_NeoPixel.displayRGB_FRONT(R_ON, B_ON, G_ON);
     HealingStation_NeoPixel.displayRGB_TOP(R_ON, B_ON, G_ON);
     }
     return gameStarted;
   } else return 1;
}

void setup() {
  HealingStation_NeoPixel.initialize();
  HealingStation_NeoPixel.off_FRONT();
  HealingStation_NeoPixel.off_TOP();
  Serial.begin(115200);
  HealingStation_IR.enable();
  EEPROM.begin(EEPROM_SIZE);
  clearEEPROM();

  if (EEPROM.read(ENABLE_add) == 0) {
    healing_station.init_treasure();
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }

  HealingStation_EspNOW.enable();
  
  bool isWiFiConnected = dbc.connectToWiFi();
  while (!isWiFiConnected) {
    Serial.println("Reconnecting..");
    isWiFiConnected = dbc.connectToWiFi();
  }

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
      healing_station.display_not_playing_yet();
      break;
    case 1:
      healing_station.receiveAction();
      healing_station.display_in_game();
      break;
  }
}
