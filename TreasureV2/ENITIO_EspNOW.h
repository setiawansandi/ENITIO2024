#include "ENITIO_wifi.h"

typedef struct feedback_message
{
  int msg_type = 1;
  int attackee_type; // 1 for player, 2 for Lvl 1 Treasure, 3 for Lvl 2 Treasure
  int attacker_CLAN;
  int attacker_ID;
  int attackee_CLAN; // or Treasure ID
  bool is_attackee_killed;
  int powerup_received;
} feedback_message;

// Global copy of slave
#define NUM_BOMBS_TARGET 15

int EspNOW_received = 0;
int current_channel;
feedback_message EspNOW_recvData;

class EspNOW
{
private:
  feedback_message feedbackData;
  esp_now_peer_info_t peerInfo;
  uint8_t broadcastAddress[6];

  esp_now_peer_info_t bomb_targets[NUM_BOMBS_TARGET] = {};
  int target_count = 0;

public:
  int num_bombed = 0;
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

  void send_data(int attackee_type, int attacker_CLAN, int attacker_ID, int attackee_CLAN, bool is_attackee_killed, int powerup_received, int attacker_channel)
  {
    // delete old peer
    esp_now_del_peer(broadcastAddress);

    // Register peer
    feedbackData.msg_type = 1;
    feedbackData.attackee_type = attackee_type;
    feedbackData.attacker_CLAN = attacker_CLAN;
    feedbackData.attacker_ID = attacker_ID;
    feedbackData.attackee_CLAN = attackee_CLAN;
    feedbackData.is_attackee_killed = is_attackee_killed;
    feedbackData.powerup_received = powerup_received;

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

  void ScanForBombTarget()
  {
    WiFi.scanNetworks(false, false, false, 100);
    int8_t scanResults = WiFi.scanComplete();
    // reset slaves
    memset(bomb_targets, 0, sizeof(bomb_targets));
    target_count = 0;
    Serial.println();
    if (scanResults == 0)
    {
      Serial.println("No WiFi devices in AP Mode found");
      return;
    }
    Serial.print("Found ");
    Serial.print(scanResults);
    Serial.println(" devices ");
    for (int i = 0; i < scanResults && target_count < NUM_BOMBS_TARGET; ++i)
    {
      uint8_t *mac = WiFi.BSSID(i);
      if (mac[0] == 0x04 && mac[1] == 0x08 && mac[2] == 0x01 && (mac[5] == 0x00 || mac[5] == 0x01))
      {
        memcpy(bomb_targets[target_count].peer_addr, mac, 6);
        bomb_targets[target_count].channel = dbc.getClanWiFiChannel((int)mac[3]); // pick a channel
        bomb_targets[target_count].encrypt = 0;                                   // no encryption
        target_count++;
      }
    }
    WiFi.scanDelete();
    if (target_count > 0)
    {
      Serial.print(target_count);
      Serial.println(" Target(s) found, processing..");
    }
    else
    {
      Serial.println("No Target Found, trying again.");
    }
  }

  void SendBombToAllTargets(int attacker_CLAN, int attacker_ID)
  {
    int i;
    for (i = 0; i < target_count; i++)
    {
      // change WiFi channel to the receiver's WiFi channel
      // There might be problems if ESP32 is sending data halfway, check later
      current_channel = WiFi.channel(); // so that we can switch back later
      dbc.changeWiFiChannel(bomb_targets[i].channel);

      if (esp_now_add_peer(&bomb_targets[i]) != ESP_OK)
      {
        Serial.println("Failed to add peer");
        return;
      }
      feedbackData.msg_type = 2;
      feedbackData.attackee_type = 1;
      feedbackData.attacker_CLAN = attacker_CLAN;
      feedbackData.attacker_ID = attacker_ID;
      feedbackData.attackee_CLAN = 255;
      feedbackData.is_attackee_killed = 255;

      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(bomb_targets[i].peer_addr, (uint8_t *)&feedbackData, sizeof(feedbackData));

      if (result == ESP_OK)
      {
        Serial.println("Sent with success");
      }
      else
      {
        Serial.println("Error sending the data");
      }
      delay(10);
      esp_now_del_peer(bomb_targets[i].peer_addr);
    }
    num_bombed = target_count;
    target_count = 0;
  }
};

EspNOW TreasureV2_EspNOW;
