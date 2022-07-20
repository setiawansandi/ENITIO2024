#define ACTION_RECV_WAIT 150 // [ms] 
#define TEMP_NOTI_WAIT 6000 // including being Attacked, Healed, Successfully Attack and Successfully Collect
#define NOTI_SOUND_DURATION 200
#define TEMP_NOTI_BLINKING 300
#define FEEDBACK_WAIT 5000

#define LUCKY_NOT_INFECTED_DURATION 5000 // [ms]

#define x2_En_Regen_bonus_duration 300000 // [ms]

int game_started_buffer = 0;

uint8_t newMACAddress[] = {4, 8, 22, 1, 255, 255};

class TreasureHuntPlayer
{
  private:
    int ID; 
    int OG;
    bool _isGL;  
    int HP;
    int MaxHP;
    int En; // energy
    int MaxEn;
    int MANA;

    action_id action;

    unsigned long lastActionReceived = 0;

    int currentPage = 0;
    int lastPageNav = 0;
    
    String tempNoti = "";
    String permNoti = "";

    unsigned long Nav_start = 0;
    unsigned long tempNoti_start = 0;
    unsigned long last_max_en_decay = 0;
    unsigned long last_en_recover = 0;
    unsigned long last_hp_decay = 0;
    unsigned long last_received_heal = 0;
    unsigned long last_lucky_not_infected = 0;
    unsigned long start_x2_en_regen = 0;

    unsigned long start_receiving_feedback = 0;

    int numKilled = 0;
    int numL1Treasure = 0;
    int numL2Treasure = 0;

    bool infectedWithVirus = 0;

    int num_bonus6HP = 0;
    int num_bonus1MaxEn = 0;
    int num_bonus1MANA = 0;
    int num_fiveminx2EnRegen = 0;
    int num_bomb = 0;

    bool is_x2EnRegen = false;

    int PowerUpNav = 0;
    bool choosingPowerUp = false;

  public:
    bool gameStarted = 0;

    // void reset(){
    //   HP = 12;
    //   MaxHP = 12;
    //   En = 5; // energy
    //   MaxEn = 5;
    //   MANA = 3;

    //   tempNoti = "";
    //   permNoti = "";
  
    //   Nav_start = 0;
    //   tempNoti_start = 0;
    //   last_max_en_decay = 0;
    //   last_en_recover = 0;
    //   last_hp_decay = 0;
  
    //   start_receiving_feedback = 0;
  
    //   numKilled = 0;
    //   numL1Treasure = 0;
    //   numL2Treasure = 0;
  
    //   LastL1TreasureCollected = -1;
  
    //   infectedWithVirus = 0;
  
    //   gameStarted = 0 ;
    //   game_started_buffer = 0;
    // }

    
    void setup_initial_state(int id, int og, bool isGL) {
      ID = id;
      OG = og ;
      _isGL = isGL;

      newMACAddress[4] = OG;
      newMACAddress[5] = ID;
      esp_wifi_set_mac(WIFI_IF_AP, &newMACAddress[0]);

      Serial.print("[NEW] ESP8266 Board MAC Address:  ");
      Serial.println(WiFi.macAddress());

      if (EEPROM.read(PLAYER_enable_add) == 0){
        if (!_isGL){
          HP = PARTICIPANT_MaxHP;
          En = PARTICIPANT_MaxEn;
          MaxHP = PARTICIPANT_MaxHP;
          MaxEn = PARTICIPANT_MaxEn;
          MANA = INITIAL_MANA;
        }
        else {
          HP = GL_MaxHP;
          En = GL_MaxEn;
          MaxHP = GL_MaxHP;
          MaxEn = GL_MaxEn;
          MANA = INITIAL_MANA;
        }

        EEPROM.write(PLAYER_enable_add, 1);
        EEPROM.write(PLAYER_HP_add, HP);
        EEPROM.write(PLAYER_EN_add, En);
        EEPROM.write(PLAYER_MaxHP_add, MaxHP);
        EEPROM.write(PLAYER_MaxEn_add, MaxEn);
        EEPROM.write(PLAYER_MANA_add, MANA);
        EEPROM.commit();
      }
      else {
        HP = EEPROM.read(PLAYER_HP_add);
        En = EEPROM.read(PLAYER_EN_add);
        MaxHP = EEPROM.read(PLAYER_MaxHP_add);
        MaxEn = EEPROM.read(PLAYER_MaxEn_add);
        MANA = EEPROM.read(PLAYER_MANA_add);
        numKilled = EEPROM.read(PLAYER_numKilled_add);
        numL1Treasure = EEPROM.read(PLAYER_numL1Treasure_add);
        numL2Treasure = EEPROM.read(PLAYER_numL2Treasure_add);
        num_bonus6HP = EEPROM.read(PLAYER_num_bonus6HP_add);
        num_bonus1MaxEn = EEPROM.read(PLAYER_num_bonus1MaxEn_add);
        num_bonus1MANA = EEPROM.read(PLAYER_num_bonus1MANA_add);
        num_fiveminx2EnRegen = EEPROM.read(PLAYER_num_fiveminx2EnRegen_add);
        num_bomb = EEPROM.read(PLAYER_num_bomb_add);
      }
    }

