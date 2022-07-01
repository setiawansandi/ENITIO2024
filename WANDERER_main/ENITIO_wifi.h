#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <esp_now.h>
#include <WiFi.h>

/**  WiFi Credentials **/
#define WIFI_SSID "TP-Link_E45E"  // For some reason if your WIFI_SSID contains spaces it wont work
#define WIFI_PASSWORD "63824377"

struct MAC_ADDRESS {
  int n1;
  int n2;
  int n3;
  int n4;
  int n5;
  int n6;
} ;

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

        MAC_ADDRESS retrieveMACAddressFromJSONArray(String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            MAC_ADDRESS mac_addr;
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return mac_addr;
            }
            Serial.println(JSON.stringify(json_obj["mac_address_part0"]));
            String n1 = JSON.stringify(json_obj["mac_address_part5"]);
            String n2 = JSON.stringify(json_obj["mac_address_part4"]);
            String n3 = JSON.stringify(json_obj["mac_address_part3"]);
            String n4 = JSON.stringify(json_obj["mac_address_part2"]);
            String n5 = JSON.stringify(json_obj["mac_address_part1"]);
            String n6 = JSON.stringify(json_obj["mac_address_part0"]);

            n1 = n1.substring(1, n1.length() - 1);
            n2 = n2.substring(1, n2.length() - 1);
            n3 = n3.substring(1, n3.length() - 1);
            n4 = n4.substring(1, n4.length() - 1);
            n5 = n5.substring(1, n5.length() - 1);
            n6 = n6.substring(1, n6.length() - 1);
            
            mac_addr.n1 = n1.toInt();
            mac_addr.n2 = n2.toInt();
            mac_addr.n3 = n3.toInt();
            mac_addr.n4 = n4.toInt();
            mac_addr.n5 = n5.toInt();
            mac_addr.n6 = n6.toInt();

            Serial.println(mac_addr.n6);

            return mac_addr;
        }
        
        String POST_Request(const char* server, const char* payload) {
            if (WiFi.status() == WL_CONNECTED) {
                HTTPClient http;
        
                http.begin(server);
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
            }
        };
    
    public:
        bool connectToWiFi() {
            // returns True if connected, False if timeout
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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

        bool registerWanderer(int OG, String mac_addr) {
            String url = DATABASE_URL + "player_registration";
            String httpRequestData = "{\"OG\": " + String(OG) + ", \"mac_address\": \"" + mac_addr + "\" }";
            Serial.println(httpRequestData);
            String jsonArray = POST_Request(url.c_str(), httpRequestData.c_str());
            Serial.println(jsonArray);
            return jsonArray != "{}";
        };

        bool hasGameStarted() {
            String url = DATABASE_URL + "game_status";
            String jsonArray = GET_Request(url.c_str());
            String gameStatus = retrieveParameterFromJSONArray("has_game_started", jsonArray);
            return gameStatus == "1";
        }

        int getPlayerID(int OG, String mac_addr) {
            String url = DATABASE_URL + "player_id/" + String(OG) + "/" + mac_addr;
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("player_id", jsonArray).toInt();
        };
        
        MAC_ADDRESS getDeviceMACAddress(int playerIdentifier) {
            String url = DATABASE_URL + "player/" + String(playerIdentifier);
            String jsonArray = GET_Request(url.c_str());
            return retrieveMACAddressFromJSONArray(jsonArray);
        };

        MAC_ADDRESS setTreasureAsOpened(String treasureName, int playerIdentifier) {
            String url = DATABASE_URL + "treasure/" + treasureName;
            String httpRequestData = "{\"player_identifier\": " + String(playerIdentifier) + " }";
            String jsonArray = POST_Request(url.c_str(), httpRequestData.c_str());
            return retrieveMACAddressFromJSONArray(jsonArray);
        };
};

DBConnection dbc;
