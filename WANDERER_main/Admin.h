void clearEEPROM(){
  int i;
  for (i=0; i<EEPROM_SIZE; i++){
    EEPROM.write(i,0);
  }
  EEPROM.commit();
};

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
        bool isSettingGL = false;
        int isGLNav = 1 ;
        bool isOverwritingID = false;
        int currentIDnum = 0;

    public:
        void handleJoystickMain(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos){
                     case up:
                        FunctionNav = max(FunctionNav - 1, 0);
                        Player_joystick.set_state();
                        break;

                    case down:
                        FunctionNav = min(FunctionNav + 1, 4);
                        Player_joystick.set_state();
                        break;

                    case button:
                        switch (FunctionNav)
                        {
                        case FactoryResetFunction:
                            isConfirmingReset = true;
                            break;

                        case SetGLFunction:
                            isSettingGL = true;
                            break;

                        case ToggleServerConnectivityFunction:
                            handleServerConnectivityToggle();
                            break;

                        case OverwritePlayerIDFunction:
                            isOverwritingID = true;
                            break;

                        case ExitFunction:
                            currentProcess = MainMenuProcess;
                            verified = false ; 
                            FunctionNav = 0;
                            break;
                        
                        default:
                            break;
                        }
                        Player_joystick.set_state();
                        break;

                    case idle:
                        break;

                    default:
                        Player_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickPwd(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos){
                    case up:
                        currentDigit ++;
                        if (currentDigit > 9) currentDigit -= 10;
                        if (currentDigit < 0) currentDigit += 10;
                        Player_joystick.set_state();
                        break;

                    case down:
                        currentDigit --;
                        if (currentDigit > 9) currentDigit -= 10;
                        if (currentDigit < 0) currentDigit += 10;
                        Player_joystick.set_state();
                        break;

                    case left:
                        enteringPwdNav = max(enteringPwdNav - 1 , 0) ;
                        Player_joystick.set_state();
                        break;

                    case right:
                        enteringPwdNav = min(enteringPwdNav + 1 , 2) ;
                        Player_joystick.set_state();
                        break;

                    case button:
                        switch (enteringPwdNav)
                        {
                        case 0:
                            currentProcess = MainMenuProcess;
                            enteringPwdNav = 1;
                            currentDigit = 0;
                            currentDigitIndex = 0;
                            break;
                        
                        case 1:
                            if (currentDigitIndex < 5){
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
                                for (i = 0; i < 6; i++){
                                    pwd_check *= (int) (user_key_in_pwd[i] == admin_pwd[i]);
                                }
                                if (pwd_check){
                                    verified = true;
                                }
                                else{
                                    Admin_OLED.WrongPwdDisplay();
                                    Player_Buzzer.sound(NOTE_E4);
                                    delay(1500);
                                    Player_Buzzer.end_sound();
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
                        Player_joystick.set_state();
                        break;

                    case idle:
                        break;

                    default:
                        Player_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickConfirmReset() {
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos){
                    case up:
                        ConfirmingResetNav = max(ConfirmingResetNav - 1, 0);
                        Player_joystick.set_state();
                        break;

                    case down:
                        ConfirmingResetNav = min(ConfirmingResetNav + 1, 1);
                        Player_joystick.set_state();
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
                            // write into EEPROM Online mode again
                            EEPROM.write(ONLINE_mode_add, 1);
                            EEPROM.commit();
                            ESP.restart();
                            break;
                        
                        default:
                            break;
                        }
                        Player_joystick.set_state();
                        break;

                    case idle:
                        break;
                    
                    default:
                        Player_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickSettingGL(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos){
                    case up:
                        isGLNav = max(isGLNav - 1, 0);
                        Player_joystick.set_state();
                        break;

                    case down:
                        isGLNav = min(isGLNav + 1, 1);
                        Player_joystick.set_state();
                        break;

                    case button:
                        EEPROM.write(isGL_add, isGLNav);
                        Admin_OLED.ConfirmSettingGLDisplay(isGLNav);
                        if (isGLNav) {
                            // set to GL HP and Energy
                            EEPROM.write(PLAYER_HP_add, GL_MaxHP);
                            EEPROM.write(PLAYER_EN_add, GL_MaxEn);
                            EEPROM.write(PLAYER_MaxHP_add, GL_MaxHP);
                            EEPROM.write(PLAYER_MaxEn_add, GL_MaxEn);
                        } else {
                            EEPROM.write(PLAYER_HP_add, PARTICIPANT_MaxHP);
                            EEPROM.write(PLAYER_EN_add, PARTICIPANT_MaxEn);
                            EEPROM.write(PLAYER_MaxHP_add, PARTICIPANT_MaxHP);
                            EEPROM.write(PLAYER_MaxEn_add, PARTICIPANT_MaxEn);
                        }
                        EEPROM.commit();
                        isGLNav = 1;
                        isSettingGL = false;
                        delay(1500);
                        Player_joystick.set_state();
                        break;

                    case idle:
                        break;
                    
                    default:
                        Player_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickOverwriteID(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                int saved_clan;
                switch (joystick_pos){
                    case up:
                        currentIDnum = max(currentIDnum - 1, 0);
                        Player_joystick.set_state();
                        break;

                    case down:
                        currentIDnum = min(currentIDnum + 1, 255);
                        Player_joystick.set_state();
                        break;

                    case button:
                        EEPROM.write(ID_add, currentIDnum);
                        saved_clan = EEPROM.read(CLAN_add);
                        Admin_OLED.ConfirmPlayerIDDisplay(saved_clan, currentIDnum);
                        EEPROM.commit();
                        isOverwritingID = false;
                        delay(1500);
                        Player_joystick.set_state();
                        break;

                    case idle:
                        break;
                    
                    default:
                        Player_joystick.set_state();
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
          Player_joystick.set_state();
        }

        void AdminLoop(){
            switch (verified)
            {
            case 0:
                handleJoystickPwd();
                Admin_OLED.display_EnteringPwd(currentDigitIndex, currentDigit, enteringPwdNav, user_key_in_pwd);
                break;

            case 1:
                if (isConfirmingReset){
                    handleJoystickConfirmReset();
                    Admin_OLED.display_ConfirmingReset(ConfirmingResetNav);
                }
                else if(isSettingGL){
                    handleJoystickSettingGL();
                    Admin_OLED.display_SettingGL(isGLNav);
                }
                else if (isOverwritingID) {
                    handleJoystickOverwriteID();
                    int previousID = EEPROM.read(ID_add);
                    Admin_OLED.display_SettingID(previousID, currentIDnum);
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

Admin My_Admin;