    void sendAction() {
      // format of the IR signal (16-bit hexadecimal, i.e. 4 digits)
      // address: 0x0<OG><ID - 2 bit>  (ID is 2 bits as there maybe more than 16 people in one OG)
      // command: 0x00<En><Action>
      if ((action != do_nothing) && (En > 0)) {
        uint16_hex_digits address_digits, command_digits;

        address_digits.digit0 = ID;
        address_digits.digit2 = OG;

        command_digits.digit0 = action;

        int this_action_multiplier;

        switch (action)
        {
        case attack:
          this_action_multiplier = min(MANA, MAX_ATTACK_MANA);
          break;

        case collect:
          this_action_multiplier = min(MANA, MAX_COLLECT_MANA);
          break;

        case heal:
          this_action_multiplier = HEAL_MANA;
          break;
        
        default:
          this_action_multiplier = 0;
          break;
        }

        command_digits.digit1 = this_action_multiplier;

        ir_signal send_signal;
        send_signal.address = address_digits;
        send_signal.command = command_digits;

        Player_IR.send(send_signal, 1);

        Player_EspNOW.is_waiting_for_feedback = 1;
        start_receiving_feedback = millis();

        En--;
        EEPROM.write(PLAYER_EN_add, En);
      }
    };

    void receiveAction() {
      int OG_, ID_, MANA_, action_, _isL1Treasure; // underscore denotes details of IR signal sender
      unsigned long currTime = millis();
      if (Player_IR.available()) {
         ir_signal IRsignal_ = Player_IR.read();

         if (currTime - lastActionReceived > ACTION_RECV_WAIT) {
           OG_ = IRsignal_.address.digit2;
           ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);
    
           MANA_ = IRsignal_.command.digit1;
           action_ = IRsignal_.command.digit0;
    
           Serial.printf("%d %d %d %d %d %d %d %d \n", IRsignal_.address.digit3, IRsignal_.address.digit2, IRsignal_.address.digit1, IRsignal_.address.digit0, IRsignal_.command.digit3, IRsignal_.command.digit2, IRsignal_.command.digit1, IRsignal_.command.digit0);
  
           lastActionReceived = currTime;

           if (((OG_ != OG) && (action_ == attack)) || ((action_ == heal) && (OG_ == OG) && (ID_ != ID)) || ((action_ == heal) && (OG_ != OG)))
                handleAction(OG_, ID_, action_, MANA_);
           }
        }
      }

    void update_player_state(){
      if (HP == 0){
        En = 0;
        EEPROM.write(PLAYER_EN_add, En);
        permNoti = "    You Are Killed!     ";
        unsigned long currTime = millis();
        // if (currTime - last_max_en_decay >= MAX_EN_DECAY_DURATION){
        //   MaxEn = max(MaxEn - 1, 1);
        //   EEPROM.write(PLAYER_MaxEn_add, MaxEn);
        //   last_max_en_decay = currTime;
        // }
        if (MANA > 1) {
          MANA = 1;
          EEPROM.write(PLAYER_MANA_add, MANA);
        }
      }
      else {
        permNoti = "";
        unsigned long currTime = millis();
        last_max_en_decay = currTime;
        if (En >= MaxEn){
          last_en_recover = currTime;
        }
        else {
          int en_rcv = EN_RECOVER_DURATION;
          if (is_x2EnRegen) {
            if (currTime - start_x2_en_regen < x2_En_Regen_bonus_duration){
              en_rcv /= 2;
            }
            else {
              is_x2EnRegen = false;
            }
          }
          if (currTime - last_en_recover >= en_rcv){
            En++ ;
            EEPROM.write(PLAYER_EN_add, En);
            last_en_recover = currTime;
          }
        }

        // Check if player is infected with virus
        if (infectedWithVirus) {
            if (currTime - last_hp_decay >= VIRUS_DECAY_DURATION) {
                HP = max(HP - 1, 0);
                EEPROM.write(PLAYER_HP_add, HP);
                last_hp_decay = currTime;
            };
            permNoti = "    You Are Infected!   ";
        } else if (!_isGL){
            // currently not infected with virus AND is not healer
            // check if nearby devices are transmitting virus
            if (Player_Bluetooth.isThereVirus && (currTime - last_received_heal >= VIRUS_IMMUNITY_DURATION) && (currTime - last_lucky_not_infected >= LUCKY_NOT_INFECTED_DURATION)) {
                randomSeed(currTime);
                int virus_infection_num = random(100);
                if (virus_infection_num < VIRUS_INFECTION_PROBABILITY){
                  infectedWithVirus = true;
                  HP--;
                  EEPROM.write(PLAYER_HP_add, HP);
                  last_hp_decay = currTime;
                  Player_Bluetooth.startSpreadingVirus();
                  permNoti = "    You Are Infected!   ";
                }
                else {
                  last_lucky_not_infected = currTime;
                }
            }
        }
      }
      EEPROM.commit();
    }
    
    void handleJoystick_waiting(){
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

    void handleJoystickInGame(){
      joystick_pos joystick_pos = Player_joystick.read_Joystick();
      if (Player_joystick.get_state() == 0) {
        switch (joystick_pos)
        {
        case up:
          action = attack;
          Player_joystick.set_state();
          break;
        
        case down:
          if (!_isGL) 
            action = collect;
          else
            action = heal ;
          Player_joystick.set_state();
          break;

        case left:
          if (!choosingPowerUp){
            lastPageNav--;
            if (lastPageNav > 4) lastPageNav -= 5;
            if (lastPageNav < 0) lastPageNav += 5;
            Nav_start = millis();
          }
          else {
            PowerUpNav-- ;
            if (PowerUpNav > 5) PowerUpNav -= 6;
            if (PowerUpNav < 0) PowerUpNav += 6;
          }
          Player_joystick.set_state();
          break;
          
         case right:
          if (!choosingPowerUp){
            lastPageNav++;
            if (lastPageNav > 4) lastPageNav -= 5;
            if (lastPageNav < 0) lastPageNav += 5;
            Nav_start = millis();
          }
          else {
            PowerUpNav++ ;
            if (PowerUpNav > 5) PowerUpNav -= 6;
            if (PowerUpNav < 0) PowerUpNav += 6;
          }
          Player_joystick.set_state();
          break;
          
        case button:
          if (!choosingPowerUp){
            if (lastPageNav != currentPage)
              currentPage = lastPageNav;
            if (currentPage == exitPage) {
              currentProcess = MainMenuProcess;
              currentPage = mainPage; // reset current page
              lastPageNav = currentPage;
            } 
            if (currentPage == powerupPage){
              choosingPowerUp = true;
              lastPageNav = mainPage;
            }
          }
          else {
            if (PowerUpNav == 0){
              currentPage = mainPage;
              choosingPowerUp = false;
            }
            else{
              handlePowerUp(PowerUpNav);
            }
          }

          Player_joystick.set_state();
          break;

        default:
          action = do_nothing;
          if (lastPageNav != currentPage){
            unsigned long currTime = millis();
            if (currTime - Nav_start > NAV_WAIT) lastPageNav = currentPage;
          }
          break;
        }
      }
      else {
        action = do_nothing;
        if (lastPageNav != currentPage){
            unsigned long currTime = millis();
            if (currTime - Nav_start > NAV_WAIT) lastPageNav = currentPage;
          }
      }
    }

    void handlePowerUp(int PowerUpNav){
      switch (PowerUpNav)
      {
      case bonus6HP:
        if (num_bonus6HP > 0){
          num_bonus6HP -- ;
          HP = min(HP + 6, MaxHP);
          EEPROM.write(PLAYER_num_bonus6HP_add, num_bonus6HP);
          EEPROM.write(PLAYER_HP_add, HP);
        }
        break;

      case bonus1MaxEn:
        if (num_bonus1MaxEn > 0){
          num_bonus1MaxEn -- ;
          MaxEn++;
          EEPROM.write(PLAYER_num_bonus1MaxEn_add, num_bonus1MaxEn);
          EEPROM.write(PLAYER_MaxEn_add, MaxEn);
        }
        break;

      case bonus1MANA:
        if (num_bonus1MANA > 0){
          num_bonus1MANA -- ;
          MANA++;
          EEPROM.write(PLAYER_num_bonus1MANA_add, num_bonus1MANA);
          EEPROM.write(PLAYER_MANA_add, MANA);
        }
        break; 

      case fiveminx2EnRegen:
        if (num_fiveminx2EnRegen > 0) {
          num_fiveminx2EnRegen -- ;
          start_x2_en_regen = millis();
          is_x2EnRegen = true;
          EEPROM.write(PLAYER_num_fiveminx2EnRegen_add, num_fiveminx2EnRegen);
        }
        break;

      case bomb:
        if (num_bomb > 0){
          num_bomb -- ;
          EEPROM.write(PLAYER_num_bomb_add, num_bomb);
        }
      
      default:
        break;
      }
    }

    void handleAction(int OG_, int ID_, int action_, int MANA_){
      Serial.println(action_);
      switch (action_)
      {
      case attack:
        if(HP > 0) {
          HP = max(HP - MANA_, 0);
          EEPROM.write(PLAYER_HP_add, HP);
          Serial.printf("Attacked. Current HP: %d \n", HP);
          tempNoti = "       Attacked      ";
          tempNoti_start = millis();
          feedback_attack(OG_, ID_);
          Player_Buzzer.sound(NOTE_E3);
        }
        break;
        
      case heal:
        HP = min(HP + MANA_, MaxHP);
        EEPROM.write(PLAYER_HP_add, HP);
        tempNoti = "        Healed       ";
        tempNoti_start = millis();
        if (infectedWithVirus) {
            Player_Bluetooth.stopSpreadingVirus();
        }
        infectedWithVirus = 0;
        last_received_heal = tempNoti_start;
        break;
      
      default:
        break;
      }
    }

    void feedback_attack(int OG_, int ID_){
      bool killed = (HP == 0);
      Player_EspNOW.send_data(1, OG_, ID_, OG, killed);
    } ;
      
    void receiveFeedback(){
      if (Player_EspNOW.is_waiting_for_feedback == 1) {
        unsigned long currTime = millis();
        if (currTime - start_receiving_feedback <= FEEDBACK_WAIT){
          if (EspNOW_received == 1){
            feedback_message feedbackData = Player_EspNOW.get_feedback_received();
            switch (feedbackData.attackee_type)
            {
            case 1:
              if ((feedbackData.attacker_OG == OG) && (feedbackData.attacker_ID == ID)){
                if (feedbackData.is_attackee_killed == true) {
                  tempNoti = " You killed a person ";
                  tempNoti_start = millis();
                  MANA++;
                  numKilled ++ ;
                  EEPROM.write(PLAYER_MANA_add, MANA);
                  EEPROM.write(PLAYER_numKilled_add, numKilled);
                }
                else {
                  tempNoti = " Attack successfully ";
                  tempNoti_start = millis();
                }
                EspNOW_received = 0;
                Player_EspNOW.is_waiting_for_feedback = 0;
              }
              break;

            case 3:
              if ((feedbackData.attacker_OG == OG) && (feedbackData.attacker_ID == ID)){
                if (feedbackData.is_attackee_killed == true) {
                  tempNoti = " U Opened L2 Treasure";
                  tempNoti_start = millis();
                  numL2Treasure ++ ;
                  EEPROM.write(PLAYER_numL2Treasure_add, numL2Treasure);
                }
                else {
                  tempNoti = " L2 Treasure Damaged ";
                  tempNoti_start = millis();
                }
                EspNOW_received = 0;
                Player_EspNOW.is_waiting_for_feedback = 0;
              }
              break;

            case 2:
              if ((feedbackData.attacker_OG == OG) && (feedbackData.attacker_ID == ID)){
                Serial.print("L1 Treausre Collected Power Up:"); Serial.println(feedbackData.is_attackee_killed);
                switch (feedbackData.is_attackee_killed)
                {
                case bonus6HP:
                  num_bonus6HP ++ ;
                  tempNoti = "    PowerUp: +6 HP   ";
                  break;

                case bonus1MaxEn:
                  num_bonus1MaxEn ++ ;
                  tempNoti = "  PowerUp: +1 Max En ";
                  break;

                case bonus1MANA:
                  num_bonus1MANA ++ ;
                  tempNoti = "   PowerUp: +1 MANA  ";
                  break;

                case fiveminx2EnRegen:
                  num_fiveminx2EnRegen ++ ;
                  tempNoti = "PowerUp: x2 En Regen ";
                  break;

                case bomb:
                  num_bomb ++ ;
                  tempNoti = "  PowerUp: A Bomb!!  ";
                  break;
                
                default:
                  break;
                }
                tempNoti_start = millis();
                EspNOW_received = 0;
                Player_EspNOW.is_waiting_for_feedback = 0;
              }
            
            default:
              break;
            }
          }
        }
        else {
          EspNOW_received = 0;
          Player_EspNOW.is_waiting_for_feedback = 0;
        }
      }
    } ; 

    void update_display_waiting(){
      TreasureHunt_OLED.display_WaitingPage();
    }
    
    void update_display() {
      String noti_to_display;
      if (permNoti.length() == 0){
        if (tempNoti.length() != 0){
          unsigned long currTime = millis();
          if (currTime - tempNoti_start <= TEMP_NOTI_WAIT){
            if (((currTime - tempNoti_start)/TEMP_NOTI_BLINKING)%2 == 0)
              noti_to_display = tempNoti;
            else
              noti_to_display = " ";
          }
          else {
            noti_to_display = "";
            tempNoti = "";
          }
        }
      }
      else {
        noti_to_display = permNoti;
      }
      
      switch (currentPage)
      {
        case mainPage:
          TreasureHunt_OLED.display_mainPage(HP, En, noti_to_display, lastPageNav);
          break;
        case infoPage:
          TreasureHunt_OLED.display_infoPage(OG, ID, MANA, MaxEn, noti_to_display, lastPageNav);
          break;
//        case achievementPage:
//          TreasureHunt_OLED.display_achievementPage(numKilled, numL1Treasure, numL2Treasure, noti_to_display, lastPageNav);
//          break;
        case powerupPage:
          TreasureHunt_OLED.display_powerupPage(num_bonus6HP, 
                                              num_bonus1MaxEn,
                                              num_bonus1MANA,
                                              num_fiveminx2EnRegen,
                                              num_bomb,
                                              noti_to_display,
                                              PowerUpNav);
          break;
        
        default:
          lastPageNav = mainPage;
          currentPage = mainPage;
      }
    }

    void update_sound(){
      unsigned long currTime = millis();
      if (currTime - tempNoti_start >= NOTI_SOUND_DURATION){
        Player_Buzzer.end_sound();
      }
    };

    int get_game_state(){
      // retrieve game state from server
      // 0 mean game did not start
      // 1 mean in game
      // once the game has started then we dunnid to check anymore
   
      if (!gameStarted) {
        gameStarted = game_started_buffer;
        if (gameStarted) {
          int og = EEPROM.read(OG_add);
          bool isGL = EEPROM.read(isGL_add);
          int id = dbc.getPlayerID(og, my_MAC_address);
          EEPROM.write(ID_add, id);
          Serial.println(id);
          setup_initial_state(id, og, isGL); // initialize Player
          Player_Bluetooth.initialise(og);
        }
        return gameStarted;
      } else return 1;
    }

    void gameMainLoop(){
      if (!get_game_state()){
        handleJoystick_waiting();
        update_display_waiting();
      }
      else { 
        handleJoystickInGame();
        sendAction();
        receiveAction();
        receiveFeedback();
        update_player_state();
        update_display();
        update_sound();
      }
  };

  void gameBackgroundProcess(){
      if (!infectedWithVirus){
        Player_Bluetooth.scan();
        delay(50);
      }
      else{
        delay(50);
      }
   }
};

TreasureHuntPlayer PLAYER;
