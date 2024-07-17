#define EEPROM_SIZE 20

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

#define R_ON 0
#define G_ON 0
#define B_ON 0 //255

#define DOUBLE_CLICK_LENGTH 1000 // [ms]

bool AdminFunction = false;
int ID;

int failed_kill_feedback = 0;
int failed_kill_CLAN[50] = {};
int failed_kill_ID[50] = {};
int current_failed_save_pointer = 0;
int current_failed_read_pointer = 0;
bool uploadInProgress = 0;  // boolean required to ensure treasure doesn't recover while WiFi is transmitting
bool can_upload_fail = 1;
int WIFI_ON = 0;
bool parameters_updated = false;

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
#include "ENITIO_treasure_OLED.h"
#include "Admin.h"

bool isBombed = false;

// Constants
int TREASURE_LEVEL1_INITIAL_HP;
int TREASURE_LEVEL1_ACTION_RECV_WAIT;
int TREASURE_RECOVER_DURATION;
int NOTI_SOUND_DURATION = 300;

unsigned long tempNoti_start = 0;
bool clicked_once = 0;
unsigned long last_clicked = 0;

void handleJoystick() {
  joystick_pos joystick_pos = TreasureLevel1_joystick.read_Joystick();
  if (TreasureLevel1_joystick.get_state() == 0) {
    switch (joystick_pos) {
      case button:
        if (clicked_once) {
          AdminFunction = true;
          clicked_once = 0;
        }
        else {
          clicked_once = 1;
          last_clicked = millis();
        }
        TreasureLevel1_joystick.set_state();
        break;

      case idle:
        if (clicked_once) {
          unsigned long currTime = millis();
          if (currTime - last_clicked > DOUBLE_CLICK_LENGTH) clicked_once = 0;
        }
        break;

      default:
        if (clicked_once) {
          unsigned long currTime = millis();
          if (currTime - last_clicked > DOUBLE_CLICK_LENGTH) clicked_once = 0;
        }
        TreasureLevel1_joystick.set_state();
        break;
    }
  }
}

class TreasureLevel1
{
  private:
    int HP;

    int this_recover_duration = TREASURE_RECOVER_DURATION;
    unsigned long lastOpenedTime = 0;
    unsigned long lastActionReceived = 0;

  public:

    int CLAN_, ID_, En_, MULTIPLIER_, action_, channel_;

    void init_treasure() {
      EEPROM.write(ENABLE_add, 1);
      EEPROM.commit();
      HP = TREASURE_LEVEL1_INITIAL_HP;
    };

    void receiveAction() {
      int currStatus = EEPROM.read(ENABLE_add);
      if (TreasureLevel1_IR.available()) {
        ir_signal IRsignal_ = TreasureLevel1_IR.read();
        unsigned long currTime = millis();
        if ((currStatus == 1) && (currTime - lastActionReceived > TREASURE_LEVEL1_ACTION_RECV_WAIT)) {
          Serial.printf("RECV %d %d %d %d | %d %d %d %d \n", IRsignal_.address.digit3, IRsignal_.address.digit2, IRsignal_.address.digit1, IRsignal_.address.digit0, IRsignal_.command.digit3, IRsignal_.command.digit2, IRsignal_.command.digit1, IRsignal_.command.digit0);
          CLAN_ = IRsignal_.address.digit2;
          ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);

          channel_ = IRsignal_.command.digit2;
          MULTIPLIER_ = IRsignal_.command.digit1;
          action_ = IRsignal_.command.digit0;
          lastActionReceived = currTime;

          if (action_ == collect) {
            int randomValue = random(100);  // Generate a random number between 0 and 99 (inclusive)

            if (randomValue < 50) {  // chance in %
              handle_Bomb();
            } else {
              handle_Collected();
            }

            // SET treasure chest COOLDOWN
            lastOpenedTime = millis();
            this_recover_duration = TREASURE_RECOVER_DURATION * random(1, 9);
          }
        }
      }
    };

    void feedback_collectL1(int CLAN_, int ID_, int channel_) {
      unsigned long currTime = millis();
      int powerup_ID = random(1, 6);
      TreasureLevel1_EspNOW.send_data(2, CLAN_, ID_, ID, true, powerup_ID, channel_);
    } ;

    void handle_Bomb() {
      Serial.println("Fake treasure, dropping Bomb!");

      isBombed = true;

      EEPROM.write(ENABLE_add, 2);
      EEPROM.commit();

      for(int i = 0; i < 3; ++i) {
        Treasure_Buzzer.sound(NOTE_C2);
        displayBombed();
        TreasureLevel1_NeoPixel.displayRGB_FRONT(5, 0, 0);
        TreasureLevel1_NeoPixel.displayRGB_TOP(5, 0, 0);
        delay(600);

        Treasure_Buzzer.end_sound();
        displayClear();
        TreasureLevel1_NeoPixel.off_FRONT();
        TreasureLevel1_NeoPixel.off_TOP();
        delay(600);
      }
      
    }

    void handle_Collected() {
      interim_collected_display();

      // inform the server here ...
      TreasureLevel1_NeoPixel.off_FRONT();
      TreasureLevel1_NeoPixel.off_TOP();

      Serial.printf("TREASURE%d opened by CLAN %d ID %d\n", ID, CLAN_, ID_);
      feedback_collectL1(CLAN_, ID_, channel_);
      int player_identifier = CLAN_ * pow(16, 2) + ID_;

      EEPROM.write(ENABLE_add, 2);
      switch (CLAN_)
      {
        case INVICTA:
          EEPROM.write(INVICTA_add, EEPROM.read(INVICTA_add) + 1);
          break;

        case DYNARI:
          EEPROM.write(DYNARI_add, EEPROM.read(DYNARI_add) + 1);
          break;

        case EPHILIA:
          EEPROM.write(EPHILIA_add, EEPROM.read(EPHILIA_add) + 1);
          break;

        case AKRONA:
          EEPROM.write(AKRONA_add, EEPROM.read(AKRONA_add) + 1);
          break;

        case SOLARIS:
          EEPROM.write(SOLARIS_add, EEPROM.read(SOLARIS_add) + 1);
          break;

        default:
          break;
      }
      EEPROM.commit();

      Treasure_Buzzer.sound(NOTE_DS1);
      tempNoti_start = millis();

      if (WIFI_ON) {
        dbc.setTreasureAsOpened("TREASURE" + String(ID), CLAN_, ID_);
      }
    };

    void recover() {
      // Level1Treasures can recover after a fixed amt of time
      int currStatus = EEPROM.read(ENABLE_add);
      unsigned int currTime = millis();

      if (currStatus == 2 && currTime - lastOpenedTime > this_recover_duration && !uploadInProgress) {
        Serial.println("Reopening Treasure..");
        isBombed = false;
        EEPROM.write(ENABLE_add, 1);
        HP = TREASURE_LEVEL1_INITIAL_HP;
        TreasureLevel1_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
        TreasureLevel1_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
      }
    };

    void display_not_playing_yet() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("    Treasure Chest   "));

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
      display.println(F("    Treasure Chest   "));

      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("    Please wait ...   ");
      display.display();
    }

    void display_in_game() {
      int currStatus = EEPROM.read(ENABLE_add);
      if (currStatus == 1) {
        displayTreasure();
      }
      else{
        if (isBombed == 1) {
          displayCombusted();
        }
        else {
          displayTreasureLooted(CLAN_);
        }
      } 
    }
};

