
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ---------------- Wi-Fi SETTINGS ----------------
const char* WIFI_SSID = "WIFI_NAME";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";

// ---------------- THINGSPEAK SETTINGS ----------------
const char* TS_WRITE_KEY = "THINGSPEAK_API_KEY";
const char* TS_SERVER = "https://api.thingspeak.com/update";

// ---------------- GOOGLE SHEETS SETTINGS ----------------
const char* GOOGLE_SCRIPT_URL =
  "https://script.google.com/macros/s/AKfycbwPcdXVfFuBOH_mgp3OEfcNvKgwits-3AUopGPbKt2zEhgubqg91_g4-ibMOceXE0vLIw/exec";

const char* DEVICE_TOKEN = "SWM_ESP32_91Kp7xQ4mN82";

// ---------------- SENSOR PINS ----------------
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;

// ---------------- TANK SETTINGS ----------------
const float TANK_HEIGHT_CM = 9.5;  // Change to your actual tank height
const float LOW_THRESHOLD = 20.0;   // Below 20% = LOW
const float HIGH_THRESHOLD = 80.0;  // 90% and above = HIGH

// Upload interval: 20 seconds
const unsigned long UPLOAD_INTERVAL = 20000;
unsigned long lastUpload = 0;

WiFiClientSecure secureClient;

void connectWiFi() {
  WiFi.disconnect(true, true);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection failed.");
    Serial.print("WiFi status code: ");
    Serial.println(WiFi.status());
  }
}
// ---------------- READ ULTRASONIC SENSOR ----------------
float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration =
    pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1.0;
  }

  float distance = duration * 0.0343 / 2.0;
  return distance;
}

// ---------------- DETERMINE WATER STATUS ----------------
String getStatus(float level) {
  if (level < LOW_THRESHOLD) {
    return "LOW";
  }

  if (level >= HIGH_THRESHOLD) {
    return "HIGH";
  }

  return "NORMAL";
}

// ---------------- URL ENCODING ----------------
String encodeURL(const String& value) {
  const char hex[] = "0123456789ABCDEF";
  String encoded = "";

  for (size_t i = 0; i < value.length(); i++) {
    uint8_t c = (uint8_t)value[i];

    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        c == '-' || c == '_' ||
        c == '.' || c == '~') {
      encoded += (char)c;
    } else {
      encoded += '%';
      encoded += hex[c >> 4];
      encoded += hex[c & 0x0F];
    }
  }

  return encoded;
}

// ---------------- SEND DATA TO THINGSPEAK ----------------
bool sendThingSpeak(float distance,
                    float height,
                    float level) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("ThingSpeak: Wi-Fi unavailable");
    return false;
  }

  HTTPClient http;

  String url = String(TS_SERVER);
  url += "?api_key=" + String(TS_WRITE_KEY);
  url += "&field1=" + String(distance, 2);
  url += "&field2=" + String(height, 2);
  url += "&field3=" + String(level, 2);

  http.setTimeout(10000);

  if (!http.begin(secureClient, url)) {
    Serial.println("ThingSpeak connection failed.");
    return false;
  }

  int httpCode = http.GET();
  String response = http.getString();

  http.end();

  Serial.print("ThingSpeak HTTP code: ");
  Serial.println(httpCode);

  Serial.print("ThingSpeak entry ID: ");
  Serial.println(response);

  return (httpCode == 200 && response.toInt() > 0);
}

// ---------------- SEND DATA TO GOOGLE SHEETS ----------------
bool sendGoogleSheets(float distance,
                      float height,
                      float level,
                      String status) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Google Sheets: Wi-Fi unavailable");
    return false;
  }

  HTTPClient http;

  String url = String(GOOGLE_SCRIPT_URL);
  url += "?token=" + encodeURL(String(DEVICE_TOKEN));
  url += "&distance=" + String(distance, 2);
  url += "&height=" + String(height, 2);
  url += "&level=" + String(level, 2);
  url += "&status=" + encodeURL(status);

  http.setTimeout(15000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  if (!http.begin(secureClient, url)) {
    Serial.println("Google Sheets connection failed.");
    return false;
  }

  int httpCode = http.GET();
  String response = http.getString();

  http.end();

  Serial.print("Google Sheets HTTP code: ");
  Serial.println(httpCode);

  Serial.print("Google Sheets response: ");
  Serial.println(response);

  return (httpCode == 200 &&
          response.indexOf("\"ok\":true") >= 0);
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  // Prototype/testing only; does not verify HTTPS certificates.
  secureClient.setInsecure();

  connectWiFi();

  Serial.println();
  Serial.println("Smart Water Management System");
  Serial.println("Sensor monitoring started.");
}

// ---------------- MAIN LOOP ----------------
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  float distance = readDistanceCm();

  // Reject invalid readings
  if (distance < 0 || distance > TANK_HEIGHT_CM) {
    Serial.println("Invalid sensor reading.");
    Serial.println("Check sensor wiring and tank dimensions.");
    delay(1000);
    return;
  }

  // Calculate water height and percentage
  float height = TANK_HEIGHT_CM - distance;
  float level = (height / TANK_HEIGHT_CM) * 100.0;

  level = constrain(level, 0.0, 100.0);

  String status = getStatus(level);

  // Display measurements
  Serial.println("------------------------------");

  Serial.print("Distance: ");
  Serial.print(distance, 2);
  Serial.println(" cm");

  Serial.print("Water height: ");
  Serial.print(height, 2);
  Serial.println(" cm");

  Serial.print("Water level: ");
  Serial.print(level, 2);
  Serial.println(" %");

  Serial.print("Water status: ");
  Serial.println(status);

  // Upload at the specified interval
  if (millis() - lastUpload >= UPLOAD_INTERVAL) {
    lastUpload = millis();

    Serial.println("Uploading cloud data...");

    bool tsOK = sendThingSpeak(distance, height, level);

    bool sheetsOK =
      sendGoogleSheets(distance, height, level, status);

    Serial.print("ThingSpeak upload: ");
    Serial.println(tsOK ? "SUCCESS" : "FAILED");

    Serial.print("Google Sheets upload: ");
    Serial.println(sheetsOK ? "SUCCESS" : "FAILED");
  }

  delay(1000);
}
