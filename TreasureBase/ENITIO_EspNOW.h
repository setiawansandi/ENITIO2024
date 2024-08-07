#include "ENITIO_wifi.h"

typedef struct feedback_message
{
  int msg_type = 1;
  int attackee_type; // 1 for player, 2 for Lvl 1 Treasure, 3 for Lvl 2 Treasure. 4 for base
  int attacker_CLAN;
  int attacker_ID;
  int attackee_CLAN;       // or Treasure ID
  bool is_attackee_killed; // base killed or not
  int powerup_received;
} feedback_message;

int EspNOW_received = 0;
int current_channel;
feedback_message EspNOW_recvData;

bool uploadFailedESPNOW = 0;

class EspNOW
{
private:
  feedback_message feedbackData;
  esp_now_peer_info_t peerInfo;
  uint8_t broadcastAddress[6];

public:
  int is_waiting_for_feedback = 0;

  void enable()
  {
    WiFi.mode(WIFI_AP_STA);
    if (esp_now_init() != ESP_OK)
    {
      Serial.println("Error initializing ESP-NOW");
      return;
    }
    Serial.println("OK");
    esp_now_register_send_cb(OnDataSent);
  }

  void getDeviceMACAddress(int attacker_CLAN, int attacker_ID)
  {
    broadcastAddress[0] = 4;
    broadcastAddress[1] = 8;
    broadcastAddress[2] = 1;
    broadcastAddress[3] = attacker_CLAN;
    broadcastAddress[4] = attacker_ID;
    broadcastAddress[5] = 1;
  }

  void send_data(int attackee_type, int attacker_CLAN, int attacker_ID, int attackee_CLAN, bool is_attackee_killed, int attacker_channel)
  {
    // delete old peer
    esp_now_del_peer(broadcastAddress);

    // Register peer
    feedbackData.attackee_type = attackee_type;
    feedbackData.attacker_CLAN = attacker_CLAN;
    feedbackData.attacker_ID = attacker_ID;
    feedbackData.attackee_CLAN = attackee_CLAN;
    feedbackData.is_attackee_killed = is_attackee_killed;
    feedbackData.powerup_received = 0;

    getDeviceMACAddress(attacker_CLAN, attacker_ID);

    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = attacker_channel;
    peerInfo.encrypt = false;

    // change WiFi channel to the receiver's WiFi channel
    // There might be problems if ESP32 is sending data halfway, check later
    current_channel = WiFi.channel(); // so that we can switch back later
    dbc.changeWiFiChannel(attacker_channel);

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add peer");
      return;
    }

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&feedbackData, sizeof(feedbackData));

    if (result == ESP_OK)
    {
      Serial.println("Sent with success");
    }
    else
    {
      Serial.println("Error sending the data");
    }
  }

  void send_data_attacked(int msg_type, int attackee_type, int attacker_CLAN, int attacker_ID, int attackee_CLAN, int is_attackee_killed, int attacker_channel)
  {
    // delete old peer
    esp_now_del_peer(broadcastAddress);

    // Register peer
    feedbackData.msg_type = msg_type;
    feedbackData.attackee_type = attackee_type;
    feedbackData.attacker_CLAN = attacker_CLAN;
    feedbackData.attacker_ID = attacker_ID;
    feedbackData.attackee_CLAN = attackee_CLAN;
    feedbackData.is_attackee_killed = is_attackee_killed;
    feedbackData.powerup_received = 0; // because WANDERers cannot send powerups to other devices
    uploadFailedESPNOW = is_attackee_killed;

    getDeviceMACAddress(attacker_CLAN, attacker_ID);

    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = attacker_channel;
    peerInfo.encrypt = false;

    // change WiFi channel to the receiver's WiFi channel
    // There might be problems if ESP32 is sending data halfway, check later
    current_channel = WiFi.channel(); // so that we can switch back later
    dbc.changeWiFiChannel(attacker_channel);

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add peer");
      return;
    }

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&feedbackData, sizeof(feedbackData));

    if (result == ESP_OK)
    {
      Serial.println("Success");
    }
    else if (result == ESP_ERR_ESPNOW_NOT_INIT)
    {
      // How did we get so far!!
      Serial.println("ESPNOW not Init.");
    }
    else if (result == ESP_ERR_ESPNOW_ARG)
    {
      Serial.println("Invalid Argument");
    }
    else if (result == ESP_ERR_ESPNOW_INTERNAL)
    {
      Serial.println("Internal Error");
    }
    else if (result == ESP_ERR_ESPNOW_NO_MEM)
    {
      Serial.println("ESP_ERR_ESPNOW_NO_MEM");
    }
    else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
    {
      Serial.println("Peer not found.");
    }
    else
    {
      Serial.println("Not sure what happened");
    }
  }

  void disable()
  {
    esp_now_deinit();
  }

  static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
  {
    memcpy(&EspNOW_recvData, incomingData, sizeof(EspNOW_recvData));
    EspNOW_received = 1;
  }

  static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
  {
    Serial.print("WiFi Channel: ");
    Serial.println(WiFi.channel());
    Serial.print("\r\nLast Packet Send Status:\t");
    if (status == ESP_NOW_SEND_SUCCESS)
    {
      Serial.println("Delivery Success");
    }
    else
    {
      Serial.println("Delivery Failed");
      failed_kill_feedback++;
      failed_kill_CLAN[current_failed_save_pointer] = mac_addr[3];
      failed_kill_ID[current_failed_save_pointer] = mac_addr[4];
      current_failed_save_pointer++;
      if (current_failed_save_pointer >= 50)
        current_failed_save_pointer -= 50;
    }
    Serial.println("Changing Back to original channel");
    dbc.changeWiFiChannel(current_channel);
  }

  feedback_message get_feedback_received()
  {
    return EspNOW_recvData;
  }
};

EspNOW TreasureBase_EspNOW;
