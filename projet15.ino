#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h> 

// --- CONFIGURATION ---
const char* ssid = "Nessim";
const char* password = "vivelevent";
const char* mqtt_server = "broker.hivemq.com"; 

// 🚨 CORRECTION ICI : On envoie directement sur le canal du Dashboard
const char* topic_send = "maison/plante/data"; 

// Pins Capteurs
#define SDA_PIN 21
#define SCL_PIN 22
#define LDR_PIN 34

Adafruit_BME280 bme; 
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0; 

void setup_wifi() {
  delay(10);
  Serial.print("Connexion WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("OK");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connexion MQTT (Capteurs)...");
    // ID Unique pour éviter les conflits
    String clientId = "ESP32-Sensors-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Connecté !");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Démarrage sécurisé du BME280
  if (!bme.begin(0x76, &Wire)) {
     if (!bme.begin(0x77, &Wire)) Serial.println("❌ Erreur BME280");
  }
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop(); 

  unsigned long now = millis();
  
  // Envoi rapide (toutes les 2 secondes) pour voir le Dashboard bouger
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    int ldrRaw = analogRead(LDR_PIN);
    
    // Protection NaN
    if (isnan(temp)) temp = 0;

    JsonDocument doc;
    doc["temperature"] = temp;
    doc["humidity"] = hum;
    doc["luminosity"] = ldrRaw;
    
    // Note : On n'envoie PAS "soil_moisture" car c'est Next.js qui le calcule !

    char jsonBuffer[256];
    serializeJson(doc, jsonBuffer);
    
    client.publish(topic_send, jsonBuffer);
    Serial.print("📡 Envoi Dashboard : ");
    Serial.println(jsonBuffer);
  }
}
