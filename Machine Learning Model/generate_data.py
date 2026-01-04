import csv
import random

# Nom du fichier de sortie
filename = "dataset_large.csv"
lignes_a_generer = 1000  # On génère 1000 lignes pour être précis

print(f"🔄 Génération de {lignes_a_generer} lignes de données simulées...")

with open(filename, mode='w', newline='') as file:
    writer = csv.writer(file)
    # 1. Écriture de l'en-tête (Même format que votre ancien fichier)
    writer.writerow(['temperature', 'humidity', 'luminosity', 'soil_moisture', 'needs_water'])

    count_arrosage = 0

    for i in range(lignes_a_generer):
        # 2. Génération de valeurs aléatoires réalistes
        temp = round(random.uniform(10, 42), 1)  # Température entre 10 et 42°C
        hum = random.randint(10, 95)             # Humidité air entre 10 et 95%
        lux = random.randint(0, 5000)            # Lumière (0=Nuit, 5000=Plein soleil)
        soil = random.randint(0, 100)            # Humidité sol

        # 3. La "VRAIE" Logique Agronomique (L'étiquetage)
        # C'est ici qu'on apprend à l'IA quand arroser.
        needs_water = 0

        # Règle A : Sol critique (Quel que soit le temps)
        if soil < 20:
            needs_water = 1
        
        # Règle B : Sol stressé + Canicule (>30°C)
        elif soil < 40 and temp > 30:
            needs_water = 1
            
        # Règle C : Sol moyen + Air très sec (<30%) + Beaucoup de lumière
        elif soil < 50 and hum < 30 and lux > 2000:
            needs_water = 1

        if needs_water == 1:
            count_arrosage += 1

        # 4. Écriture dans le CSV
        writer.writerow([temp, hum, lux, soil, needs_water])

print(f"✅ Fichier '{filename}' créé avec succès !")
print(f"📊 Statistiques : {count_arrosage} cas nécessitent un arrosage sur {lignes_a_generer} ({(count_arrosage/lignes_a_generer)*100:.1f}%).")