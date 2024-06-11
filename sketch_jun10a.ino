/* ESP32 WiFi Scanning example */

#include "DHT.h"
#include "Arduino.h"
#include "SSD1306Wire.h"
#include <WiFi.h> 
#include <ThingsBoard.h> 

// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// WiFi access point
#define WIFI_AP_NAME        "DellMizi"
// WiFi password
#define WIFI_PASSWORD       "6203001138"

// See https://thingsboard.io/docs/getting-started-guides/helloworld/ 
// to understand how to obtain an access token
#define TOKEN               "AunIt2oOZFDV7gJTRb6J"
// ThingsBoard server instance.
#define THINGSBOARD_SERVER  "demo.thingsboard.io"

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD    115200

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;


SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);
DHT dht(13, DHT11);
float t;
float u;
int counter = 1;
void VextOFF(void)  //Vext default OFF
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}
void VextON(void) {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}
void displayReset(void) {
  // Send a reset
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, HIGH);
  delay(1);
  digitalWrite(RST_OLED, LOW);
  delay(1);
  digitalWrite(RST_OLED, HIGH);
  delay(1);
}
void setup() {

  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  InitWiFi();

  dht.begin();
  Serial.begin(9600);

    Serial.println();
  Serial.println();

  // This turns on and resets the OLED on the Heltec boards
  VextON();
  displayReset();

  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}

void loop() {

    // Reconnect to WiFi, if needed
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
    return;
  }
  
    if (!tb.connected()) {
    subscribed = false;

    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }
  if (!subscribed) {
    Serial.println("Subscribing for RPC...");

    // Perform a subscription. All consequent data processing will happen in
    // callbacks as denoted by callbacks[] array.
    if (!tb.RPC_Subscribe(callbacks, COUNT_OF(callbacks))) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }

    Serial.println("Subscribe done");
    subscribed = true;
  }


  u = dht.readHumidity();
  t = dht.readTemperature();

  Serial.print("Temperatura:");
  Serial.print(t);
  Serial.print("   - umidade:"); 
  Serial.print(u);
  Serial.println();
  display.clear();

//display
  drawFontFaceDemo(u,t);
  // write the buffer to the display
  display.display();

//display
  counter++;

  if (send_passed > send_delay) {
    Serial.println("Sending data...");

    // Uploads new telemetry to ThingsBoard using MQTT. 
    // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api 
    // for more details


      tb.sendTelemetryFloat("temperature", t);
      tb.sendTelemetryFloat("humidity", h);

    send_passed = 0;
  }

 tb.loop();

  delay(1000);
}
void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}
void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}
void drawFontFaceDemo(float u, float t) {
  // Font Demo1
  // create more fonts at http://oleddisplay.squix.ch/
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 10, "Temperatura: " + String(t));
  display.drawString(0, 26, "\nHumidade: " +String(u));
}
