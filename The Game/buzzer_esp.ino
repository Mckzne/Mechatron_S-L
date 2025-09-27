#include <WiFi.h>
#include <PubSubClient.h>

#define BUZZER_PIN 4    // GPIO pin connected to team buzzer
#define DEBOUNCE_DELAY 200

const char* ssid = "RPI_AP";        // Connect to Pi hotspot
const char* password = "raspberry";
const char* mqtt_server = "192.168.4.1";

const char* team_name = "blue";     // Set this ESP for the team's color/name

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPressTime = 0;

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Optional: receive "disable buzzer" command
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];
  if(msg == "disable") {
    // Could light an LED to show buzzer disabled
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(team_name)) {
      client.subscribe(String("buzzer/")+team_name+"/control");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUZZER_PIN, INPUT_PULLUP);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  int buzzerState = digitalRead(BUZZER_PIN);
  if (buzzerState == LOW) { // Button pressed
    unsigned long now = millis();
    if(now - lastPressTime > DEBOUNCE_DELAY){
      lastPressTime = now;
      // Send MQTT message
      String topic = "buzzer/pressed";
      String payload = team_name;
      client.publish(topic.c_str(), payload.c_str());
    }
  }
}
