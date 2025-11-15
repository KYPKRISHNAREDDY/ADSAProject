#!/usr/bin/env python3
"""
ML-based Triage Scoring System for Hospital Emergency Department
Uses patient medical parameters to predict urgency score (0-100)
"""

import sys
import numpy as np
from sklearn.ensemble import RandomForestRegressor
import warnings
warnings.filterwarnings('ignore')

class TriageMLModel:
    def __init__(self):
        """Initialize the ML model with pre-trained weights"""
        # Create a Random Forest model with pre-configured parameters
        self.model = RandomForestRegressor(
            n_estimators=50,
            max_depth=10,
            random_state=42
        )

        # Train with synthetic medical data to simulate real triage scenarios
        self._train_model()

    def _train_model(self):
        """Train the model on synthetic medical emergency data"""
        # Features: age, gender(0/1), chest_pain(0-4), diabetes(0/1), bp, cholesterol,
        #           bmi, max_heart_rate, hypertension(0/1)

        # Synthetic training data representing various emergency scenarios
        X_train = np.array([
            # Critical cases (score: 90-100)
            [65, 1, 4, 1, 180, 250, 32.5, 180, 1],  # Severe chest pain, high BP
            [70, 0, 4, 1, 190, 270, 35.0, 175, 1],  # Critical cardiac symptoms
            [55, 1, 4, 0, 175, 240, 30.0, 185, 1],  # Acute chest pain
            [80, 1, 3, 1, 185, 260, 33.0, 170, 1],  # Elderly with severe symptoms
            [60, 0, 4, 1, 195, 280, 36.0, 165, 1],  # Multiple risk factors

            # High priority (score: 70-89)
            [50, 1, 3, 1, 160, 220, 29.0, 150, 1],  # Moderate cardiac risk
            [45, 0, 3, 0, 165, 230, 28.5, 155, 1],  # Chest discomfort
            [68, 1, 2, 1, 155, 210, 31.0, 145, 1],  # Elderly with conditions
            [52, 1, 3, 1, 170, 235, 30.5, 160, 0],  # Diabetic with chest pain
            [58, 0, 2, 1, 168, 225, 29.5, 150, 1],  # Multiple comorbidities

            # Medium priority (score: 40-69)
            [35, 1, 2, 0, 130, 180, 26.0, 140, 0],  # Mild symptoms, young
            [42, 0, 1, 0, 135, 185, 25.5, 145, 0],  # Minor chest discomfort
            [55, 1, 1, 1, 140, 200, 27.0, 135, 0],  # Diabetic, mild symptoms
            [48, 0, 2, 0, 145, 190, 26.5, 142, 0],  # Moderate assessment
            [40, 1, 1, 0, 138, 175, 24.5, 148, 0],  # Low-moderate risk

            # Low priority (score: 20-39)
            [25, 1, 0, 0, 120, 150, 23.0, 130, 0],  # Young, no major symptoms
            [30, 0, 1, 0, 125, 160, 22.5, 135, 0],  # Minimal symptoms
            [28, 1, 0, 0, 118, 145, 21.0, 128, 0],  # Routine checkup level
            [32, 0, 0, 0, 122, 155, 23.5, 132, 0],  # Very low risk
            [27, 1, 1, 0, 115, 150, 22.0, 125, 0],  # Minor concern

            # Additional training samples for robustness
            [75, 1, 4, 1, 200, 290, 37.0, 190, 1],  # Critical
            [63, 0, 3, 1, 172, 245, 31.5, 168, 1],  # High
            [38, 1, 1, 0, 128, 170, 24.0, 138, 0],  # Medium
            [22, 0, 0, 0, 110, 140, 20.5, 120, 0],  # Low
        ])

        # Target scores (urgency/priority)
        y_train = np.array([
            95, 98, 92, 90, 96,  # Critical
            85, 82, 78, 80, 75,  # High
            65, 58, 62, 55, 60,  # Medium
            35, 38, 32, 28, 30,  # Low
            97, 83, 52, 25       # Additional
        ])

        # Train the model
        self.model.fit(X_train, y_train)

    def predict_triage_score(self, age, gender, chest_pain, diabetes, bp,
                            cholesterol, bmi, max_heart_rate, hypertension):
        """
        Predict triage urgency score based on patient parameters

        Parameters:
        -----------
        age: int (0-120)
        gender: int (0=female, 1=male)
        chest_pain: int (0=none, 1=mild, 2=moderate, 3=severe, 4=crushing)
        diabetes: int (0=no, 1=yes)
        bp: int (systolic blood pressure, e.g., 120)
        cholesterol: int (total cholesterol, e.g., 200)
        bmi: float (body mass index, e.g., 25.5)
        max_heart_rate: int (maximum heart rate, e.g., 150)
        hypertension: int (0=no, 1=yes)

        Returns:
        --------
        float: Triage score (0-100, higher = more urgent)
        """

        # Prepare input features
        features = np.array([[age, gender, chest_pain, diabetes, bp,
                            cholesterol, bmi, max_heart_rate, hypertension]])

        # Get base prediction
        score = self.model.predict(features)[0]

        # Apply rule-based adjustments for critical scenarios
        # This ensures medical logic is preserved

        # Critical chest pain overrides
        if chest_pain >= 4:
            score = max(score, 90)
        elif chest_pain >= 3:
            score = max(score, 75)

        # Extreme vital signs
        if bp >= 180 or bp <= 90:
            score = max(score, 85)

        if max_heart_rate >= 180 or max_heart_rate <= 50:
            score = max(score, 80)

        # Age-based risk adjustment
        if age >= 70 and (diabetes or hypertension):
            score *= 1.1
        elif age >= 80:
            score *= 1.15

        # Multiple comorbidities
        comorbidity_count = diabetes + hypertension
        if comorbidity_count >= 2 and chest_pain >= 2:
            score *= 1.12

        # Ensure score is within valid range
        score = max(0, min(100, score))

        return round(score, 2)

def main():
    """
    CLI interface for C++ integration
    Usage: python triage_ml_model.py <age> <gender> <chest_pain> <diabetes>
           <bp> <cholesterol> <bmi> <max_heart_rate> <hypertension>
    """
    if len(sys.argv) != 10:
        print("ERROR: Invalid number of arguments", file=sys.stderr)
        print("Usage: triage_ml_model.py <age> <gender> <chest_pain> <diabetes> "
              "<bp> <cholesterol> <bmi> <max_heart_rate> <hypertension>", file=sys.stderr)
        sys.exit(1)

    try:
        # Parse command line arguments
        age = int(sys.argv[1])
        gender = int(sys.argv[2])
        chest_pain = int(sys.argv[3])
        diabetes = int(sys.argv[4])
        bp = int(sys.argv[5])
        cholesterol = int(sys.argv[6])
        bmi = float(sys.argv[7])
        max_heart_rate = int(sys.argv[8])
        hypertension = int(sys.argv[9])

        # Create model and predict
        model = TriageMLModel()
        score = model.predict_triage_score(
            age, gender, chest_pain, diabetes, bp,
            cholesterol, bmi, max_heart_rate, hypertension
        )

        # Output only the score (for easy parsing by C++)
        print(score)

    except ValueError as e:
        print(f"ERROR: Invalid parameter format - {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"ERROR: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
