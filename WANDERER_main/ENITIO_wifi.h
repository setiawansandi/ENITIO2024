#include <HTTPClient.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <Arduino_JSON.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// WiFi Based on https://github.com/martinius96/ESP32-eduroam/blob/master/2022/eduroam/eduroam.ino

/**  WiFi Credentials **/
#define EAP_ANONYMOUS_IDENTITY  ""
#define EAP_IDENTITY  "@student.main.ntu.edu.sg"
#define EAP_PASSWORD  ""
#define HOME_WIFI_SSID ""
#define HOME_WIFI_PASSWORD ""

const char *ssid = "NTUSECURE";
int wifi_reconnect_counter = 0;
unsigned long last_disconnected_time = 0;
int HTTP_TIMEOUT = 30 * 1000;

struct MAC_ADDRESS {
  int n1;
  int n2;
  int n3;
  int n4;
  int n5;
  int n6;
} ;

struct GAME_CONSTANTS {
    int EN_RECOVER_DURATION;
    int VIRUS_DECAY_DURATION;
    int VIRUS_IMMUNITY_DURATION;
    int VIRUS_INFECTION_PROBABILITY;  // integer between 0 and 100
    int PARTICIPANT_MaxHP;
    int GL_MaxHP;
    int PARTICIPANT_MaxEn;
    int GL_MaxEn;
    int INITIAL_MULTIPLIER;
    int HEAL_MULTIPLIER;
    int MAX_ATTACK_MULTIPLIER;
    int MAX_COLLECT_MULTIPLIER;
    int BOMB_HP_DEDUCTION;
    int KILL_UPDATE_SERVER_INTERVAL;
    int HTTP_TIMEOUT;
    int revival_MULTIPLIER;
};

struct FailedFeedbackStatistics {
    int num_kills;
    int num_powerups;
};

class DBConnection {
    private:
        String DATABASE_URL = "https://enitiotreasurehunt.link/";
        String DATABASE_DEBUG_URL = "http://enitiotreasurehunt.link/";
        bool DEBUG = true;
        String GET_Request(String route) {
            HTTPClient http;
            String url;
            http.setTimeout(HTTP_TIMEOUT);
            if (!DEBUG) {
                url = DATABASE_URL + route;
            } else {
                url = DATABASE_DEBUG_URL + route;
                Serial.print("DEBUG GET "); Serial.println(url);
                http.begin(url.c_str());
            }
            int httpResponseCode = http.GET();
            String payload = "{}";
        
            if (httpResponseCode > 0) {
                Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
                payload = http.getString();
            } else {
                Serial.print("Error code: "); Serial.println(httpResponseCode);
            }
            http.end();
            return payload;
        };

