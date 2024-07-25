#define EEPROM_SIZE 11

#define ID_BOUND 50

#define ENABLE_add 0 // 0 means Treasure has not been initialized, 1 means already initialized
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
#define G_ON 5
#define B_ON 0

#define R_OFF 5
#define G_OFF 0
#define B_OFF 0

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
bool uploadInProgress = 0; // boolean required to ensure treasure doesn't recover while WiFi is transmitting
bool can_upload_fail = 1;
int WIFI_ON;
bool parameters_updated = false;
int AKRONA_TREAS = 0;
int INVICTA_TREAS = 0;
int DYNARI_TREAS = 0;
int EPHILIA_TREAS = 0;
int SOLARIS_TREAS = 0;

int action = 0;
int currentMenu = MainMenuProcess;
int display_nav_bar = 0;
int clan_pointer = 0;
int BaseClanValue;

bool clicked_once = 0;
unsigned long last_clicked = 0;
unsigned long lastRecoveredTime = 0;
bool ProfileFunction = false;

const unsigned long DISPLAY_HP_DURATION = 15000;
bool depositEvent = false;

// stub functions
void StartUpDisplay();
void clearEEPROM();

#include <EEPROM.h>
#include "ENITIO_ir.h"
#include "ENITIO_enums.h"
#include "ENITIO_ESPNOW.h"
#include "ENITIO_NeoPixel.h"
#include "ENITIO_joystick.h"
#include "ENITIO_Profile.h"
#include "ENITIO_treasureBase_OLED.h"
#include "ENITIO_buzzer.h"
#include "Admin.h"

// Constants
int TREASURE_BASE_INITIAL_HP = 10;
int TREASURE_BASE_ACTION_RECV_WAIT = 1500;
int TREASURE_BASE_RECOVER_DURATION = 20000;

void handleJoystick()
{
  joystick_pos joystick_pos = TreasureBase_joystick.read_Joystick();
  if (TreasureBase_joystick.get_state() == 0)
  {
    switch (joystick_pos)
    {
    case right:
      display_nav_bar++;
      display_nav_bar = min(display_nav_bar, 2); // Limit display_nav_bar to 1 for Admin
      TreasureBase_joystick.set_state();
      break;

    case left:
      display_nav_bar = max(display_nav_bar - 1, 0); // Always show Main Menu when moving left
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
        if (currTime - last_clicked > DOUBLE_CLICK_LENGTH)
          clicked_once = 0;
        TreasureBase_joystick.get_state();
      }
      break;

    default:
      if (clicked_once)
      {
        unsigned long currTime = millis();
        if (currTime - last_clicked > DOUBLE_CLICK_LENGTH)
          clicked_once = 0;
      }
      TreasureBase_joystick.set_state();
      break;
    }
  }
};

void handleButtonPress()
{
  unsigned long current_time = millis();
  switch (display_nav_bar)
  {
  case 0:
    AdminFunction = false;
    ProfileFunction = false;
    break;
  case 1:
    AdminFunction = true;
    ProfileFunction = false;
    last_clicked = current_time;
    break;
  case 2:
    ProfileFunction = true;
    AdminFunction = false;
    last_clicked = current_time;
  }
}

class TreasureBase
{
private:
  int HP;

  int recovery_time = 0;
  unsigned long lastActionReceived = 0;
  unsigned long lastAttackedTime = INT_MIN;

public:
  int CLAN_, ID_, En_, MULTIPLIER_, action_, channel_, Treas_Deposit_;

  void init_treasure()
  {
    EEPROM.write(ENABLE_add, 1);
    EEPROM.commit();
    HP = TREASURE_BASE_INITIAL_HP;
  }

