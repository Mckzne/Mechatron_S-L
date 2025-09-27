#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 4  // 4 tiles per ESP32
#define DATA_PIN 18

Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

// WiFi credentials
const char* ssid = "RPI_AP";
const char* password = "raspberry";
const char* mqtt_server = "192.168.4.1";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Parse JSON and set LEDs
  // {"tiles":[0,1],"color":[255,0,0],"animation":"movement","duration":1000}
  // Example: use ArduinoJson to parse
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Tile")) {
      client.subscribe("esp/01/commands");
    } else { delay(5000); }
  }
}

void setup() {
  strip.begin();
  strip.show();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