TreasureLevel1 Treasure;  // use OLED to input ID
bool gameStarted = 0;

void clearEEPROM() {
  int i;
  for (i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void update_sound()
{
  unsigned long currTime = millis();
  if (currTime - tempNoti_start >= NOTI_SOUND_DURATION)
  {
    Treasure_Buzzer.end_sound();
  }
}

bool setUpDone = 0;
int get_game_state() {
  // retrieve game state from server
  // 0 mean game did not start
  // 1 mean in game
  if (!parameters_updated) {
    Serial.println("[BACKGROUND] Connecting to WiFi to retrieve latest parameters..");
    bool isWiFiConnected = dbc.connectToWiFi();
    if (isWiFiConnected) {
      GAME_CONSTANTS game_consts = dbc.getGameConstants();
      HTTP_TIMEOUT = game_consts.HTTP_TIMEOUT;
      TREASURE_LEVEL1_INITIAL_HP = game_consts.TREASURE_LEVEL1_INITIAL_HP;
      TREASURE_LEVEL1_ACTION_RECV_WAIT = game_consts.TREASURE_LEVEL1_ACTION_RECV_WAIT;
      TREASURE_RECOVER_DURATION = game_consts.TREASURE_LEVEL1_RECOVER_DURATION;
      parameters_updated = true;
      Serial.println("[BACKGROUND] Parameters Retrieved");
    } else {
      Serial.println("[BACKGROUND] Update Unsuccessful");
      return 0;
    }
  }

  if (gameStarted) return 1;  // once the game has started then we dunnid to check anymore
  else {
    gameStarted = dbc.hasGameStarted();
    if (gameStarted) {
      Serial.println("[BACKGROUND] Game has started! Initialising Treasure..");
      TreasureLevel1_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
      TreasureLevel1_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
      Treasure.init_treasure();
      setUpDone = 1;
      WiFi.disconnect();
    }
    return gameStarted;
  }
}

TaskHandle_t backgroundTask;

void backgroundTaskCode(void * pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for ( ; ; ) {
    if (!WIFI_ON) {
      // offline mode
      if (!setUpDone) {
        TreasureLevel1_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
        TreasureLevel1_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
        Treasure.init_treasure();
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
  randomSeed(analogRead(0));

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  StartUpDisplay();
  Serial.begin(115200);
  TreasureLevel1_NeoPixel.initialize();
  TreasureLevel1_NeoPixel.off_FRONT();
  TreasureLevel1_NeoPixel.off_TOP();

  TreasureLevel1_IR.enable();
  TreasureLevel1_EspNOW.enable();
  EEPROM.begin(EEPROM_SIZE);
  ID = EEPROM.read(ID_add);
  WIFI_ON = EEPROM.read(ONLINE_mode_add);

  // Hardcoded constants, in case there is no WiFi to update
  HTTP_TIMEOUT = 30000;
  TREASURE_LEVEL1_INITIAL_HP = 1;
  TREASURE_LEVEL1_ACTION_RECV_WAIT = 3000;
  TREASURE_RECOVER_DURATION = 2000;

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
  if (AdminFunction) {
    TreasureLevel1_Admin.AdminLoop();
  } 
  else if (!WIFI_ON) {
    // offline mode
    handleJoystick();
    Treasure.display_in_game();
    Treasure.receiveAction();
    Treasure.recover();
    update_sound();
  } 
  else {
    // online mode
    handleJoystick();
    if (setUpDone) {
      Treasure.display_in_game();
      Treasure.receiveAction();
      Treasure.recover();
      update_sound();
    } else {
      Treasure.display_not_playing_yet();
    }
  }
}
