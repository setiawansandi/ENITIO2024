#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <HTTPClient.h>
#include <Arduino_JSON.h>

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

struct GAME_CONSTANTS {
  int REVIVAL_STATION_INITIAL_HP;
  int REVIVAL_STATION_ACTION_RECV_WAIT;
  int REVIVAL_STATION_RECOVER_DURATION;
  int REVIVAL_STATION_MULTIPLIER;
  int HTTP_TIMEOUT;
};

class DBConnection {
  private:
    String DATABASE_URL = "https://enitiotreasurehunt.link/";
    String DATABASE_DEBUG_URL = "http://enitiotreasurehunt.link/";
    bool DEBUG = true;  // HTTPS not supported yet
    String GET_Request(String route) {
      HTTPClient http;
      String url;
      http.setTimeout(HTTP_TIMEOUT);
      if (!DEBUG) {
        url = DATABASE_URL + route;
      } else {
        url = DATABASE_DEBUG_URL + route;
        Serial.print("DEBUG GET "); Serial.println(url);
      }
      http.begin(url.c_str());

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
        url = DATABASE_URL + route;
      } else {
        url = DATABASE_DEBUG_URL + route;
        Serial.print("DEBUG POST "); Serial.println(url);
      }
      
      http.begin(url.c_str());
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
      game_const.REVIVAL_STATION_INITIAL_HP = JSON.stringify(json_obj["REVIVAL_STATION_INITIAL_HP"]).toInt();
      game_const.REVIVAL_STATION_ACTION_RECV_WAIT = JSON.stringify(json_obj["REVIVAL_STATION_ACTION_RECV_WAIT"]).toInt();
      game_const.REVIVAL_STATION_RECOVER_DURATION = JSON.stringify(json_obj["REVIVAL_STATION_RECOVER_DURATION"]).toInt();
      game_const.REVIVAL_STATION_MULTIPLIER = JSON.stringify(json_obj["REVIVAL_MULTIPLIER"]).toInt();
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

    GAME_CONSTANTS getGameConstants() {
      String url = "get_game_constants";
      String jsonArray = GET_Request(url.c_str());
      return retrieveGameConstantsFromJSONArray(jsonArray);
    };
};

DBConnection dbc;
