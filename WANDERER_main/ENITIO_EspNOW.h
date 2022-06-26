#include <esp_now.h>
#include <WiFi.h>
#include "ENITIO_wifi.h"

typedef struct feedback_message {
  int attacker_OG;
  int attacker_ID;
  int attackee_OG;
  bool is_attackee_killed;
} feedback_message;

int EspNOW_received = 0;
feedback_message EspNOW_recvData;

class EspNOW {
  private:
    feedback_message feedbackData;
    esp_now_peer_info_t peerInfo;
    uint8_t broadcastAddress[6];
    
  public:
    int is_waiting_for_feedback = 0;
    int is_feedbacking = 0;
    
    void enable() {
      // WiFi.mode(WIFI_STA);
      WiFi.mode(WIFI_AP_STA);
      if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
      }
      Serial.println("OK");
      esp_now_register_send_cb(OnDataSent);
      esp_now_register_recv_cb(OnDataRecv);
    }

    void getDeviceMACAddress(int attacker_OG, int attacker_ID){
//      int playerIdentifier = attacker_OG*pow(16,2) + attacker_ID;
//      MAC_ADDRESS target_addr = dbc.getDeviceMACAddress(playerIdentifier);
//      broadcastAddress[0] = target_addr.n1;
//      broadcastAddress[1] = target_addr.n2;
//      broadcastAddress[2] = target_addr.n3;
//      broadcastAddress[3] = target_addr.n4;
//      broadcastAddress[4] = target_addr.n5;
//      broadcastAddress[5] = target_addr.n6;
     if ((attacker_OG == 0) && (attacker_ID == 0)){
      broadcastAddress[0] = 12;
      broadcastAddress[1] = 184;
      broadcastAddress[2] = 21;
      broadcastAddress[3] = 246;
      broadcastAddress[4] = 3;
      broadcastAddress[5] = 240;
     }
     if ((attacker_OG == 0) && (attacker_ID == 1)){
      broadcastAddress[0] = 12;
      broadcastAddress[1] = 184;
      broadcastAddress[2] = 21;
      broadcastAddress[3] = 245;
      broadcastAddress[4] = 231;
      broadcastAddress[5] = 112;
     }
     if ((attacker_OG == 0) && (attacker_ID == 2)){
      broadcastAddress[0] = 120;
      broadcastAddress[1] = 33;
      broadcastAddress[2] = 132;
      broadcastAddress[3] = 146;
      broadcastAddress[4] = 11;
      broadcastAddress[5] = 184;
     }
     if ((attacker_OG == 1) && (attacker_ID == 0)){
      broadcastAddress[0] = 12;
      broadcastAddress[1] = 184;
      broadcastAddress[2] = 21;
      broadcastAddress[3] = 245;
      broadcastAddress[4] = 233;
      broadcastAddress[5] = 60;
     }
     if ((attacker_OG == 1) && (attacker_ID == 1)){
      broadcastAddress[0] = 232;
      broadcastAddress[1] = 49;
      broadcastAddress[2] = 205;
      broadcastAddress[3] = 215;
      broadcastAddress[4] = 5;
      broadcastAddress[5] = 12;
     }
     if ((attacker_OG == 1) && (attacker_ID == 2)){
      broadcastAddress[0] = 232;
      broadcastAddress[1] = 49;
      broadcastAddress[2] = 205;
      broadcastAddress[3] = 214;
      broadcastAddress[4] = 248;
      broadcastAddress[5] = 236;
     }
     }
  
   void send_data(int attacker_OG, int attacker_ID, int attackee_OG, bool is_attackee_killed){
            // Register peer
      feedbackData.attacker_OG = attacker_OG;
      feedbackData.attacker_ID = attacker_ID;
      feedbackData.attackee_OG = attackee_OG;
      feedbackData.is_attackee_killed = is_attackee_killed;
  
      getDeviceMACAddress(attacker_OG, attacker_ID);
      
      memcpy(peerInfo.peer_addr, broadcastAddress, 6);
      peerInfo.channel = 0;  
      peerInfo.encrypt = false;
        
      // Add peer     
      if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
      }
      
      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &feedbackData, sizeof(feedbackData));
       
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }
      
      esp_now_del_peer(broadcastAddress);
    }

    void disable(){
      esp_now_deinit();
    } 

    static void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
      memcpy(&EspNOW_recvData, incomingData, sizeof(EspNOW_recvData));
      EspNOW_received = 1;
    }

    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
      Serial.print("\r\nLast Packet Send Status:\t");
      Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    }

    feedback_message get_feedback_received(){
      return EspNOW_recvData;
    }
};

EspNOW Player_EspNOW;
