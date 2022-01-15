#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Mitsubishi.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRMitsubishiAC ac(kIrLed);  // Set the GPIO used for sending messages.

// WIFI
const char* ssid = "Boom";
const char* password = "0969163254";

// Config MQTT Server
#define mqtt_server "27.254.140.105"
#define mqtt_port 1883
#define mqtt_user "boom"
#define mqtt_password "0969163254"

WiFiClient espClient;
PubSubClient client(espClient);

char buf[20];

void setup() {
  ac.begin();
  Serial.begin(115200);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
void loop() {
  // WiFi
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/beacon-project");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
      return;
    }
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  int i = 0;
  while (i < length) msg += (char)payload[i++];

  if (msg == "air-on") {
    ac.on();
    ac.setFan(3);
    ac.setMode(kMitsubishiAcCool);
    ac.setTemp(24);
    ac.setVane(kMitsubishiAcVaneAuto);
    ac.send();
    return;
  } else if (msg == "air-off") {
    ac.off();
    ac.setFan(1);
    ac.setMode(kMitsubishiAcCool);
    ac.setTemp(26);
    ac.setVane(kMitsubishiAcVaneAuto);
    ac.send();
    return;
  }
  
  Serial.println(msg);
}
