class Profile{
    private:
        int CLAN_pointer = 0;
        int CLAN;
        bool registerStatus = false;
        // int isGL_pointer = 0;
        // int isGL;
        // int currentRegisteringPage = 0;
    
    public:
        // void reset(){
        //   CLAN_pointer = 0;
        //   isGL_pointer = 0;
        //   currentRegisteringPage = 0;
        // }
    
        void handleJoystickRegistered(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos)
                { 
                case button:
                    currentProcess = MainMenuProcess;
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

        void handleJoystickRegisteringCLAN(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos)
                { 
                case button:
                    Profile_OLED.interim_registering_display();
                    Player_joystick.set_state();
                    CLAN = CLAN_pointer;
                    registerStatus = dbc.registerWanderer(CLAN, my_MAC_address); 
                    while (!registerStatus){
                        delay(10000);
                        registerStatus = dbc.registerWanderer(CLAN, my_MAC_address); 
                    }
                    EEPROM.write(CLAN_add, CLAN);
                    EEPROM.write(PROFILE_enable_add, 1);
                    if (EEPROM.read(isGL_add) != 1) EEPROM.write(isGL_add, 0);
                    EEPROM.commit();
                    Player_joystick.set_state();
                    break;

                case up:
                    CLAN_pointer = max(CLAN_pointer - 1, 0);
                    Player_joystick.set_state();
                    break;

                case down:
                    CLAN_pointer = min(CLAN_pointer + 1, 4);
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

        // void handleJoystickRegisteringisGL(){
        //     joystick_pos joystick_pos = Player_joystick.read_Joystick();
        //     if (Player_joystick.get_state() == 0) {
        //         switch (joystick_pos)
        //         { 
        //         case up:
        //             isGL_pointer = max(isGL_pointer - 1, 0);
        //             Player_joystick.set_state();
        //             break;

        //         case down:
        //             isGL_pointer = min(isGL_pointer + 1, 1);
        //             Player_joystick.set_state();
        //             break;

        //         case button:
        //             isGL = isGL_pointer;
        //             EEPROM.write(isGL_add, isGL);
        //             EEPROM.write(CLAN_add, CLAN);
        //             dbc.registerWanderer(CLAN, my_MAC_address); 
        //             EEPROM.write(PROFILE_enable_add, 1);
        //             EEPROM.commit();
        //             Player_joystick.set_state();
        //             break;

        //         default:
        //             break;
        //         }
        //     }
        // }

        void ProfileLoop(){
            if (EEPROM.read(PROFILE_enable_add) == 0){
                handleJoystickRegisteringCLAN();
                Profile_OLED.display_CLANregisteringPage(CLAN_pointer);
            }
            else {
                handleJoystickRegistered();
                int CLAN_ = EEPROM.read(CLAN_add);
                int isGL_ = EEPROM.read(isGL_add);
                Profile_OLED.display_CompleteProfilePage(CLAN_, isGL_);
            }
        }
};

Profile My_Profile;

