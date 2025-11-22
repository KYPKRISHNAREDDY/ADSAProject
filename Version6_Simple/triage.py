"""
Hospital Triage ML Prediction
=============================
This script loads the trained Random Forest model and predicts priority scores.

Usage: python triage.py <age> <chest_pain> <bp> <heart_rate>
Output: Priority score (0-100)

The model must be trained first using: python train_model.py
"""

import sys
import pickle
import numpy as np
import os

def predict_priority(age, chest_pain, bp, heart_rate):
    """
    Predict patient priority using trained ML model.

    Parameters:
    - age: Patient age (years)
    - chest_pain: Chest pain type (0-4, higher = worse)
    - bp: Blood pressure (mmHg)
    - heart_rate: Heart rate (bpm)

    Returns:
    - Priority score (0-100, higher = more urgent)
    """

    model_path = 'trained_model.pkl'

    # Check if model exists
    if not os.path.exists(model_path):
        # Fallback to rule-based scoring if model not trained
        return fallback_scoring(age, chest_pain, bp, heart_rate)

    try:
        # Load trained model
        with open(model_path, 'rb') as f:
            model_data = pickle.load(f)

        model = model_data['model']
        scaler = model_data['scaler']

        # Prepare input
        patient_data = np.array([[age, chest_pain, bp, heart_rate]])
        patient_scaled = scaler.transform(patient_data)

        # Get probability of high risk
        prob = model.predict_proba(patient_scaled)[0]
        high_risk_prob = prob[1]  # Probability of class 1 (high risk)

        # Convert to priority score (0-100)
        # Base score from ML model probability
        base_score = high_risk_prob * 60  # Base from ML (0-60)

        # Additional factors for fine-tuning (0-40)
        age_factor = min(age / 100 * 15, 15)  # Max 15 points
        bp_factor = min(max(bp - 120, 0) / 80 * 12, 12)  # Max 12 points
        hr_factor = min(max(heart_rate - 80, 0) / 100 * 8, 8)  # Max 8 points
        cp_factor = chest_pain * 1.5  # Max ~6 points

        priority = int(base_score + age_factor + bp_factor + hr_factor + cp_factor)

        # Ensure range 25-100
        priority = max(25, min(100, priority))

        return priority

    except Exception as e:
        # Fallback if model loading fails
        return fallback_scoring(age, chest_pain, bp, heart_rate)


def fallback_scoring(age, chest_pain, bp, heart_rate):
    """
    Rule-based scoring (used if ML model not available).
    This is a backup in case trained_model.pkl doesn't exist.
    """
    score = 30  # Base score

    # Age factor
    if age > 60:
        score += 20
    elif age > 45:
        score += 12
    else:
        score += 5

    # Chest pain factor (0-4 scale)
    score += chest_pain * 8

    # Blood pressure factor
    if bp > 180:
        score += 20
    elif bp > 140:
        score += 12
    elif bp < 90:
        score += 15

    # Heart rate factor
    if heart_rate > 150:
        score += 15
    elif heart_rate > 100:
        score += 8
    elif heart_rate < 50:
        score += 12

    return max(25, min(100, score))


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: python triage.py <age> <chest_pain> <bp> <heart_rate>")
        print("50")  # Default score
        sys.exit(1)

    age = int(sys.argv[1])
    chest_pain = int(sys.argv[2])
    bp = int(sys.argv[3])
    heart_rate = int(sys.argv[4])

    priority = predict_priority(age, chest_pain, bp, heart_rate)
    print(priority)
