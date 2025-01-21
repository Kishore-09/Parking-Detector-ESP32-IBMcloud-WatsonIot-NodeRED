#include <WiFi.h>
#include <PubSubClient.h>

#define ORG "kotoq5"
#define DEVICE_TYPE "ESP32"
#define DEVICE_ID "12345"
#define TOKEN "12345678"

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char publishTopic[] = "iot-2/evt/Data/fmt/json";
char subscribeTopic[] = "iot-2/cmd/test/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

const int trigPin = 5;
const int echoPin = 18;
#define SOUND_SPEED 0.034

long duration;
float distance;

void callback(char* subscribeTopic, byte* payload, unsigned int payloadLength) {
  String data3;
  for (int i = 0; i < payloadLength; i++) {
    data3 += (char)payload[i];
  }
  Serial.println("Data received: " + data3);
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  wifiConnect();
  mqttConnect();
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * SOUND_SPEED / 2;

  Serial.print("Distance (cm): ");
  Serial.println(distance);

  if (distance < 100) {
    Serial.println("ALERT!!");
    delay(1000);
    publishData(distance);
    delay(1000);
    if (!client.loop()) {
      mqttConnect();
    }
  }
  delay(1000);
}

void publishData(float dist) {
  mqttConnect();
  String payload = "{\"Distance\":" + String(dist) + ",\"Alert\":\"Distance less than 100 cm\"}";
  Serial.print("Sending payload: ");
  Serial.println(payload);

  if (client.publish(publishTopic, (char*)payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish failed");
  }
}

void mqttConnect() {
  if (!client.connected()) {
    while (!client.connect(clientId, authMethod, token)) {
      delay(500);
    }
    initManagedDevice();
  }
}

void wifiConnect() {
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void initManagedDevice() {
  client.subscribe(subscribeTopic);
}
