#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include <ESP32Servo.h>

// --- CONFIGURATION ---
const char* ssid = "Nessim";
const char* password = "vivelevent";
const char* mqtt_server = "broker.hivemq.com"; 

// Topic d'entrée (Ordres)
const char* topic_listen_action = "maison/plante/action";

// Pins Actionneurs
#define SERVO_PIN 13
#define LED_PIN 12
#define BUZZER_PIN 14

WiFiClient espClient;
PubSubClient client(espClient);
Servo pompeServo;

// --- RÉCEPTION DES ORDRES ---
void on_message(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) message += (char)payload[i];
  
  Serial.print("📩 Ordre Reçu : ");
  Serial.println(message);

  JsonDocument doc;
  deserializeJson(doc, message);
  String method = doc["method"].as<String>();
  bool params = doc["params"].as<bool>();

  // ACTION 1 : POMPE
  if (method == "setPump") {
    if (params) {
      Serial.println("💦 ACTIVATION POMPE !");
      // Alarme Sonore
      tone(BUZZER_PIN, 1000, 200); delay(300);
      tone(BUZZER_PIN, 2000, 400); delay(400);
      
      // Moteur
      pompeServo.write(90);
      delay(3000); 
      pompeServo.write(0);
      Serial.println("🛑 Fin Pompe");
    }
  }

  // ACTION 2 : LUMIÈRE
  if (method == "setLed") {
    if (params) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("💡 LED ON");
    } else {
      digitalWrite(LED_PIN, LOW);
      Serial.println("🌑 LED OFF");
    }
  }
}

void setup_wifi() {
  delay(10);
  Serial.print("Connexion WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("OK");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connexion MQTT (Actionneurs)...");
    String clientId = "ESP32-Actors-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) { 
      Serial.println("Connecté !");
      client.subscribe(topic_listen_action); // S'abonne aux ordres
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pompeServo.attach(SERVO_PIN);
  pompeServo.write(0); 

  // Petit Bip de démarrage pour confirmer que la carte 2 est vivante
  tone(BUZZER_PIN, 3000, 100);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(on_message);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop(); // Écoute permanente
}
