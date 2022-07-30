#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <esp_now.h>

// WiFi Based on https://github.com/martinius96/ESP32-eduroam/blob/master/2022/test_2.0.3/test_2.0.3.ino

/**  WiFi Credentials **/
#define EAP_ANONYMOUS_IDENTITY  ""
#define EAP_IDENTITY  "quan005@student.main.ntu.edu.sg"
#define EAP_PASSWORD  "P1&S1bTV!30121976"
#define HOME_WIFI_SSID "TP-Link_E45E"
#define HOME_WIFI_PASSWORD "63824377"
const char *ssid = "NTUSECURE";
int wifi_reconnect_counter = 0;

class DBConnection {
    private:
        String DATABASE_URL = "https://kahleong.pythonanywhere.com/";
        String GET_Request(const char* server) {
            HTTPClient http;
            http.begin(server);
            int httpResponseCode = http.GET();
        
            String payload = "{}";
        
            if (httpResponseCode > 0) {
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
        
        String POST_Request(const char* server, const char* payload) {
            if (WiFi.status() == WL_CONNECTED) {
                HTTPClient http;
                Serial.println("check1");
                http.begin(server);
                http.addHeader("Content-Type", "application/json");
                Serial.println("check2");
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
            }
        };
    
    public:
        bool connectToWiFi() {
            // returns True if connected, False if timeout
           WiFi.begin(HOME_WIFI_SSID, HOME_WIFI_PASSWORD);
            //  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);
            Serial.print("Connecting to Wi-Fi");
            int counter = 0;
            while (WiFi.status() != WL_CONNECTED) {
                Serial.print(".");
                delay(500);
                counter++;
                if (counter >= 60) {
                    // timeout
                    return false;
                }
            };
            Serial.print("Connected with IP: "); Serial.println(WiFi.localIP());
            return true;
        };

        int hasGameStarted() {
            String url = DATABASE_URL + "game_status";
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("has_game_started", jsonArray).toInt();
        }
        
        String getDeviceMACAddress(int playerIdentifier) {
            String url = DATABASE_URL + "player/" + String(playerIdentifier);
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("mac_address", jsonArray);
        };

        String setTreasureAsOpened(String treasureName, int og, int participant_id) {
            String url = DATABASE_URL + "treasure/1/" + treasureName + "/" + String(og) + "/" + String(participant_id);
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("mac_address", jsonArray);
        };
};

DBConnection dbc;
