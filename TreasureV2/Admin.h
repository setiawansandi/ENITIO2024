const int SetIDFunction = 0;
const int FactoryResetFunction = 1;
const int CheckCollectionFunction = 2;
const int ToggleServerConnectivityFunction = 3;
const int ExitFunction = 4;

class Admin_OLED {
  private:

  public:
    void WrongPwdDisplay() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 28);

      display.println("    Wrong Password   ");
      display.display();
    }

    void ConfirmSettingIDDisplay(int currentIDnum) {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 28);

      display.print("Treasure ID set as ");
      display.println(currentIDnum);

      display.display();
    }

    void display_EnteringPwd(int currentDigitIndex, int currentDigit, int enteringPwdNav, int *user_key_in_pwd) {
      display.clearDisplay();

      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("Please Enter Password"));

      int i;
      for (i = 0; i < 6; i++) {
        display.setCursor(5 + 15 * (i + 1), 28);
        if (currentDigitIndex < i) {
          display.setTextColor(SSD1306_WHITE);
          display.print("*");
        }
        else if (currentDigitIndex == i) {
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(currentDigit);
        }
        else {
          display.setTextColor(SSD1306_WHITE);
          display.print(user_key_in_pwd[i]);
        }
      }

      display.setCursor(0, 56);
      switch (enteringPwdNav)
      {
        case 0:
          display.setTextColor(SSD1306_WHITE);
          display.print(" ");
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print("Back to Main Menu");
          display.setTextColor(SSD1306_WHITE);
          display.print(" >");
          break;

        case 1:
          display.setTextColor(SSD1306_WHITE);
          display.print(" <      Enter      > ");
          break;

        case 2:
          display.setTextColor(SSD1306_WHITE);
          display.print(" <          ");
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print("Backspace");
          break;

        default:
          break;
      }

      display.display();
    }

    void display_MainAdmin(int FunctionNav) {
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("        Admin        "));

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 10);
      display.println("Register ID");
      display.setCursor(10, 19);
      display.println("Factory Reset");
      display.setCursor(10, 28);
      display.println("Check Collections");

      display.setCursor(10, 37);
      if (EEPROM.read(ONLINE_mode_add) == 0) {
        display.println("Set Online Mode");
      } else display.println("Set Offline Mode");

      display.setCursor(10, 46);
      display.println("Back to Main Menu");

      switch (FunctionNav)
      {
        case SetIDFunction:
          display.setCursor(2, 10);
          display.println(">");
          break;

        case FactoryResetFunction:
          display.setCursor(2, 19);
          display.println(">");
          break;

        case CheckCollectionFunction:
          display.setCursor(2, 28);
          display.println(">");
          break;

        case ToggleServerConnectivityFunction:
          display.setCursor(2, 37);
          display.println(">");
          break;

        case ExitFunction:
          display.setCursor(2, 46);
          display.println(">");
          break;

        default:
          break;
      }

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)");

      display.display();
    }

    void display_ConfirmingReset(int ConfirmingResetNav) {
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("  Confirm to Reset?  "));
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 12);
      display.println("NO");
      display.setCursor(10, 22);
      display.println("YES, KEEP TRES. ID");
      display.setCursor(10, 32);
      display.println("YES, CLEAR TRES. ID");

      switch (ConfirmingResetNav)
      {
        case 0:
          display.setCursor(2, 12);
          display.println(">");
          break;

        case 1:
          display.setCursor(2, 22);
          display.println(">");
          break;

        case 2:
          display.setCursor(2, 32);
          display.println(">");
          break;

        default:
          break;
      }

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)");

      display.display();
    }

    void display_SettingID(int previousID, int currentIDnum) {
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F(" Setting Treasure ID "));
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 12);
      display.print("Old ID: ");
      display.println(previousID);

      display.setCursor(0, 40);
      display.print("New ID: ");
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println(currentIDnum);

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 50);
      display.display();
    }

    void display_CheckingCollection(int invicta_collection, int dynari_collection, int ephilia_collection,
                                    int akrona_collection, int solaris_collection) {
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F(" Collections by CLAN "));

      display.setTextColor(SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 12);
      display.print("Invicta: "), display.println(invicta_collection);
      display.setCursor(0, 21);
      display.print("Dynari: "), display.println(dynari_collection);
      display.setCursor(0, 30);
      display.print("Ephilia: "), display.println(ephilia_collection);
      display.setCursor(0, 39);
      display.print("Akrona: "), display.println(akrona_collection);
      display.setCursor(0, 48);
      display.print("Solaris: "), display.println(solaris_collection);

      display.setCursor(10, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to go back)");
      display.display();
    }

    void ConfirmConnectivityToggle(int connectivityState) {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 22);

      display.println(" Set WANDERER Mode: ");

      display.setCursor(0, 34);
      if (connectivityState) display.println("       Online       ");
      else display.println("       Offline      ");
      display.display();
    }
};

