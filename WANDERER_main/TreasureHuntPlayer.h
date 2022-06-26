#define ENABLE_add 0 
#define ID_add 1
#define HP_add 2
#define En_add 3
#define MaxEn_add 4
#define MANA_add 5

#define ACTION_RECV_WAIT 150 // [ms] 
#define TEMP_NOTI_WAIT 6000 // including being Attacked, Healed, Successfully Attack and Successfully Collect
#define NOTI_SOUND_DURATION 200
#define TEMP_NOTI_BLINKING 300
#define NAV_WAIT 4000
#define FEEDBACK_WAIT 5000

#define EN_RECOVER_DURATION 10000
#define MANA_DECAY_DURATION 10000
#define VIRUS_DECAY_DURATION 10000

class TreasureHuntPlayer
{
  private:
    int ID; 
    int OG;
    bool _isGL;  
    int HP = 12;
    int MaxHP = 12;
    int En = 5; // energy
    int MaxEn = 5;
    int MANA = 3;

    action_id action;

    unsigned long lastActionReceived = 0;

    int currentPage = 0;
    int lastPageNav = 0;
    
    String tempNoti = "";
    String permNoti = "";

    unsigned long Nav_start = 0;
    unsigned long tempNoti_start = 0;
    unsigned long last_mana_decay = 0;
    unsigned long last_en_recover = 0;
    unsigned long last_hp_decay = 0;

    unsigned long start_receiving_feedback = 0;

    int numKilled = 0;
    int numL1Treasure = 0;
    int numL2Treasure = 0;

    int LastL1TreasureCollected = -1;

    bool infectedWithVirus = 0;

  public:
    void setup_initial_state(int id, int og, bool isGL) {
      ID = id;
      OG = og ;
      _isGL = isGL;
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
        command_digits.digit1 = MANA;

        ir_signal send_signal;
        send_signal.address = address_digits;
        send_signal.command = command_digits;

        Player_IR.send(send_signal, 1);

        Player_EspNOW.is_waiting_for_feedback = 1;
        start_receiving_feedback = millis();

        En--;
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
           if ((action_ == 4) && (OG_ == OG) && (ID_ == ID)) {
            int TreasureID = IRsignal_.command.digit1 + (IRsignal_.command.digit2 << 4);
            if (TreasureID != LastL1TreasureCollected) {
              numL1Treasure++;
              tempNoti = "Collected L1 Treasure";
              LastL1TreasureCollected = TreasureID;
              tempNoti_start = millis();
            }
           }
           else if (((OG_ != OG) && (action_ == attack)) || ((action_ == heal) && (OG_ == OG) && (ID_ != ID)) || ((action_ == heal) && (OG_ != OG)))
                handleAction(OG_, ID_, action_, MANA_);
           }
        }
      }

    void update_player_state(){
      if (HP == 0){
        En = 0;
        permNoti = "    You Are Killed!     ";
        unsigned long currTime = millis();
        if (currTime - last_mana_decay >= MANA_DECAY_DURATION){
          MANA--;
          last_mana_decay = currTime;
        }
      }
      else {
        permNoti = "";
        unsigned long currTime = millis();
        last_mana_decay = currTime;
        if (En >= MaxEn){
          last_en_recover = currTime;
        }
        else {
          if (currTime - last_en_recover >= EN_RECOVER_DURATION){
            En++ ;
            last_en_recover = currTime;
          }
        }

        // Check if player is infected with virus
        if (infectedWithVirus) {
            if (currTime - last_hp_decay >= VIRUS_DECAY_DURATION) {
                HP--;
                last_hp_decay = currTime;
            };
            permNoti = "    You Are Infected!   ";
        } else if (!_isGL){
            // currently not infected with virus AND is not healer
            // check if nearby devices are transmitting virus
            if (Player_Bluetooth.isThereVirus) {
                infectedWithVirus = true;
                HP--;
                last_hp_decay = currTime;
                permNoti = "    You Are Infected!   ";
            }
        }
      }
    }

    void handleJoystick(){
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
          lastPageNav--;
          if (lastPageNav > 4) lastPageNav -= 5;
          if (lastPageNav < 0) lastPageNav += 5;
          Nav_start = millis();
          Player_joystick.set_state();
          break;

         case right:
          lastPageNav++;
          if (lastPageNav > 4) lastPageNav -= 5;
          if (lastPageNav < 0) lastPageNav += 5;
          Nav_start = millis();
          Player_joystick.set_state();
          break;
          
        case button:
          if (lastPageNav != currentPage)
            currentPage = lastPageNav;
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

    void handleAction(int OG_, int ID_, int action_, int MANA_){
      Serial.println(action_);
      switch (action_)
      {
      case attack:
        if(HP > 0) {
          HP = max(HP - MANA_, 0);
          Serial.printf("Attacked. Current HP: %d \n", HP);
          tempNoti = "       Attacked      ";
          tempNoti_start = millis();
          feedback(OG_, ID_);
          Player_Buzzer.sound(NOTE_E3);
        }
        break;
        
      case heal:
        HP = min(HP + MANA_, MaxHP);
        tempNoti = "        Healed       ";
        tempNoti_start = millis();
        infectedWithVirus = 0;
        break;
      
      default:
        break;
      }
    }

    void feedback(int OG_, int ID_){
      bool killed = (HP == 0);
      Player_EspNOW.send_data(OG_, ID_, OG, killed);
    } ;
      
    void receiveFeedback(){
      if (Player_EspNOW.is_waiting_for_feedback == 1) {
        unsigned long currTime = millis();
        if (currTime - start_receiving_feedback <= FEEDBACK_WAIT){
          if (EspNOW_received == 1){
            feedback_message feedbackData = Player_EspNOW.get_feedback_received();
            if ((feedbackData.attacker_OG == OG) && (feedbackData.attacker_ID == ID)){
              if (feedbackData.is_attackee_killed == true) {
                tempNoti = " You killed a person ";
                tempNoti_start = millis();
              }
              else {
                tempNoti = " Attack successfully ";
                tempNoti_start = millis();
              }
              EspNOW_received = 0;
              Player_EspNOW.is_waiting_for_feedback = 0;
            }
          }
        }
        else {
          EspNOW_received = 0;
          Player_EspNOW.is_waiting_for_feedback = 0;
        }
      }
    } ; 
    
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
        case achievementPage:
          TreasureHunt_OLED.display_achievementPage(numKilled, numL1Treasure, numL2Treasure, noti_to_display, lastPageNav);
          break;
        case leaderboardPage:
          TreasureHunt_OLED.display_leaderboardPage();
          break;
      }
    }

    void update_sound(){
      unsigned long currTime = millis();
      if (currTime - tempNoti_start >= NOTI_SOUND_DURATION){
        Player_Buzzer.end_sound();
      }
    };

    void gameMainLoop(){
      handleJoystick();
      sendAction();
      receiveAction();
      receiveFeedback();
      update_player_state();
      update_display();
      update_sound();
  };

  void gameBackgroundProcess(){
    Player_Bluetooth.scan();
    delay(50);
  }
};

TreasureHuntPlayer PLAYER;