  void receiveAction()
  {
    unsigned long currTime = millis();

    if (currTime - lastActionReceived > TREASURE_BASE_ACTION_RECV_WAIT)
    {
      TreasureBase_NeoPixel.off_FRONT();
      TreasureBase_NeoPixel.off_TOP();
    }

    int currStatus = EEPROM.read(ENABLE_add);
    if (TreasureBase_IR.available())
    {
      ir_signal IRsignal_ = TreasureBase_IR.read();
      if ((currStatus == 1) && (currTime - lastActionReceived > TREASURE_BASE_ACTION_RECV_WAIT))
      {
        // Serial.printf("command digit one received %d\n",IRsignal_.command.digit1);
        Serial.printf("RECV %d %d %d %d | %d %d %d %d \n", IRsignal_.address.digit3, IRsignal_.address.digit2, IRsignal_.address.digit1, IRsignal_.address.digit0, IRsignal_.command.digit3, IRsignal_.command.digit2, IRsignal_.command.digit1, IRsignal_.command.digit0);
        CLAN_ = IRsignal_.address.digit2;
        ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);

        channel_ = IRsignal_.command.digit2;
        Treas_Deposit_ = IRsignal_.command.digit3; // Extracting the upper 4 bits
        MULTIPLIER_ = IRsignal_.command.digit1;    // Extracting the lower 4 bits
        action_ = IRsignal_.command.digit0;
        lastActionReceived = currTime;

        if (CLAN_ == BaseClanValue)
        {
          if (action_ == collect)
          {
            handleDeposit(CLAN_, ID_, Treas_Deposit_, MULTIPLIER_, channel_);
          }
        }
        else if (CLAN_ != BaseClanValue)
        {
          if (action_ == attack)
          {
            handleAttacked(CLAN_, ID_, MULTIPLIER_, channel_);
          }
        }
      }
    }
  }

  void handleDeposit(int CLAN_, int ID_, int Treas_Deposit_, int MULTIPLIER_, int channel_)
  {
    depositEvent = true;

    int treasureCount = Treas_Deposit_ % 16;

    interim_collected_display();

    TreasureBase_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
    TreasureBase_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);

    Serial.printf("TREASURE%d deposited by CLAN %d ID %d\n", ID, CLAN_, ID_);

    switch (CLAN_)
    {
    case INVICTA:
      EEPROM.write(INVICTA_add, EEPROM.read(INVICTA_add) + treasureCount);
      INVICTA_TREAS = EEPROM.read(INVICTA_add);
      break;

    case DYNARI:
      EEPROM.write(DYNARI_add, EEPROM.read(DYNARI_add) + treasureCount); // EEPROM.write(address, value)
      DYNARI_TREAS = EEPROM.read(DYNARI_add);
      break;

    case EPHILIA:
      EEPROM.write(EPHILIA_add, EEPROM.read(EPHILIA_add) + treasureCount);
      EPHILIA_TREAS = EEPROM.read(EPHILIA_add);
      break;

    case AKRONA:
      EEPROM.write(AKRONA_add, EEPROM.read(AKRONA_add) + treasureCount);
      AKRONA_TREAS = EEPROM.read(AKRONA_add);
      break;

    case SOLARIS:
      EEPROM.write(SOLARIS_add, EEPROM.read(SOLARIS_add) + treasureCount);
      SOLARIS_TREAS = EEPROM.read(SOLARIS_add);
      break;

    default:
      break;
    }
    EEPROM.commit();

    // to clear the inventory of the treasures collected by Wanderers
    sendDepositAction(CLAN_, ID_, MULTIPLIER_, action_, channel_);
  }

  void handleAttacked(int CLAN_, int ID_, int MULTIPLIER_, int channel_)
  {
    Serial.print("Attacked, HP: ");
    HP = max(HP - MULTIPLIER_, 0);
    EEPROM.write(HP_add, HP);
    Serial.println(HP);
    if (HP == 0)
    {
      EEPROM.write(ENABLE_add, 2);

      display_base_destroyed();

      for (int i = 0; i < 3; ++i)
      {
        TreasureBase_Buzzer.sound(NOTE_C2);
        TreasureBase_NeoPixel.displayRGB_FRONT(R_OFF, G_OFF, B_OFF);
        TreasureBase_NeoPixel.displayRGB_TOP(R_OFF, G_OFF, B_OFF);
        delay(600);

        TreasureBase_Buzzer.end_sound();
        TreasureBase_NeoPixel.off_FRONT();
        TreasureBase_NeoPixel.off_TOP();
        delay(600);
      }

      // set what the base recover duration
      recovery_time = TREASURE_BASE_RECOVER_DURATION + millis();
      Serial.print("Treasure will respawn at ");
      Serial.print(recovery_time);
      Serial.print(" Current time: ");
      Serial.println(millis());
    }
    lastAttackedTime = millis();

    EEPROM.commit();
  }

  void sendDepositAction(int CLAN_, int ID_, int MULTIPLIER_, int action_, int channel_)
  {
    uint16_hex_digits address_digits, command_digits;

    int This_Action_Multiplier = MULTIPLIER_; // Set the multiplier to the treasure deposit amount
    int ID = ID_;
    int CLAN = CLAN_;
    int action = action_;

    address_digits.digit0 = ID % 16;
    address_digits.digit1 = ID / 16;
    address_digits.digit2 = CLAN;

    command_digits.digit2 = channel_;               // Assuming WiFi channel or similar is not needed
    command_digits.digit1 = This_Action_Multiplier; // Set the multiplier for the deposit

    // Ensure the action is set to deposit
    switch (action_)
    {
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

  void recover()
  {
    // Level1Treasures can recover after a fixed amt of time
    int currStatus = EEPROM.read(ENABLE_add);
    unsigned int currTime = millis();

    if (currStatus == 2 && currTime > recovery_time && !uploadInProgress)
    {
      Serial.print("Respawning Base.. | Current-time: ");
      Serial.println(millis());
      EEPROM.write(ENABLE_add, 1);
      HP = TREASURE_BASE_INITIAL_HP;
    }
  };

  void display_not_playing_yet()
  {
    display.clearDisplay();
    display.setTextSize(1);                             // Draw SIZE
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

  void interim_collected_display()
  {
    display.clearDisplay();
    display.setTextSize(1);                             // Draw SIZE
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
    display.setCursor(0, 0);
    display.println("    Treasure Base    ");

    display.setCursor(0, 16);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.println("    Please wait ...   ");
    display.display();
  }

  void display_default_screen()
  {
    int currStatus = EEPROM.read(ENABLE_add);
    if (currStatus == 1)
    {
      display.clearDisplay();
      display.setTextSize(1);                             // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println("    Treasure Base    ");

      display.setCursor(0, 18);
      display.setTextSize(2);              // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
   
      BaseClanValue = EEPROM.read(CLAN_add);

      switch (BaseClanValue)
      {
      case 0:
        display.print("  INVICTA  ");
        break;
      case 1:
        display.print("  DYNARI  ");
        break;
      case 2:
        display.print("  EPHILIA  ");
        break;
      case 3:
        display.print("  AKRONA  ");
        break;
      case 4:
        display.print("  SOLARIS  ");
        break;
      default:
        display.print("  UNKNOWN  ");
        break;
      }

      display.setCursor(0, 38);
      display.setTextSize(1);              // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("  Hold   to Deposit  ");
      displayNavBar();

      display.display();
    }
    // else
    // {
    //   display.clearDisplay();
    //   display.setTextSize(1);                             // Draw SIZE
    //   display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
    //   display.setCursor(0, 0);
    //   display.println("    Treasure Base    ");

    //   display.setCursor(0, 12);
    //   display.setTextSize(1);
    //   display.setTextColor(SSD1306_WHITE); // Draw white text
    //   display.println("   Clan Deposited:  ");

    //   display.setCursor(0, 25); // sets the row and the column
    //   display.setTextSize(1);   // set the size of the text
    //   if (CLAN_ == INVICTA)

    //   {
    //     display.println("      INVICTA       ");
    //     display.println("  ");
    //     display.println("    Total Tressure  ");
    //     display.setCursor(50, 55);
    //     display.println(INVICTA_TREAS);
    //   }

    //   else if (CLAN_ == DYNARI)

    //   {
    //     display.println(" ");
    //     display.print("       DYNARI       ");
    //     display.println("  ");
    //     display.println("    Total Tressure  ");
    //     display.setCursor(50, 55);
    //     display.println(DYNARI_TREAS);
    //   }

    //   else if (CLAN_ == EPHILIA)
    //   {
    //     display.println(" ");
    //     // display.setCursor(20, 30); // sets the row and the column
    //     display.println("      EPHILIA       ");
    //     display.println("    Total Tressure  ");
    //     display.setCursor(50, 55);
    //     display.println(EPHILIA_TREAS);
    //   }

    //   else if (CLAN_ == AKRONA)
    //   {
    //     display.println("       AKRONA       ");
    //     display.println("  ");
    //     display.println("    Total Tressure  ");
    //     display.setCursor(50, 55);
    //     display.println(AKRONA_TREAS);
    //   }

    //   else if (CLAN_ == SOLARIS)
    //   {
    //     display.println("      SOLARIS       ");
    //     display.println("      ");
    //     display.println("    Total Tressure  ");
    //     display.setCursor(55, 55);
    //     display.println(SOLARIS_TREAS);
    //   }
    // }
  }

  int lastHP = -1;
  void display_hp_screen()
  {
    unsigned long currentMillis = millis();

    if (HP > 0 && HP != lastHP)
    {
      lastHP = HP;

      display.clearDisplay();
      display.setTextSize(1);                             // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println("    Treasure Base    ");

      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("   Base Attacked!!   ");
      display.display();

      // Display HP as text
      display.setCursor(0, 32);
      display.print("HP: ");
      display.print(HP);

      // Display HP bar
      int barWidth = map(HP, 0, 10, 0, 128);                // Assuming max HP is 100 and screen width is 128
      display.drawRect(0, 48, 128, 16, SSD1306_WHITE);      // Draw outline
      display.fillRect(0, 48, barWidth, 16, SSD1306_WHITE); // Fill based on HP

      display.display();
    }
    else if (HP <= 0)
    {
      display_base_destroyed();
    }
  }

  void display_base_destroyed()
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 16);
    display.println(" Base");
    display.println(" Destroyed");
    display.display();
  }

  void display_in_game()
  {
    unsigned long currentMillis = millis();

    if (depositEvent || currentMillis - lastAttackedTime >= DISPLAY_HP_DURATION)
    {
      depositEvent = false;
      display_default_screen();
    }
    else
    {
      display_hp_screen();
    }
  }
};

