#include <Wire.h>
#include <DHT.h>
#include <TridentTD_LineNotify.h>
#include <ArtronShop_SHT45.h>
#include <Arduino.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

// Replace with your network credentials
const char* ssid = "true_home2G_68a";
const char* password = "66fef68a";
const int ledPin = 2;
// REPLACE with your Domain name and URL path or IP address with a path
const char* serverName = "http://smartmushroomfarm.com/post-esp-data-log.php";

ArtronShop_SHT45 sht45(&Wire, 0x44); // SHT45-AD1B => 0x44

float temperature, humidity;

#define RELAY1_PIN 26  
#define RELAY2_PIN 27  

#define LINE_TOKEN "ZKu3jfkQpIZHeV8RnREWuxW24uDV5JXDuXwzi3xK4l8"

// Keep this API Key value to be compatible with the PHP code provided in the project page.
String apiKeyValue = "tPmAT5Ab3j7F9";
String sensorName = "SHT45";
String sensorLocation = "HOUSE1";
String equipment1 = "WATERPUMP";
String equipment2 = "FAN";
String equipment = "NO ACTION NEEDED";
String status1 = "ON";
String status2 = "OFF";
String status = "NO ACTION NEEDED";

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  LINE.setToken(LINE_TOKEN);
  connectToWiFi();
  delay(2000);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);

  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);

  Wire.begin();
  while (!sht45.begin()) {
    Serial.println("SHT45 not found !");
    LINE.notify("HOUSE 1 : SHT45 not found !");
    delay(20000);
  }
  LINE.notify("House 1 : Ready"); 
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    //loop1
    if (sht45.measure()) {
    temperature = sht45.temperature();
    humidity = sht45.humidity();
    
    Serial.print("Temperature: ");
    Serial.print(temperature, 2); 
    Serial.print(" *C\tHumidity: ");
    Serial.print(humidity, 2); 
    Serial.println(" %RH");
    delay(2000);
    if (humidity > 80.99 || temperature > 32.99) {
        // เปิด Relay 2
        Serial.println("HOUSE 1 : relay 2 fan on");
        digitalWrite(RELAY2_PIN, HIGH);

        LINE.notify("HOUSE 1 : Relay 2 (fan) turned on");

        sendSensorDataFANOn(temperature, humidity);

        delay(20000);

        if (sht45.measure()) {
        temperature = sht45.temperature();
        humidity = sht45.humidity();
        
        Serial.print("Temperature: ");
        Serial.print(temperature, 2); 
        Serial.print(" *C\tHumidity: ");
        Serial.print(humidity, 2); 
        Serial.println(" %RH");
        delay(2000);

          // ปิด Relay 2
          digitalWrite(RELAY2_PIN, LOW);
          Serial.println("HOUSE 1 : relay 2 fan off");

          LINE.notify("HOUSE 1 : Relay 2 (fan) turned off");

          sendSensorDataFANOff(temperature, humidity);

          delay(20000);
 
        } else {
          digitalWrite(RELAY2_PIN, LOW);
          Serial.println("HOUSE 1 : relay 2 fan off");
          LINE.notify("HOUSE 1 : Relay 2 (fan) turned off");
          Serial.println("SHT45 read error");
          LINE.notify("HOUSE 1 : Failed to read from SHT45 sensor!");
          LINE.notify("-> How to fix: Close and then open again.");
          delay(20000);
        }
    }
    
    } else {
      Serial.println("SHT45 read error");
      LINE.notify("HOUSE 1 : Failed to read from SHT45 sensor!");
      LINE.notify("-> How to fix: Close and then open again.");
      delay(20000);
    }
    //loop2
    if (sht45.measure()) {
    temperature = sht45.temperature();
    humidity = sht45.humidity();
    
    Serial.print("Temperature: ");
    Serial.print(temperature, 2); 
    Serial.print(" *C\tHumidity: ");
    Serial.print(humidity, 2); 
    Serial.println(" %RH");
    delay(2000);
      if (humidity < 70) {
          // เปิด Relay 1
          Serial.println("HOUSE 1 : relay 1 pump on");
          digitalWrite(RELAY1_PIN, HIGH);

          LINE.notify("HOUSE 1 : Relay 1 (pump) turned on");

          sendSensorDataPumpOn(temperature, humidity);

          delay(10000);

          if (sht45.measure()) {
          temperature = sht45.temperature();
          humidity = sht45.humidity();
          
          Serial.print("Temperature: ");
          Serial.print(temperature, 2); 
          Serial.print(" *C\tHumidity: ");
          Serial.print(humidity, 2); 
          Serial.println(" %RH");
          delay(2000);
          // ปิด Relay 1
            digitalWrite(RELAY1_PIN, LOW);
            Serial.println("HOUSE 1 : relay 1 pump off");

            LINE.notify("HOUSE 1 : Relay 1 (pump) turned off");

            sendSensorDataPumpOff(temperature, humidity);

            delay(20000);
          } else {
            digitalWrite(RELAY1_PIN, LOW);
            Serial.println("HOUSE 1 : relay 1 pump off");
            LINE.notify("HOUSE 1 : Relay 1 (pump) turned off");
            Serial.println("SHT45 read error");
            LINE.notify("HOUSE 1 : Failed to read from SHT45 sensor!");
            LINE.notify("-> How to fix: Close and then open again.");
            delay(20000);
          }
      }

    } else {
      Serial.println("SHT45 read error");
      LINE.notify("HOUSE 1 : Failed to read from SHT45 sensor!");
      LINE.notify("-> How to fix: Close and then open again.");
      delay(20000);
    }

    //loop3 No action needed
    if (sht45.measure()) {
    temperature = sht45.temperature();
    humidity = sht45.humidity();
    
    Serial.print("Temperature: ");
    Serial.print(temperature, 2); 
    Serial.print(" *C\tHumidity: ");
    Serial.print(humidity, 2); 
    Serial.println(" %RH");
    delay(2000);

      if(humidity >= 70 && humidity <= 80.99 && temperature <= 32.99){
          Serial.println("HOUSE 1 : No action needed");

          LINE.notify("HOUSE 1 : No action needed");

          sendSensorDataEquipmentDefault(temperature, humidity);

          delay(20000);
      }
    
    } else {
      Serial.println("SHT45 read error");
      LINE.notify("HOUSE 1 : Failed to read from SHT45 sensor!");
      LINE.notify("-> How to fix: Close and then open again.");
      delay(20000);
    }
    
  } else {
    Serial.println("WiFi Disconnected");
    connectToWiFi(); // Attempt to reconnect to WiFi
  }
}

