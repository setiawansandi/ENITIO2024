#define NAV_WAIT 4000

class MainMenu {
    private:
        int lastProcessNav = 0;
        unsigned long Nav_start = 0;
    
    public:
        void handle_joystick(){
            joystick_pos joystick_pos = MainMenu_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos)
                {
                case left:
                    lastProcessNav--;
                    if (lastProcessNav > 6) lastProcessNav -= 7;
                    if (lastProcessNav < 0) lastProcessNav += 7;
                    Nav_start = millis();
                    MainMenu_joystick.set_state();
                    break;

                case right:
                    lastProcessNav++;
                    if (lastProcessNav > 6) lastProcessNav -= 7;
                    if (lastProcessNav < 0) lastProcessNav += 7;
                    Nav_start = millis();
                    MainMenu_joystick.set_state();
                    break;
                
                case button:
                    if (lastProcessNav != currentProcess)
                        currentProcess = lastProcessNav;
                    break;

                default:
                    if (lastProcessNav != currentProcess){
                        unsigned long currTime = millis();
                        if (currTime - Nav_start > NAV_WAIT) lastProcessNav = currentProcess;
                    }
                    break;
                }
            }
            else {
                if (lastProcessNav != currentProcess){
                    unsigned long currTime = millis();
                    if (currTime - Nav_start > NAV_WAIT) lastProcessNav = currentProcess;
                }
            }
        };

        void update_display(){
            MainMenu_OLED.display_menu(lastProcessNav);
        }

        void MainMenuLoop(){
            handle_joystick();
            update_display()
        }
};