        String retrieveParameterFromJSONArray(String parameter, String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return "";
            }
            return JSON.stringify(json_obj[parameter]);
        };

        FailedFeedbackStatistics retrieveStatisticsFromJSONArray(String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            FailedFeedbackStatistics feedback_stats;
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return feedback_stats;
            }
            feedback_stats.num_kills = JSON.stringify(json_obj["num_kills"]).toInt();
            feedback_stats.num_powerups = JSON.stringify(json_obj["num_powerups"]).toInt();
            return feedback_stats;
        }
        
        String POST_Request(String route, const char* payload) {
            HTTPClient http;
            http.setTimeout(HTTP_TIMEOUT);
            String url;
            if (!DEBUG) {
                url = DATABASE_URL + route;
            } else {
                url = DATABASE_DEBUG_URL + route;
                Serial.print("DEBUG POST "); Serial.println(url);
                http.begin(url.c_str());
            }
            Serial.println(url);
            http.addHeader("Content-Type", "application/json");
            int httpResponseCode = http.POST(payload);
            String responsePayload = "{}";
            if (httpResponseCode > 0) {
                Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
                responsePayload = http.getString();
            }
            else {
                Serial.print("Error code: "); Serial.println(httpResponseCode);
            }
            http.end();
            return responsePayload;    
        };
    
    public:
        void startWiFiConnection() {
            // WiFi.begin(HOME_WIFI_SSID, HOME_WIFI_PASSWORD);
            WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);
        }
        bool connectToWiFi() {
          Serial.print("Connecting to Wi-Fi");

          // Attempt to connect to Wi-Fi
          startWiFiConnection();
          for (int i = 0; i < 10; i++) {
            if (WiFi.status() == WL_CONNECTED) {
              Serial.print("Connected with IP: ");
              Serial.println(WiFi.localIP());
              return true;
            }
            delay(500);
            Serial.print(".");
          }
          // Timeout
          Serial.println("Connection Timed Out.");
          return false;
        }

        bool registerWanderer(int CLAN, String mac_addr) {
            bool connectedToWiFi = connectToWiFi();
            if (!connectedToWiFi) {
                Serial.println("Rebooting...");
                ESP.restart();
            }

            String url = "register";
            String httpRequestData = "{\"clan\": " + String(CLAN) + ", \"mac_address\": \"" + mac_addr + "\" }";
            Serial.println(httpRequestData);
            String jsonArray = POST_Request(url, httpRequestData.c_str());
            Serial.println(jsonArray);
            return jsonArray != "{\"result\":\"OK\"}";
        };

        bool hasGameStarted() {
            if (WiFi.status() != WL_CONNECTED) {
                bool connectedToWiFi = connectToWiFi();
                if (!connectedToWiFi) {
                    Serial.print("WiFi not connected, so unable to retrieve game status. ");
                    Serial.println("Override gameStatus variable to force the game to start.");
                    return false;
                }
            }
            String url = "game_status";
            String jsonArray = GET_Request(url);
            String gameStatus = retrieveParameterFromJSONArray("has_game_started", jsonArray);
            return gameStatus == "1";
        }

        int getPlayerID(int CLAN, String mac_addr) {
            if (WiFi.status() != WL_CONNECTED) {
                bool connectedToWiFi = connectToWiFi();
                if (!connectedToWiFi) {
                    Serial.print("WiFi not connected, so unable to retrieve playerID. ");
                    return false;
                }
            }
            Serial.println("Retrieving Participant ID from server...");
            String url = "player_id/" + String(CLAN) + "/" + mac_addr;
            String jsonArray = GET_Request(url);
            Serial.println(jsonArray);
            return retrieveParameterFromJSONArray("player_id", jsonArray).toInt();
        };

        GAME_CONSTANTS getGameConstants() {
            if (WiFi.status() != WL_CONNECTED) {
                bool connectedToWiFi = connectToWiFi();
                if (!connectedToWiFi) {
                    Serial.print("WiFi not connected, so unable to retrieve constants. ");
                    GAME_CONSTANTS game_const;
                    return game_const;
                }
            }
            String url = "get_game_constants";
            String jsonArray = GET_Request(url);
            return retrieveGameConstantsFromJSONArray(jsonArray);
        };

        String getPlayerIDandGameConstantsJSON(int CLAN, String mac_addr) {
            if (WiFi.status() != WL_CONNECTED) {
                bool connectedToWiFi = connectToWiFi();
                if (!connectedToWiFi) {
                    Serial.print("WiFi not connected, so unable to retrieve playerID. ");
                    return "";
                }
            }
            Serial.println("Retrieving Participant ID from server...");
            String url = "player_id/" + String(CLAN) + "/" + mac_addr;
            String jsonArray = GET_Request(url);
            Serial.println(jsonArray);
            return jsonArray;
        }

        int getPlayerIDFromJSON(String jsonArray) {
            return retrieveParameterFromJSONArray("player_id", jsonArray).toInt();
        }

        GAME_CONSTANTS retrieveGameConstantsFromJSONArray(String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            GAME_CONSTANTS game_const;
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return game_const;
            }
            game_const.EN_RECOVER_DURATION = JSON.stringify(json_obj["EN_RECOVER_DURATION"]).toInt();
            game_const.VIRUS_DECAY_DURATION = JSON.stringify(json_obj["VIRUS_DECAY_DURATION"]).toInt();
            game_const.VIRUS_IMMUNITY_DURATION = JSON.stringify(json_obj["VIRUS_IMMUNITY_DURATION"]).toInt();
            game_const.VIRUS_INFECTION_PROBABILITY = JSON.stringify(json_obj["VIRUS_INFECTION_PROBABILITY"]).toInt();
            game_const.PARTICIPANT_MaxHP = JSON.stringify(json_obj["PARTICIPANT_MaxHP"]).toInt();
            game_const.GL_MaxHP = JSON.stringify(json_obj["GL_MaxHP"]).toInt();
            game_const.PARTICIPANT_MaxEn = JSON.stringify(json_obj["PARTICIPANT_MaxEn"]).toInt();
            game_const.GL_MaxEn = JSON.stringify(json_obj["GL_MaxEn"]).toInt();
            game_const.INITIAL_MULTIPLIER = JSON.stringify(json_obj["INITIAL_MULTIPLIER"]).toInt();
            game_const.HEAL_MULTIPLIER = JSON.stringify(json_obj["HEAL_MULTIPLIER"]).toInt();
            game_const.revival_MULTIPLIER = JSON.stringify(json_obj["REVIVAL_MULTIPLIER"]).toInt();
            game_const.MAX_ATTACK_MULTIPLIER = JSON.stringify(json_obj["MAX_ATTACK_MULTIPLIER"]).toInt();
            game_const.MAX_COLLECT_MULTIPLIER = JSON.stringify(json_obj["MAX_COLLECT_MULTIPLIER"]).toInt();
            game_const.BOMB_HP_DEDUCTION = JSON.stringify(json_obj["BOMB_HP_DEDUCTION"]).toInt();
            game_const.KILL_UPDATE_SERVER_INTERVAL = JSON.stringify(json_obj["SERVER_UPDATE_INTERVAL"]).toInt();
            game_const.HTTP_TIMEOUT = JSON.stringify(json_obj["HTTP_TIMEOUT"]).toInt();

            return game_const;
        };

        FailedFeedbackStatistics sendGameStatistics(int CLAN, int ID, int kills, int num_level1_treasure, int num_level2_treasure) {
            if (WiFi.status() != WL_CONNECTED) {
                bool connectedToWiFi = connectToWiFi();
                if (!connectedToWiFi) {
                    Serial.print("WiFi not connected, so unable to send statistics");
                    FailedFeedbackStatistics feedback_stats;
                    return feedback_stats;
                }
            }
            String url = "player_score";
            String httpRequestData = "{\"clan\": " + String(CLAN) + ", \"id\": " + String(ID) + ", \"num_kills\": " + String(kills);
            httpRequestData = httpRequestData + ", \"level1\": " + String(num_level1_treasure) + ", \"level2\": " + String(num_level2_treasure);
            httpRequestData = httpRequestData + "}";
            Serial.println(httpRequestData);
            String jsonArray = POST_Request(url, httpRequestData.c_str());
            Serial.println(jsonArray);
            return retrieveStatisticsFromJSONArray(jsonArray);
            WiFi.disconnect(true);
        };

        bool uploadFailedFeedback(int CLAN, int participant_id) {
            if (WiFi.status() != WL_CONNECTED) {
                bool connectedToWiFi = connectToWiFi();
                if (!connectedToWiFi) {
                    Serial.print("WiFi not connected, so unable to send feedback");
                    return false;
                }
            }
            String url = "player_kill/" + String(CLAN) + "/" + String(participant_id);
            String jsonArray = GET_Request(url.c_str());
            return jsonArray != "{}";
            WiFi.disconnect(true);
        };

        void changeWiFiChannel(int targetChannelID) {
            Serial.print("Current WiFi Channel: "); Serial.println(WiFi.channel());
            esp_wifi_set_promiscuous(true);
            esp_wifi_set_channel(targetChannelID, WIFI_SECOND_CHAN_NONE);
            esp_wifi_set_promiscuous(false);
            Serial.print("NEW WiFi Channel: "); Serial.println(WiFi.channel());
        }

        int getClanWiFiChannel(int CLAN) {
            switch(CLAN) {
                case INVICTA:
                    return 1;
                case DYNARI:
                    return 6;
                case EPHILIA:
                    return 6;
                case AKRONA:
                    return 11;
                case SOLARIS:
                    return 11;
                default:
                    return 1;
            }
        }
};

DBConnection dbc;
