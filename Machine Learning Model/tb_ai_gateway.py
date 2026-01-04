import time
import json
import random
import joblib
import pandas as pd # Ajouté pour corriger le warning Scikit
import warnings
import paho.mqtt.client as mqtt

# 1. IGNORER LES AVERTISSEMENTS TECHNIQUES (Pour une démo propre)
warnings.filterwarnings("ignore")

# --- CONFIGURATION THINGSBOARD ---
THINGSBOARD_HOST = 'demo.thingsboard.io'
ACCESS_TOKEN = '2vMTm6Ns9ztfmop8PVfQ' # VOTRE TOKEN

# Charger le modèle
print("⏳ Chargement du modèle IA...")
try:
    model = joblib.load('tb_model.pkl')
    print("✅ Modèle chargé !")
except:
    print("❌ Erreur : Lancez train_model.py d'abord")
    exit()

# Connexion MQTT (Callback Version corrigée pour éviter le warning)
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
client.username_pw_set(ACCESS_TOKEN)
client.connect(THINGSBOARD_HOST, 1883, 60)
client.loop_start()

print("🚀 Gateway IA connectée à ThingsBoard. Envoi des prédictions...")

# Noms des colonnes (Identiques au CSV d'entraînement)
feature_names = ['temperature', 'humidity', 'luminosity', 'soil_moisture']

try:
    while True:
        # 1. SIMULATION
        temp = round(random.uniform(15, 35), 1)
        hum = random.randint(20, 80)
        lux = random.randint(0, 4000)
        soil = random.randint(10, 100)

        # 2. PRÉDICTION IA (Correction Warning: On utilise un DataFrame)
        # On crée un petit tableau structuré au lieu d'une liste brute
        features_df = pd.DataFrame([[temp, hum, lux, soil]], columns=feature_names)
        
        prediction = model.predict(features_df)[0]
        proba = model.predict_proba(features_df)[0][prediction] * 100

        status_ia = "CRITIQUE - ARROSER" if prediction == 1 else "Normal"
        
        # 3. PREPARATION
        telemetry = {
            "temperature": temp,
            "humidity": hum,
            "luminosity": lux,
            "soil_moisture": soil,
            "ia_prediction_brute": int(prediction),
            "ia_message": status_ia,
            "ia_confiance": round(proba, 1)
        }

        # 4. ENVOI
        client.publish('v1/devices/me/telemetry', json.dumps(telemetry))
        
        # Affichage avec couleurs pour la console (Vert = Normal, Rouge = Critique)
        if prediction == 1:
            print(f"📡 Envoi TB : T:{temp}°C Eau:{soil}% -> 🚨 IA: {status_ia} ({proba:.0f}%)")
        else:
            print(f"📡 Envoi TB : T:{temp}°C Eau:{soil}% -> ✅ IA: {status_ia} ({proba:.0f}%)")
        
        time.sleep(2)

except KeyboardInterrupt:
    client.loop_stop()
    client.disconnect()
    print("Arrêt.")