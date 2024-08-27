#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TridentTD_LineNotify.h>

const char* ssid = "true_home2G_68a";
const char* password = "66fef68a";
const char* host = "www.smartmushroomfarm.com";
String url2 = "/button1_proc.php";
String url = url2;
String id = "button1_stat";
String value = "";

#define RELAY1_PIN D6 // เปลี่ยนจาก D4 เป็น D2 (GPIO4)
#define LINE_TOKEN "ZKu3jfkQpIZHeV8RnREWuxW24uDV5JXDuXwzi3xK4l8"

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long previousReconnectAttempt = 0;
const long reconnectInterval = 30000; // 30 วินาที

void setup() {
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  delay(1000);
  pinMode(RELAY1_PIN, OUTPUT);

  LINE.setToken(LINE_TOKEN);
  connectToWiFi();
  LINE.notify("House Control : Ready");
}

String GetField(String str, int fcount) {
  int i = 0;
  char ch;
  int len = str.length();
  String str2 = "";
  while (fcount >= 1 && i < len) {
    ch = str[i];
    if (ch != ',' && ch != ' ') {
      str2 += ch;
    } else {
      fcount--;
      if (fcount > 0) {
        str2 = "";
      }
    }
    i++;
  }
  return (str2);
}

String stat = "OFF";
String line = "";
String line2 = "";

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected");
    url = url2;

    if (stat == "ON") {
      digitalWrite(RELAY1_PIN, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Status House 1");
      lcd.setCursor(0, 1);
      lcd.print("ON");
    } else if (stat == "OFF") {
      digitalWrite(RELAY1_PIN, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Status House 1");
      lcd.setCursor(0, 1);
      lcd.print("OFF");
    }
    delay(100);
    Serial.println(stat);

    value = "?";

    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }

    url += "?id=" + id;
    url += "&value=" + value;

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    url = url2;
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
      yield(); // Refresh watchdog timer
    }

    while (client.available()) {
      line = client.readStringUntil('\n');
      if (char(line[0]) == '>') {
        line.trim();
        line2 = line;
      }
    }
    stat = GetField(line2, 3);
  } else {
    Serial.println("WiFi Disconnected");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Status WiFi");
    lcd.setCursor(0, 1);
    lcd.print("WiFiDisconnected");
    unsigned long currentMillis = millis();
    if (currentMillis - previousReconnectAttempt >= reconnectInterval) {
      previousReconnectAttempt = currentMillis;
      reconnectWiFi();
    }
  }
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Status WiFi");
  lcd.setCursor(0, 1);
  lcd.print("Connecting WiFi");
  char message[255];
  sprintf(message, "House Control: Connect ... WiFi SSID = %s", ssid);
  WiFi.begin(ssid, password);

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 30) {
    Serial.print(".");
    lcd.setCursor(0, 1);
    lcd.print(".");
    delay(500);
    attempt++;
    yield(); // Refresh watchdog timer
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    LINE.notify(message);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Status WiFi");
    lcd.setCursor(0, 1);
    lcd.print("Connected WiFi");
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Status WiFi");
    lcd.setCursor(0, 1);
    lcd.print("Failed to connect");
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please reboot");
  }
}

void reconnectWiFi() {
  Serial.println("Reconnecting to WiFi...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Status WiFi");
  lcd.setCursor(0, 1);
  lcd.print("Reconnecting...");
  WiFi.disconnect();
  delay(1000);
  connectToWiFi();
}