Admin_OLED Admin_OLED;

class Admin {
  private:
    int admin_pwd[6] = {1, 0, 0, 0, 0, 9};
    int user_key_in_pwd[6] = {};
    int currentDigitIndex = 0;
    int currentDigit = 0;
    int enteringPwdNav = 1 ;
    bool verified = false ;
    int FunctionNav = 0;
    bool isConfirmingReset = false;
    int ConfirmingResetNav = 1;
    bool isSettingID = false;
    int currentIDnum = 0 ;
    bool isCheckingCollection = false;

  public:
    void handleJoystickMain() {
      joystick_pos joystick_pos = TreasureV2_joystick.read_Joystick();
      if (TreasureV2_joystick.get_state() == 0) {
        switch (joystick_pos) {
          case up:
            FunctionNav = max(FunctionNav - 1, 0);
            TreasureV2_joystick.set_state();
            break;

          case down:
            FunctionNav = min(FunctionNav + 1, 4);
            TreasureV2_joystick.set_state();
            break;

          case button:
            switch (FunctionNav)
            {
              case FactoryResetFunction:
                isConfirmingReset = true;
                break;

              case SetIDFunction:
                isSettingID = true;
                break;

              case CheckCollectionFunction:
                isCheckingCollection = true;
                break;

              case ToggleServerConnectivityFunction:
                handleServerConnectivityToggle();
                break;

              case ExitFunction:
                AdminFunction = false;
                verified = false ;
                break;

              default:
                break;
            }
            FunctionNav = 0;
            TreasureV2_joystick.set_state();
            break;

          case idle:
            break;

          default:
            TreasureV2_joystick.set_state();
            break;
        }
      }
    }

    void handleJoystickPwd() {
      joystick_pos joystick_pos = TreasureV2_joystick.read_Joystick();
      if (TreasureV2_joystick.get_state() == 0) {
        switch (joystick_pos) {
          case up:
            currentDigit ++;
            if (currentDigit > 9) currentDigit -= 10;
            if (currentDigit < 0) currentDigit += 10;
            TreasureV2_joystick.set_state();
            break;

          case down:
            currentDigit --;
            if (currentDigit > 9) currentDigit -= 10;
            if (currentDigit < 0) currentDigit += 10;
            TreasureV2_joystick.set_state();
            break;

          case left:
            enteringPwdNav = max(enteringPwdNav - 1 , 0) ;
            TreasureV2_joystick.set_state();
            break;

          case right:
            enteringPwdNav = min(enteringPwdNav + 1 , 2) ;
            TreasureV2_joystick.set_state();
            break;

          case button:
            switch (enteringPwdNav)
            {
              case 0:
                AdminFunction = false;
                enteringPwdNav = 1;
                currentDigit = 0;
                currentDigitIndex = 0;
                break;

              case 1:
                if (currentDigitIndex < 5) {
                  user_key_in_pwd[currentDigitIndex] = currentDigit ;
                  currentDigitIndex ++ ;
                  currentDigit = 0;
                }
                else {
                  user_key_in_pwd[currentDigitIndex] = currentDigit ;
                  currentDigit = 0;
                  currentDigitIndex = 0;
                  int pwd_check = 1 ;
                  int i ;
                  for (i = 0; i < 6; i++) {
                    pwd_check *= (int) (user_key_in_pwd[i] == admin_pwd[i]);
                  }
                  if (pwd_check) {
                    verified = true;
                  }
                  else {
                    Admin_OLED.WrongPwdDisplay();
                    delay(1500);
                  }
                }
                break;

              case 2:
                currentDigitIndex = max(currentDigitIndex - 1, 0);
                currentDigit = 0;
                enteringPwdNav = 1;
                break;

              default:
                break;
            }
            TreasureV2_joystick.set_state();
            break;

          case idle:
            break;

          default:
            TreasureV2_joystick.set_state();
            break;
        }
      }
    }

