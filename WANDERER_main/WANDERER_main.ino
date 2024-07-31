#include <EEPROM.h>
#include "ENITIO_enums.h"
#include "ENITIO_const.h"
#include "ENITIO_ir.h"
#include "ENITIO_joystick.h"
#include "ENITIO_buzzer.h"
#include "ENITIO_EspNOW.h"
#include "ENITIO_OLED.h"
#include "ENITIO_NeoPixel.h"
#include "MainMenu.h"
#include "Profile.h"
#include "TreasureHuntPlayer.h"
#include "Admin.h"
#include "Credits.h"
#include "MACAddress.h"
#define LED_BUILTIN 2

TaskHandle_t backgroundTask;

void backgroundTaskCode(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    if (currentProcess == TreasureHuntProcess)
    {
      if (!PLAYER.gameStarted)
      {
        game_started_buffer = dbc.hasGameStarted();
      }
      else if (WIFI_ON)
      {
        PLAYER.gameBackgroundProcess();
      }
      else
      {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
      }
    }
    else
    {
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
  }
};

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  Player_NeoPixel.initialize();

  // check OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  StartUpDisplay();

  // initialize Joystick
  Player_joystick.initialize();

  // initialize IR
  Player_IR.enable();

  Player_EspNOW.enable();

  my_MAC_address = WiFi.macAddress();

  //  here is a set of fixed constants [FOR OFFLINE MODE]
  HTTP_TIMEOUT = 15000;
  EN_RECOVER_DURATION = 5000;
  VIRUS_DECAY_DURATION = 30000;
  VIRUS_IMMUNITY_DURATION = 120000;
  VIRUS_INFECTION_PROBABILITY = 30;
  PARTICIPANT_MaxHP = 12;
  GL_MaxHP = 24;
  SIGMA_MaxHP = 24;
  PARTICIPANT_MaxEn = 12;
  GL_MaxEn = 18;
  SIGMA_MaxEn = 30;
  INITIAL_MULTIPLIER = 1;
  HEAL_MULTIPLIER = 4;
  MAX_ATTACK_MULTIPLIER = 3;
  MAX_COLLECT_MULTIPLIER = 10;
  BOMB_HP_DEDUCTION = 69;
  KILL_UPDATE_SERVER_INTERVAL = 10 * 60 * 1000; // 10 mins
  WIFI_ON = EEPROM.read(ONLINE_mode_add);
  MAX_COOLDOWN = 20; // seconds
  SIGMA_KILLED_SCORE = 10;
  HP_RECOVERY_RATE = 5000; // 1HP/5s

  Serial.print("Actual MAC Address: ");
  Serial.println(my_MAC_address);

  pinMode(LED_BUILTIN, OUTPUT);

  if (EEPROM.read(PLAYER_enable_add) == 0) // will only run after fac. reset or once on the very FIRST flash
    EEPROM.write(CLAN_add, 255);           // 255 to set clan to unknown (default 0 is invicta)

  xTaskCreatePinnedToCore(
      backgroundTaskCode, // Task function.
      "backgroundTask",   // name of task.
      10000,              // Stack size of task
      NULL,               // parameter of the task
      2,                  // priority of the task
      &backgroundTask,    // Task handle to keep track of created task
      0);
}

void loop()
{
  switch (currentProcess)
  {
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
