#include "ENITIO_enums.h"
#include "ENITIO_const.h"
#include "ENITIO_ir.h"
#include "ENITIO_joystick.h"
#include "ENITIO_buzzer.h"
#include "ENITIO_EspNOW.h"
#include "ENITIO_OLED.h"
#include "ENITIO_player_bluetooth.h"
#include "MainMenu.h"
#include "TreasureHuntPlayer.h"

TaskHandle_t backgroundTask;

void backgroundTaskCode(void * pvParameters){
  for ( ; ; ) {
    PLAYER.gameBackgroundProcess();
    delay(50);
  }
};

void setup() {
      // initialize serial
      Serial.begin(115200);

      // check OLED display
      if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }

      PLAYER.setup_initial_state(1, 0, false); // ID, OG, isGL
      // initialize Joystick 
      Player_joystick.initialize();

      // initialize IR
      Player_IR.enable();
      
      Player_EspNOW.enable();

      dbc.connectToWiFi();
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

  }
  else if (currentProcess == TreasureHuntProcess){
    PLAYER.gameMainLoop();
  }
}