    void handleJoystickConfirmReset() {
      joystick_pos joystick_pos = TreasureV2_joystick.read_Joystick();
      if (TreasureV2_joystick.get_state() == 0) {
        switch (joystick_pos) {
          case up:
            ConfirmingResetNav = max(ConfirmingResetNav - 1, 0);
            TreasureV2_joystick.set_state();
            break;

          case down:
            ConfirmingResetNav = min(ConfirmingResetNav + 1, 2);
            TreasureV2_joystick.set_state();
            break;

          case button:
            switch (ConfirmingResetNav)
            {
              case 0:
                isConfirmingReset = false;
                ConfirmingResetNav = 1;
                break;

              case 1:
                StartUpDisplay();
                clearEEPROM();
                EEPROM.write(ID_add, ID);
                EEPROM.write(ONLINE_mode_add, 0);
                EEPROM.write(ENABLE_add, 0);
                EEPROM.commit();
                ESP.restart();
                break;

              case 2:
                StartUpDisplay();
                clearEEPROM();
                EEPROM.write(ONLINE_mode_add, 0);
                EEPROM.write(ENABLE_add, 0);
                EEPROM.commit();
                ESP.restart();
                break;

              default:
                break;
            }
            TreasureV2_joystick.set_state();
            break;

          case idle:
            break;

          default:
            TreasureV2_joystick.set_state();
            break;
        }
      }
    }

    void handleJoystickCheckingCollection() {
      joystick_pos joystick_pos = TreasureV2_joystick.read_Joystick();
      if (TreasureV2_joystick.get_state() == 0) {
        switch (joystick_pos) {
          case button:
            isCheckingCollection = false;
            TreasureV2_joystick.set_state();
            break;

          case idle:
            break;

          default:
            TreasureV2_joystick.set_state();
            break;
        }
      }
    }

    void handleJoystickSettingID() {
      joystick_pos joystick_pos = TreasureV2_joystick.read_Joystick();
      if (TreasureV2_joystick.get_state() == 0) {
        switch (joystick_pos) {
          case down:
            currentIDnum = max(currentIDnum - 1, 0);
            TreasureV2_joystick.set_state();
            break;

          case up:
            currentIDnum = min(currentIDnum + 1, ID_BOUND);
            TreasureV2_joystick.set_state();
            break;

          case button:
            ID = currentIDnum;
            EEPROM.write(ID_add, ID);
            EEPROM.commit();
            Admin_OLED.ConfirmSettingIDDisplay(currentIDnum);
            currentIDnum = 0;
            isSettingID = false;
            delay(1500);
            TreasureV2_joystick.set_state();
            break;

          case idle:
            break;

          default:
            TreasureV2_joystick.set_state();
            break;
        }
      }
    }

    void handleServerConnectivityToggle() {
      int newConnectivityMode;
      if (EEPROM.read(ONLINE_mode_add) == 0) {
        Serial.println("Switching from offline -> online mode");
        newConnectivityMode = 1;
      } else {
        Serial.println("Switching from online -> offline mode");
        newConnectivityMode = 0;
      }
      EEPROM.write(ONLINE_mode_add, newConnectivityMode);
      EEPROM.commit();
      WIFI_ON = newConnectivityMode;
      Admin_OLED.ConfirmConnectivityToggle(newConnectivityMode);
      delay(1500);
      TreasureV2_joystick.set_state();
    }

    void AdminLoop() {
      switch (verified)
      {
        case 0:
          handleJoystickPwd();
          Admin_OLED.display_EnteringPwd(currentDigitIndex, currentDigit, enteringPwdNav, user_key_in_pwd);
          break;

        case 1:
          if (isConfirmingReset) {
            handleJoystickConfirmReset();
            Admin_OLED.display_ConfirmingReset(ConfirmingResetNav);
          }
          else if (isSettingID) {
            handleJoystickSettingID();
            int previousID = EEPROM.read(ID_add);
            Admin_OLED.display_SettingID(previousID, currentIDnum);
          }
          else if (isCheckingCollection) {
            handleJoystickCheckingCollection();
            int invicta_collection = EEPROM.read(INVICTA_add);
            int dynari_collection = EEPROM.read(DYNARI_add);
            int ephilia_collection = EEPROM.read(EPHILIA_add);
            int akrona_collection = EEPROM.read(AKRONA_add);
            int solaris_collection = EEPROM.read(SOLARIS_add);
            Admin_OLED.display_CheckingCollection(invicta_collection, dynari_collection, ephilia_collection,
                                                  akrona_collection, solaris_collection);
          }
          else {
            handleJoystickMain();
            Admin_OLED.display_MainAdmin(FunctionNav);
          }
          break;

        default:
          break;
      }
    }
};

Admin TreasureV2_Admin;