//////////////////////////////////
void blinkLED(int duration) {
  digitalWrite(ledPin, HIGH);
  delay(duration);
  digitalWrite(ledPin, LOW);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  char message[255];
  sprintf(message, "HOUSE 1 : Connect ... WiFi SSID = %s", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    blinkLED(500);
    delay(1000);
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  LINE.notify(message);
}

void sendSensorDataEquipmentDefault(float temperature, float humidity) {
  String httpRequestData = "api_key=" + apiKeyValue +
                           "&sensor=" + sensorName +
                           "&location=" + sensorLocation +
                           "&value1=" + String(temperature) +
                           "&value2=" + String(humidity) +
                           "&equipment=" + equipment +
                           "&status=" + status;

  Serial.print("httpRequestData: ");
  Serial.println(httpRequestData);

  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  delay(2000); // Add a delay before sending the HTTP request

  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Data Transmission Successful");
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Database Response Unsuccessful");
    LINE.notify(httpResponseCode);
  }

  http.end();
}

void sendSensorDataPumpOn(float temperature, float humidity) {
  String httpRequestData = "api_key=" + apiKeyValue +
                           "&sensor=" + sensorName +
                           "&location=" + sensorLocation +
                           "&value1=" + String(temperature) +
                           "&value2=" + String(humidity) +
                           "&equipment=" + equipment1 +
                           "&status=" + status1;

  Serial.print("httpRequestData: ");
  Serial.println(httpRequestData);

  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  delay(2000); // Add a delay before sending the HTTP request

  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Data Transmission Successful");
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Database Response Unsuccessful");
    LINE.notify(httpResponseCode);
  }

  http.end();
}

void sendSensorDataPumpOff(float temperature, float humidity) {
  String httpRequestData = "api_key=" + apiKeyValue +
                           "&sensor=" + sensorName +
                           "&location=" + sensorLocation +
                           "&value1=" + String(temperature) +
                           "&value2=" + String(humidity) +
                           "&equipment=" + equipment1 +
                           "&status=" + status2;

  Serial.print("httpRequestData: ");
  Serial.println(httpRequestData);

  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  delay(2000); // Add a delay before sending the HTTP request

  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Data Transmission Successful");
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Database Response Unsuccessful");
    LINE.notify(httpResponseCode);
  }

  http.end();
}

void sendSensorDataFANOn(float temperature, float humidity) {
  String httpRequestData = "api_key=" + apiKeyValue +
                           "&sensor=" + sensorName +
                           "&location=" + sensorLocation +
                           "&value1=" + String(temperature) +
                           "&value2=" + String(humidity) +
                           "&equipment=" + equipment2 +
                           "&status=" + status1;

  Serial.print("httpRequestData: ");
  Serial.println(httpRequestData);

  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  delay(2000); // Add a delay before sending the HTTP request

  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Data Transmission Successful");
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Database Response Unsuccessful");
    LINE.notify(httpResponseCode);
  }

  http.end();
}

void sendSensorDataFANOff(float temperature, float humidity) {
  String httpRequestData = "api_key=" + apiKeyValue +
                           "&sensor=" + sensorName +
                           "&location=" + sensorLocation +
                           "&value1=" + String(temperature) +
                           "&value2=" + String(humidity) +
                           "&equipment=" + equipment2 +
                           "&status=" + status2;

  Serial.print("httpRequestData: ");
  Serial.println(httpRequestData);

  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  delay(2000); // Add a delay before sending the HTTP request

  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Data Transmission Successful");
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    LINE.notify("Database Response Unsuccessful");
    LINE.notify(httpResponseCode);
  }

  http.end();
}

