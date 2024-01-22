#include "DHT.h"
#include <PubSubClient.h>
#define DHTPIN 16
#define DHTTYPE DHT11

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

DHT dht(DHTPIN, DHTTYPE);

const char *mqtt_server = "broker.hivemq.com";

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Essa mesmo"
#define WIFI_PASSWORD "naoseiapassdanett"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAaL8GP9fTKQwdSXycMKchFHso09Z5yI88"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "humtemp-iot-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup(){
  pinMode(DHTPIN, INPUT);
  dht.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }

  // Configurar o cliente MQTT
  client.setServer(mqtt_server, 1883); // Substitua pela porta do seu broker MQTT


  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "teste4@gmail.com", "testeprojetoiot")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){


  // Leitura dos dados do sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Conectar-se ao broker MQTT
  if (!client.connected()) {
    if (client.connect("ESP32_Emissor")) {
      Serial.println("Conectado ao broker MQTT!");
    } else {
      Serial.println("Falha na conexão com o broker MQTT. Tente novamente em 5 segundos...");
      delay(5000);
      return;
    }
  }

  // Enviar os dados para um tópico MQTT
  char temp[10];
  char hum[10];
  dtostrf(temperature, 6, 2, temp);
  dtostrf(humidity, 6, 2, hum);

  client.publish("sensor/Temperatura", temp);
  client.publish("sensor/Humidade", hum);

  // Aguardar um intervalo antes de realizar a próxima leitura
  delay(5000); // Intervalo de 5 segundos entre as leituras

 delay(1000);
  float h = dht.readHumidity();

  float t = dht.readTemperature();
  
  if (Firebase.ready() && signupOK ) {
    
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/humidity",h)){
//      Serial.println("PASSED");
       Serial.print("Humidity: ");
       Serial.println(h);
      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/temperature", t)){
//      Serial.println("PASSED");
       Serial.print("Temperature: ");
       Serial.println(t);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  Serial.println("______________________________");
}