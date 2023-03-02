#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Arduino_JSON.h>
#include <esp_now.h>
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
int HTTP_TIMEOUT = 30 * 1000;

const char* root_ca PROGMEM = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDljCCAn6gAwIBAgIQNVdBpOD85JGeYm6zWCKCmjANBgkqhkiG9w0BAQsFADBI\n" \
"MRswGQYDVQQDDBJFU0VUIFNTTCBGaWx0ZXIgQ0ExHDAaBgNVBAoME0VTRVQsIHNw\n" \
"b2wuIHMgci4gby4xCzAJBgNVBAYTAlNLMB4XDTIzMDIyNzE1NTU1N1oXDTIzMDUy\n" \
"ODE1NTU1NlowIjEgMB4GA1UEAxMXZW5pdGlvdHJlYXN1cmVodW50LmxpbmswggEi\n" \
"MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDXIa37ToQjxiy8koxWUw0Rpzdv\n" \
"/11odPGgIbMGpT05DD/UNK9CaxTfJLw3ftT4EEcPHFzavvTXG7uJ6OOtXfi0WIbf\n" \
"fgiKD/ve8ch6oc9Vqp7HbFWZ0cg3JKFwkGw5MF2JO6vTkzbzht97dj4UAj5FMh8d\n" \
"EGPFMR7plbf8P5RGuQCdzrPk+i3/6N/jo6bSCYr1hqxCMQgeGkTLFr+F7I7emkdQ\n" \
"JcOfaqAcJalnKS39PDqf44bhza2xz2cxKllgS4fxCKJQH/ApyTaHDMLHPlhMNStj\n" \
"uf6WDs6c1K8XIj5AmGYvFxuRyhzmcbdAG2hv3WyPGjcDAeu53LK/wejj/h05AgMB\n" \
"AAGjgaEwgZ4wCwYDVR0PBAQDAgWgMB0GA1UdDgQWBBQtHxvNJX/jCsJbVPZZFiai\n" \
"0ndfqTAfBgNVHSMEGDAWgBR1fBmAqE/V8PIcSLwG0TBEt6AnrTAdBgNVHSUEFjAU\n" \
"BggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIwADAiBgNVHREEGzAZghdl\n" \
"bml0aW90cmVhc3VyZWh1bnQubGluazANBgkqhkiG9w0BAQsFAAOCAQEAuThNYcXz\n" \
"VVMGPYNICi/puKYGxkiS019soTbIwRjq3dNKcWF4sB/ba2OjioM7UCnJ1TE9uEGO\n" \
"vkjj7gbSIcJakfv0idFrj5i3q9Kgo3bnpqcm+LdpEo6+FvR+Lra+ptxG8lqj1p4l\n" \
"luth4Pf6Sf1Ck2iPgjIoTDtdO/xW+0oF2pG9dEAFnHMabjS6F+CpR126SHv80gcW\n" \
"D86l8F9Z5/r7mTwGv2K5B6OiTuxV1UkbxRfHlzosVkRTu1nvOKxuwS/6do1spy6F\n" \
"IZXVoS81d2f+4CTAnwDLajUUcL27i/UD+nVhHaR2vs+WQcZ2FrIlXVsoXdlgr+ko\n" \
"nDWRvXqKgJnnMA==\n" \
"-----END CERTIFICATE-----\n";

struct GAME_CONSTANTS {
    int TREASURE_LEVEL1_INITIAL_HP;
    int TREASURE_LEVEL1_ACTION_RECV_WAIT;
    int TREASURE_LEVEL1_RECOVER_DURATION;
    int HTTP_TIMEOUT;
};

class DBConnection {
    private:
        String DATABASE_URL = "https://enitiotreasurehunt.link/";
        String DATABASE_DEBUG_URL = "http://192.168.0.114:5000/";
        bool DEBUG = false;
        String GET_Request(String route) {
            HTTPClient http;
            String url;
            http.setTimeout(HTTP_TIMEOUT);
            if (!DEBUG) {
                WiFiClientSecure https_client;
                https_client.setCACert(root_ca);
                https_client.setInsecure();
                url = DATABASE_URL + route;
                http.begin(https_client, url.c_str());
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
        
        String POST_Request(String route, const char* payload) {
            HTTPClient http;
            http.setTimeout(HTTP_TIMEOUT);
            String url;
            if (!DEBUG) {
                WiFiClientSecure https_client;
                https_client.setCACert(root_ca);
                https_client.setInsecure();
                url = DATABASE_URL + route;
                http.begin(https_client, url.c_str());
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

        GAME_CONSTANTS retrieveGameConstantsFromJSONArray(String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            GAME_CONSTANTS game_const;
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return game_const;
            }
            game_const.TREASURE_LEVEL1_INITIAL_HP = JSON.stringify(json_obj["TREASURE_LEVEL1_INITIAL_HP"]).toInt();
            game_const.TREASURE_LEVEL1_ACTION_RECV_WAIT = JSON.stringify(json_obj["TREASURE_LEVEL1_ACTION_RECV_WAIT"]).toInt();
            game_const.TREASURE_LEVEL1_RECOVER_DURATION = JSON.stringify(json_obj["TREASURE_LEVEL1_RECOVER_DURATION"]).toInt();
            game_const.HTTP_TIMEOUT = JSON.stringify(json_obj["HTTP_TIMEOUT"]).toInt();
            return game_const;
        };
    
    public:
        bool connectToWiFi() {
            // returns True if connected, False if timeout
            // WiFi.begin(HOME_WIFI_SSID, HOME_WIFI_PASSWORD);
            WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);
            
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
            String url = "game_status";
            String jsonArray = GET_Request(url);
            return retrieveParameterFromJSONArray("has_game_started", jsonArray).toInt();
        }

        String setTreasureAsOpened(String treasureName, int og, int participant_id) {
            String url = "treasure/1/" + treasureName + "/" + String(og) + "/" + String(participant_id);
            String jsonArray = GET_Request(url);
            return retrieveParameterFromJSONArray("mac_address", jsonArray);
        };

        bool sendGameStatistics(String treasureName, int alatar, int drachen, int eva, int invicta) {
            String url = "treasure_score";
            String httpRequestData = "{\"treasureName\": " + treasureName + ", \"alatar\": " + String(alatar) + ", \"drachen\": " + String(drachen);
            httpRequestData = httpRequestData + ", \"eva\": " + String(eva) + ", \"invicta\": " + String(invicta);
            httpRequestData = httpRequestData + "}";
            Serial.println(httpRequestData);
            String jsonArray = POST_Request(url, httpRequestData.c_str());
            Serial.println(jsonArray);
            return jsonArray != "{}";
        };

        GAME_CONSTANTS getGameConstants() {
            String url = "get_game_constants";
            String jsonArray = GET_Request(url);
            return retrieveGameConstantsFromJSONArray(jsonArray);
        };

        bool uploadFailedFeedback(String treasureName, int og, int participant_id) {
            String url = "treasurefeedback/" + treasureName + "/" + String(og) + "/" + String(participant_id);
             String jsonArray = GET_Request(url);
//            String httpRequestData = "{\"OG\": " + String(og) + ", \"ID\": " + String(participant_id) + "}";
//            String jsonArray = POST_Request(url.c_str(), httpRequestData.c_str());
            Serial.println(jsonArray);
            return jsonArray != "{}";
        }
};

DBConnection dbc;
