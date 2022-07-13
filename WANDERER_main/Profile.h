const int OG_RegisteringPage = 0;
const int isGL_RegisteringPage = 1;

class Profile{
    private:
        int OG_pointer = 0;
        int OG;
        int isGL_pointer = 0;
        int isGL;
        int currentRegisteringPage = 0;
    
    public:
        // void reset(){
        //   OG_pointer = 0;
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

                default:
                    break;
                }
            }
        }

        void handleJoystickRegisteringOG(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos)
                { 
                case button:
                    Player_joystick.set_state();
                    OG = OG_pointer;
                    currentRegisteringPage += 1;
                    break;

                case up:
                    OG_pointer = max(OG_pointer - 1, 0);
                    Player_joystick.set_state();
                    break;

                case down:
                    OG_pointer = min(OG_pointer + 1, 3);
                    Player_joystick.set_state();
                    break;

                default:
                    break;
                }
            }
        }

        void handleJoystickRegisteringisGL(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos)
                { 
                case up:
                    isGL_pointer = max(isGL_pointer - 1, 0);
                    Player_joystick.set_state();
                    break;

                case down:
                    isGL_pointer = min(isGL_pointer + 1, 1);
                    Player_joystick.set_state();
                    break;

                case button:
                    isGL = isGL_pointer;
                    EEPROM.write(isGL_add, isGL);
//                    EEPROM.write(NAME_add, "Alex");
                    EEPROM.write(OG_add, OG);
                    dbc.registerWanderer(OG, my_MAC_address); 
                    EEPROM.write(PROFILE_enable_add, 1);
                    EEPROM.commit();
                    Player_joystick.set_state();
                    break;

                default:
                    break;
                }
            }
        }

        void ProfileLoop(){
            if (EEPROM.read(PROFILE_enable_add) == 0){
                switch (currentRegisteringPage)
                {
                case OG_RegisteringPage:
                    handleJoystickRegisteringOG();
                    Profile_OLED.display_OGregisteringPage(OG_pointer);
                    break;

                case isGL_RegisteringPage:
                    handleJoystickRegisteringisGL();
                    Profile_OLED.display_isGLregisteringPage(isGL_pointer);
                    break;
                
                default:
                    break;
                }
            }
            else {
                handleJoystickRegistered();
                OG = EEPROM.read(OG_add);
                isGL = EEPROM.read(isGL_add);
                String name = "Alex";
                Profile_OLED.display_CompleteProfilePage(OG, isGL, name);
            }
        }
};

Profile My_Profile;
