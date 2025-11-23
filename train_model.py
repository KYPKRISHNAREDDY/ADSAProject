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

import csv
import os
import pickle

# Check if sklearn is available
try:
    from sklearn.model_selection import train_test_split
    from sklearn.ensemble import RandomForestClassifier
    from sklearn.preprocessing import StandardScaler
    from sklearn.metrics import accuracy_score
    SKLEARN_AVAILABLE = True
except ImportError:
    SKLEARN_AVAILABLE = False
    print("WARNING: scikit-learn not installed.")
    print("Install with: pip install scikit-learn")
    print()

def load_csv(filepath):
    """Load CSV file without pandas"""
    data = []
    with open(filepath, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            data.append(row)
    return data

def main():
    print("=" * 60)
    print("  HOSPITAL TRIAGE - ML MODEL TRAINING")
    print("=" * 60)
    print()

    if not SKLEARN_AVAILABLE:
        print("ERROR: scikit-learn is required for training.")
        print("Run: pip install scikit-learn")
        return

    # Load training data
    print("[1] Loading training dataset...")
    data_path = os.path.join("datasets", "heart_disease_train.csv")

    if not os.path.exists(data_path):
        print(f"ERROR: Training data not found at {data_path}")
        return

    raw_data = load_csv(data_path)
    print(f"    Loaded {len(raw_data)} patient records")
    print()

    # Extract features and target
    print("[2] Preparing features...")
    features = ['age', 'cp', 'trestbps', 'thalach']
    print(f"    Selected features: {features}")

    X = []
    y = []
    for row in raw_data:
        X.append([
            float(row['age']),
            float(row['cp']),
            float(row['trestbps']),
            float(row['thalach'])
        ])
        y.append(int(row['target']))

    # Count classes
    low_risk = sum(1 for label in y if label == 0)
    high_risk = sum(1 for label in y if label == 1)
    print(f"    Target distribution:")
    print(f"      - Low risk (0): {low_risk} patients")
    print(f"      - High risk (1): {high_risk} patients")
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
    print("    - Trees: 100")
    print("    - Max depth: 10")
    print()

    # Evaluate
    print("[6] Evaluating model...")
    y_pred = model.predict(X_test_scaled)
    accuracy = accuracy_score(y_test, y_pred)
    print(f"    Accuracy: {accuracy * 100:.2f}%")
    print()

    # Simple classification report
    tp = sum(1 for true, pred in zip(y_test, y_pred) if true == 1 and pred == 1)
    tn = sum(1 for true, pred in zip(y_test, y_pred) if true == 0 and pred == 0)
    fp = sum(1 for true, pred in zip(y_test, y_pred) if true == 0 and pred == 1)
    fn = sum(1 for true, pred in zip(y_test, y_pred) if true == 1 and pred == 0)

    print("    Results:")
    print(f"      True Positives:  {tp}")
    print(f"      True Negatives:  {tn}")
    print(f"      False Positives: {fp}")
    print(f"      False Negatives: {fn}")
    print()

    # Feature importance
    print("[7] Feature Importance:")
    importance = model.feature_importances_
    feat_imp = sorted(zip(features, importance), key=lambda x: x[1], reverse=True)
    for feat, imp in feat_imp:
        bar = '#' * int(imp * 40)
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
    test_patient = [[55, 3, 180, 150]]  # age, chest_pain, bp, heart_rate
    test_scaled = scaler.transform(test_patient)
    prob = model.predict_proba(test_scaled)[0]
    priority = int(prob[1] * 100)

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
