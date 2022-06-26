// upload this code whenever you want to start testing
// comment out the clearEEPROM() function when used on real game

#include <EEPROM.h>
#include "ENITIO_ir.h"
#define EEPROM_SIZE 10
#define delay_send 1000

#define ENABLE_add 0

typedef enum {
    do_nothing = 0,
    attack = 1,
    collect = 2,
    heal = 3,
} action_id ;


class TreasureLevel1
{
  private:
    int ID;

  public:
    
    int OG_, ID_, En_, action_; 

    TreasureLevel1(int id){
      ID = id;
    }

    void init_treasure(){
      EEPROM.write(ENABLE_add, 1);
      EEPROM.commit();
    }
    
    void receiveAction() {
      if (TreasureLevel1_IR.available()) {
         ir_signal IRsignal_ = TreasureLevel1_IR.read();

         OG_ = IRsignal_.address.digit2;
         ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);
    
         En_ = IRsignal_.command.digit1;
         action_ = IRsignal_.command.digit0;
    
         Serial.printf("%d %d %d %d \n", action_, En_, ID_, OG_);
  
         if (action_ == collect) {
          delay(delay_send);
          sendFeedback();
          EEPROM.write(ENABLE_add, 2);
          EEPROM.commit();
         }
        }
      }
  
    void sendFeedback() {
      ir_signal Feedback;
      uint16_hex_digits f_address, f_command;
      f_address.digit0 = ID_;
      f_address.digit2 = OG_;
      f_command.digit1 = ID;
      f_command.digit0 = 4;
      Feedback.address = f_address;
      Feedback.command = f_command;
      TreasureLevel1_IR.send(Feedback, 10);
    }
};

TreasureLevel1 Treasure(3);

void clearEEPROM(){
  int i;
  for (i=0; i<EEPROM_SIZE; i++){
    EEPROM.write(i,0);
  }
}

void setup() {
  TreasureLevel1_IR.enable();
  EEPROM.begin(EEPROM_SIZE);
//  clearEEPROM();
  if (EEPROM.read(ENABLE_add) == 0) {
    Treasure.init_treasure();
  }
  Serial.begin(115200);
}

void loop() {
  if(EEPROM.read(ENABLE_add) == 1){
    Treasure.receiveAction();
  }
}
