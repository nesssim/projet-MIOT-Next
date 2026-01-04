import pandas as pd
from sklearn.tree import DecisionTreeClassifier
import joblib

# 1. Charger
data = pd.read_csv('dataset_large.csv')
X = data[['temperature', 'humidity', 'luminosity', 'soil_moisture']]
y = data['needs_water']

# 2. Entraîner
model = DecisionTreeClassifier(max_depth=5)
model.fit(X, y)

# 3. Sauvegarder
joblib.dump(model, 'tb_model.pkl')
print("✅ Modèle IA généré : tb_model.pkl")