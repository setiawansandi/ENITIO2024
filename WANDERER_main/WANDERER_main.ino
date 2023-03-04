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
#include "Admin.h"
#include "Credits.h"
#include "MACAddress.h"
#define LED_BUILTIN 2

#define LED_BUILTIN 2

TaskHandle_t backgroundTask;


void backgroundTaskCode(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        if (currentProcess == TreasureHuntProcess) {
            if (!PLAYER.gameStarted) {
                game_started_buffer = dbc.hasGameStarted();
            } else {
                PLAYER.gameBackgroundProcess();
            }
        } else {
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
        }
    }
};

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  // initialize serial
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

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

  my_MAC_address = WiFi.macAddress();

  //  here is a set of fixed constants
  HTTP_TIMEOUT = 15000;
  EN_RECOVER_DURATION = 5000;
  VIRUS_DECAY_DURATION = 30000;
  VIRUS_IMMUNITY_DURATION = 120000;
  VIRUS_INFECTION_PROBABILITY = 30;
  PARTICIPANT_MaxHP = 12;
  GL_MaxHP = 50;
  PARTICIPANT_MaxEn = 5;
  GL_MaxEn = 50;
  INITIAL_MULTIPLIER = 1;
  HEAL_MULTIPLIER = 4;
  MAX_ATTACK_MULTIPLIER = 3;
  MAX_COLLECT_MULTIPLIER = 10;
  BOMB_HP_DEDUCTION = 6;
  KILL_UPDATE_SERVER_INTERVAL = 120000;
    
  Serial.print("Actual MAC Address: "); Serial.println(my_MAC_address);

  pinMode(LED_BUILTIN, OUTPUT);

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
  // First check if ESP is connected to WiFi
  if ((WiFi.status() != WL_CONNECTED) && (millis() - last_disconnected_time > 2000)) {
    Serial.println("Lost WiFi Connection.. attempting to reconnect");
    digitalWrite(LED_BUILTIN, HIGH); // lights up the built-in LED when the WiFi connection is lost
    dbc.startWiFiConnection();
    last_disconnected_time = millis();
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  switch (currentProcess) {
    case MainMenuProcess:
      My_MainMenu.MainMenuLoop();
      break;
    case TreasureHuntProcess:
      PLAYER.gameMainLoop();
      break;
    case ProfileProcess:
      My_Profile.ProfileLoop();
      break;
    case AdminProcess:
      My_Admin.AdminLoop();
      break;
    case CreditProcess:
      My_Credits.CreditsLoop();
      break;
    case MACAddressProcess:
      My_MACAddress.MACAddressLoop();
      break;
    default:
      currentProcess = MainMenuProcess;
      break;
  }
}

