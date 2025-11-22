# Simple ML-like Triage Scoring Model
# This simulates a trained model for the demo

import sys
import random

def calculate_score(age, chest_pain, bp, heart_rate):
    """
    Calculate emergency priority score based on vitals
    Higher score = More critical patient
    """
    score = 0

    # Age factor (elderly patients get higher priority)
    if int(age) > 60:
        score += 25
    elif int(age) > 40:
        score += 15
    else:
        score += 5

    # Chest pain is critical (0 = no pain, 1 = pain)
    if int(chest_pain) == 1:
        score += 35

    # Blood pressure check
    bp_val = int(bp)
    if bp_val > 160 or bp_val < 90:
        score += 25  # Dangerous BP
    elif bp_val > 140:
        score += 15
    else:
        score += 5

    # Heart rate check
    hr_val = int(heart_rate)
    if hr_val > 120 or hr_val < 50:
        score += 20  # Dangerous HR
    elif hr_val > 100:
        score += 10
    else:
        score += 5

    # Add small randomness (simulates ML uncertainty)
    score += random.randint(-3, 3)

    # Keep score between 1-100
    return max(1, min(100, score))

if __name__ == "__main__":
    # Called from C++: python triage.py <age> <chest_pain> <bp> <heart_rate>
    if len(sys.argv) < 5:
        print("50")  # Default score
    else:
        score = calculate_score(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
        print(score)