TreasureBase treasureBase; // use OLED to input ID
bool gameStarted = 0;

void clearEEPROM()
{
  int i;
  for (i = 0; i < EEPROM_SIZE; i++)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

bool setUpDone = 0;
int get_game_state()
{
  // Directly set game parameters without connecting to WiFi or checking server
  if (!parameters_updated)
  {
    Serial.println("[BACKGROUND] Setting game parameters locally...");

    // Set your game parameters directly

    Serial.println("[BACKGROUND] Parameters Set Locally");
  }

  // Assuming the game has started
  if (gameStarted)
  {
    treasureBase.init_treasure();
    return 1; // once the game has started then we don't need to check anymore
  }
  else
  {
    gameStarted = true; // Directly set gameStarted to true

    Serial.println("[BACKGROUND] Game has started! Initialising Treasure..");
    treasureBase.init_treasure();
    setUpDone = 1;

    // No need to disconnect WiFi as we're not using it
    // WiFi.disconnect();

    return gameStarted;
  }
}

TaskHandle_t backgroundTask;

void backgroundTaskCode(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    if (!WIFI_ON)
    {
      // offline mode
      if (!setUpDone)
      {
        TreasureBase_NeoPixel.displayRGB_FRONT(0, 0, 5);
        TreasureBase_NeoPixel.displayRGB_TOP(0, 0, 5);
        treasureBase.init_treasure();
        setUpDone = 1;
      }
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
    else
    {
      get_game_state();

      if (gameStarted && can_upload_fail && failed_kill_feedback > 0)
      {
        Serial.println("[BACKGROUND] Initiating Failed ESPNOW Feedback Upload...");
        uploadInProgress = 1; // prevent Treasure from recovering
        dbc.connectToWiFi();
        for (int i = failed_kill_feedback; i > 0; i--)
        {
          int this_ID = failed_kill_ID[current_failed_read_pointer];
          int this_CLAN = failed_kill_CLAN[current_failed_read_pointer];
          Serial.printf("[BACKGROUND] Upload Failed ESPNOW Feedback from CLAN %d ID %d\n", this_CLAN, this_ID);
          dbc.uploadFailedFeedback("TREASURE" + String(ID), this_CLAN, this_ID); // if still unsuccessful, it will be ignored. TODO: Fix
          current_failed_read_pointer++;
          if (current_failed_read_pointer > 50)
            current_failed_read_pointer -= 50;
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

void setup()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  StartUpDisplay();
  Serial.begin(115200);
  TreasureBase_NeoPixel.initialize();
  // TreasureBase_NeoPixel.off_FRONT();
  // TreasureBase_NeoPixel.off_TOP();

  TreasureBase_IR.enable();
  TreasureBase_EspNOW.enable();
  EEPROM.begin(EEPROM_SIZE);
  ID = EEPROM.read(ID_add);
  WIFI_ON = EEPROM.read(ONLINE_mode_add);

  HTTP_TIMEOUT = 30000;

  xTaskCreatePinnedToCore(
      backgroundTaskCode, /* Task function. */
      "backgroundTask",   /* name of task. */
      10000,              /* Stack size of task */
      NULL,               /* parameter of the task */
      2,                  /* priority of the task */
      &backgroundTask,    /* Task handle to keep track of created task */
      0);
}

void loop()
{

  if (AdminFunction)
  {
    TreasureBase_Admin.AdminLoop();
  }
  else if (ProfileFunction)
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
  }
  else
  {
    // online mode
    handleJoystick();
    if (setUpDone)
    {
      treasureBase.display_in_game();
      treasureBase.receiveAction();
      treasureBase.recover();
    }
    else
    {
      treasureBase.display_not_playing_yet();
    }
  }
}
