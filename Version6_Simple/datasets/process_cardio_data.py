"""
Process Cardiovascular Disease Dataset (70,000 records)
Converts raw data to format compatible with our ML training pipeline
"""

import csv
import random

# First names pool for generating realistic names
first_names = [
    "James", "Mary", "John", "Patricia", "Robert", "Jennifer", "Michael", "Linda",
    "William", "Elizabeth", "David", "Barbara", "Richard", "Susan", "Joseph", "Jessica",
    "Thomas", "Sarah", "Charles", "Karen", "Christopher", "Nancy", "Daniel", "Lisa",
    "Matthew", "Betty", "Anthony", "Margaret", "Mark", "Sandra", "Donald", "Ashley",
    "Steven", "Kimberly", "Paul", "Emily", "Andrew", "Donna", "Joshua", "Michelle",
    "Kenneth", "Dorothy", "Kevin", "Carol", "Brian", "Amanda", "George", "Melissa",
    "Edward", "Deborah", "Ronald", "Stephanie", "Timothy", "Rebecca", "Jason", "Sharon",
    "Jeffrey", "Laura", "Ryan", "Cynthia", "Jacob", "Kathleen", "Gary", "Amy",
    "Nicholas", "Angela", "Eric", "Shirley", "Jonathan", "Anna", "Stephen", "Brenda",
    "Larry", "Pamela", "Justin", "Emma", "Scott", "Nicole", "Brandon", "Helen",
    "Benjamin", "Samantha", "Samuel", "Katherine", "Raymond", "Christine", "Gregory", "Debra",
    "Frank", "Rachel", "Alexander", "Carolyn", "Patrick", "Janet", "Raymond", "Catherine"
]

last_names = [
    "Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", "Davis",
    "Rodriguez", "Martinez", "Hernandez", "Lopez", "Gonzalez", "Wilson", "Anderson", "Thomas",
    "Taylor", "Moore", "Jackson", "Martin", "Lee", "Perez", "Thompson", "White",
    "Harris", "Sanchez", "Clark", "Ramirez", "Lewis", "Robinson", "Walker", "Young",
    "Allen", "King", "Wright", "Scott", "Torres", "Nguyen", "Hill", "Flores",
    "Green", "Adams", "Nelson", "Baker", "Hall", "Rivera", "Campbell", "Mitchell",
    "Carter", "Roberts", "Gomez", "Phillips", "Evans", "Turner", "Diaz", "Parker",
    "Cruz", "Edwards", "Collins", "Reyes", "Stewart", "Morris", "Morales", "Murphy",
    "Cook", "Rogers", "Gutierrez", "Ortiz", "Morgan", "Cooper", "Peterson", "Bailey",
    "Reed", "Kelly", "Howard", "Ramos", "Kim", "Cox", "Ward", "Richardson"
]

def generate_name():
    return f"{random.choice(first_names)} {random.choice(last_names)}"

def days_to_years(days):
    return int(days / 365)

def estimate_chest_pain(cholesterol, gluc, ap_hi, cardio):
    """Estimate chest pain type (0-4) based on available features"""
    score = 0
    if cholesterol >= 2:
        score += 1
    if cholesterol == 3:
        score += 1
    if gluc >= 2:
        score += 1
    if ap_hi > 140:
        score += 1
    if cardio == 1:
        score += random.randint(0, 1)
    return min(4, score)

def estimate_heart_rate(age_years, active, cardio, ap_hi):
    """Estimate heart rate based on available features"""
    # Base heart rate
    base_hr = 75

    # Age adjustment (older = slightly lower resting HR variation)
    age_factor = random.randint(-10, 15)

    # Activity level adjustment
    if active == 1:
        activity_adj = random.randint(-5, 5)
    else:
        activity_adj = random.randint(0, 10)

    # Cardiovascular disease adjustment
    if cardio == 1:
        cardio_adj = random.randint(5, 25)
    else:
        cardio_adj = random.randint(-5, 10)

    # BP correlation
    bp_adj = int((ap_hi - 120) / 10) * random.randint(0, 3)

    hr = base_hr + age_factor + activity_adj + cardio_adj + bp_adj
    return max(55, min(195, hr))

def process_data():
    input_file = 'cardio_raw.csv'
    output_file = 'heart_disease_train.csv'

    processed = []

    with open(input_file, 'r') as f:
        # Read with semicolon delimiter
        reader = csv.DictReader(f, delimiter=';')

        for i, row in enumerate(reader):
            # Convert age from days to years
            age = days_to_years(int(row['age']))

            # Skip invalid age ranges
            if age < 20 or age > 90:
                continue

            # Get features
            ap_hi = int(row['ap_hi'])
            ap_lo = int(row['ap_lo'])
            cholesterol = int(row['cholesterol'])
            gluc = int(row['gluc'])
            active = int(row['active'])
            cardio = int(row['cardio'])

            # Skip invalid blood pressure values
            if ap_hi < 70 or ap_hi > 250 or ap_lo < 40 or ap_lo > 200:
                continue

            # Generate derived features
            chest_pain = estimate_chest_pain(cholesterol, gluc, ap_hi, cardio)
            heart_rate = estimate_heart_rate(age, active, cardio, ap_hi)

            # Create record with patient name
            record = {
                'patient_id': i + 1,
                'name': generate_name(),
                'age': age,
                'sex': int(row['gender']) - 1,  # Convert 1,2 to 0,1
                'cp': chest_pain,
                'trestbps': ap_hi,
                'chol': cholesterol * 80 + random.randint(100, 150),  # Scale cholesterol
                'fbs': 1 if gluc > 1 else 0,
                'restecg': random.randint(0, 2),
                'thalach': heart_rate,
                'exang': 1 if cardio == 1 and random.random() > 0.5 else 0,
                'oldpeak': round(random.uniform(0, 4) if cardio else random.uniform(0, 2), 1),
                'slope': random.randint(0, 2),
                'ca': random.randint(0, 3) if cardio else random.randint(0, 1),
                'thal': random.randint(1, 3),
                'target': cardio
            }

            processed.append(record)

    # Shuffle and take first 10000 records for reasonable file size
    random.shuffle(processed)
    processed = processed[:10000]

    # Write output
    with open(output_file, 'w', newline='') as f:
        fieldnames = ['patient_id', 'name', 'age', 'sex', 'cp', 'trestbps', 'chol',
                      'fbs', 'restecg', 'thalach', 'exang', 'oldpeak', 'slope', 'ca', 'thal', 'target']
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(processed)

    print(f"Processed {len(processed)} records")
    print(f"Output saved to: {output_file}")

    # Show sample
    print("\nSample records:")
    for rec in processed[:5]:
        print(f"  {rec['name']}: age={rec['age']}, cp={rec['cp']}, bp={rec['trestbps']}, hr={rec['thalach']}, target={rec['target']}")

if __name__ == "__main__":
    process_data()
