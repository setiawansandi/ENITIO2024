#include <EEPROM.h>

#include "ENITIO_enums.h"
#include "ENITIO_const.h"
#include "ENITIO_ir.h"
#include "ENITIO_joystick.h"
#include "ENITIO_buzzer.h"
#include "ENITIO_EspNOW.h"
#include "ENITIO_OLED.h"
#include "ENITIO_player_bluetooth.h"

#include "MainMenu.h"
#include "Profile.h"
#include "TreasureHuntPlayer.h"

TaskHandle_t backgroundTask;

void backgroundTaskCode(void * pvParameters){
  for ( ; ; ) {
    if (currentProcess == TreasureHuntProcess){
      if (!PLAYER.gameStarted){
        game_started_buffer = dbc.hasGameStarted();
      }
      else{
        PLAYER.gameBackgroundProcess();
      }
    }
    else{
      delay(50);
    }
  }
};

void clearEEPROM(){
  int i;
  for (i=0; i<EEPROM_SIZE; i++){
    EEPROM.write(i,0);
  }
  EEPROM.commit();
};

void setup() {
      // initialize serial
      Serial.begin(115200);
      EEPROM.begin(EEPROM_SIZE);

      clearEEPROM();

      // check OLED display
      if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }

      StartUpDisplay();
      
      // initialize Joystick 
      Player_joystick.initialize();

      // initialize IR
      Player_IR.enable();
      
      Player_EspNOW.enable();

      dbc.connectToWiFi();
      my_MAC_address = WiFi.macAddress();

      
      Player_Bluetooth.initialise();

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
  if (currentProcess == MainMenuProcess){
    My_MainMenu.MainMenuLoop();
  }
  else if (currentProcess == TreasureHuntProcess){
    PLAYER.gameMainLoop();
  }
  else if (currentProcess == ProfileProcess){
    My_Profile.ProfileLoop();
  }
}
