"""
Hospital Triage ML Model Training
=================================
This script trains a Random Forest model on heart disease data
to predict patient urgency/priority scores.

Dataset: UCI Heart Disease Dataset (303 records)
Model: Random Forest Classifier
Output: trained_model.pkl (saved model)

Run this ONCE before using the hospital triage system.
"""

import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score, classification_report
import pickle
import os

def main():
    print("=" * 60)
    print("  HOSPITAL TRIAGE - ML MODEL TRAINING")
    print("=" * 60)
    print()

    # Load training data
    print("[1] Loading training dataset...")
    data_path = os.path.join("datasets", "heart_disease_train.csv")

    if not os.path.exists(data_path):
        print(f"ERROR: Training data not found at {data_path}")
        return

    df = pd.read_csv(data_path)
    print(f"    Loaded {len(df)} patient records")
    print(f"    Features: {list(df.columns)}")
    print()

    # Feature selection (matching our patient data)
    # We use: age, cp (chest_pain), trestbps (bp), thalach (heart_rate)
    print("[2] Preparing features...")
    features = ['age', 'cp', 'trestbps', 'thalach']
    X = df[features]
    y = df['target']  # 0 = no heart disease, 1 = heart disease

    print(f"    Selected features: {features}")
    print(f"    Target distribution:")
    print(f"      - Low risk (0): {(y == 0).sum()} patients")
    print(f"      - High risk (1): {(y == 1).sum()} patients")
    print()

    # Split data
    print("[3] Splitting data (80% train, 20% test)...")
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42, stratify=y
    )
    print(f"    Training set: {len(X_train)} samples")
    print(f"    Test set: {len(X_test)} samples")
    print()

    # Scale features
    print("[4] Scaling features...")
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)
    print("    Features normalized using StandardScaler")
    print()

    # Train model
    print("[5] Training Random Forest model...")
    model = RandomForestClassifier(
        n_estimators=100,
        max_depth=10,
        min_samples_split=5,
        random_state=42,
        n_jobs=-1
    )
    model.fit(X_train_scaled, y_train)
    print("    Model trained successfully!")
    print(f"    - Trees: 100")
    print(f"    - Max depth: 10")
    print()

    # Evaluate
    print("[6] Evaluating model...")
    y_pred = model.predict(X_test_scaled)
    accuracy = accuracy_score(y_test, y_pred)
    print(f"    Accuracy: {accuracy * 100:.2f}%")
    print()
    print("    Classification Report:")
    print("-" * 50)
    print(classification_report(y_test, y_pred, target_names=['Low Risk', 'High Risk']))
    print("-" * 50)
    print()

    # Feature importance
    print("[7] Feature Importance:")
    importance = model.feature_importances_
    for feat, imp in sorted(zip(features, importance), key=lambda x: x[1], reverse=True):
        bar = '#' * int(imp * 50)
        print(f"    {feat:12s}: {imp:.3f} {bar}")
    print()

    # Save model and scaler
    print("[8] Saving model...")
    model_data = {
        'model': model,
        'scaler': scaler,
        'features': features
    }

    with open('trained_model.pkl', 'wb') as f:
        pickle.dump(model_data, f)

    print("    Model saved to: trained_model.pkl")
    print()

    # Test prediction
    print("[9] Test Prediction:")
    test_patient = np.array([[55, 3, 180, 150]])  # age, chest_pain, bp, heart_rate
    test_scaled = scaler.transform(test_patient)
    prob = model.predict_proba(test_scaled)[0]
    priority = int(prob[1] * 100)  # High risk probability as priority

    print(f"    Test Input: age=55, chest_pain=3, bp=180, heart_rate=150")
    print(f"    Low Risk Prob:  {prob[0]*100:.1f}%")
    print(f"    High Risk Prob: {prob[1]*100:.1f}%")
    print(f"    Priority Score: {priority}")
    print()

    print("=" * 60)
    print("  MODEL TRAINING COMPLETE!")
    print("=" * 60)
    print()
    print("You can now run the hospital triage system.")
    print("The ML model will use this trained model for predictions.")
    print()

if __name__ == "__main__":
    main()
