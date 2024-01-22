#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "Essa mesmo";
const char *password = "naoseiapassdanett";
const char *mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Conectar-se à rede WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }

  // Configurar o cliente MQTT
  client.setServer(mqtt_server, 1883); // Substitua pela porta do seu broker MQTT
  client.setCallback(callback);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Tópico: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT...");
    if (client.connect("ESP32_Receptor")) {
      Serial.println("Conectado ao broker MQTT!");
      client.subscribe("sensor/Temperatura");
      client.subscribe("sensor/Humidade");
    } else {
      Serial.println("Falha na conexão com o broker MQTT. Tente novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
