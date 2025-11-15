#!/usr/bin/env python3
"""
Create Interactive HTML Dashboard for Hospital Triage System
Reads dashboard_data.json and generates interactive HTML page
"""

import json
import os

HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hospital Triage System - Interactive Dashboard</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #333;
            padding: 20px;
        }

        .container {
            max-width: 1400px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }

        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }

        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
        }

        .header p {
            font-size: 1.2em;
            opacity: 0.9;
        }

        .controls {
            background: #f8f9fa;
            padding: 20px;
            display: flex;
            justify-content: center;
            gap: 15px;
            flex-wrap: wrap;
            border-bottom: 2px solid #ddd;
        }

        button {
            padding: 12px 30px;
            font-size: 16px;
            font-weight: bold;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.3s;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }

        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 12px rgba(0,0,0,0.15);
        }

        .btn-primary {
            background: #4CAF50;
            color: white;
        }

        .btn-primary:hover {
            background: #45a049;
        }

        .btn-secondary {
            background: #2196F3;
            color: white;
        }

        .btn-secondary:hover {
            background: #0b7dda;
        }

        .btn-danger {
            background: #f44336;
            color: white;
        }

        .btn-danger:hover {
            background: #da190b;
        }

        .btn-info {
            background: #FF9800;
            color: white;
        }

        .btn-info:hover {
            background: #e68900;
        }

        .iteration-info {
            text-align: center;
            padding: 15px;
            background: #e3f2fd;
            font-size: 1.3em;
            font-weight: bold;
            color: #1976d2;
        }

        .main-content {
            padding: 30px;
        }

        .section {
            margin-bottom: 30px;
        }

        .section-title {
            font-size: 1.8em;
            color: #667eea;
            margin-bottom: 15px;
            padding-bottom: 10px;
            border-bottom: 3px solid #667eea;
        }

        .doctors-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-top: 20px;
        }

        .doctor-card {
            background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);
            border-radius: 12px;
            padding: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
            transition: transform 0.3s;
        }

        .doctor-card:hover {
            transform: translateY(-5px);
        }

        .doctor-card.busy {
            background: linear-gradient(135deg, #ffeaa7 0%, #fab1a0 100%);
            border-left: 5px solid #d63031;
        }

        .doctor-card.available {
            background: linear-gradient(135deg, #b8e994 0%, #78e08f 100%);
            border-left: 5px solid #27ae60;
        }

        .doctor-name {
            font-size: 1.3em;
            font-weight: bold;
            margin-bottom: 10px;
            color: #2c3e50;
        }

        .doctor-status {
            display: inline-block;
            padding: 5px 15px;
            border-radius: 20px;
            font-weight: bold;
            font-size: 0.9em;
            margin-bottom: 10px;
        }

        .status-busy {
            background: #e74c3c;
            color: white;
        }

        .status-available {
            background: #27ae60;
            color: white;
        }

        .patient-info {
            margin-top: 10px;
            padding: 10px;
            background: rgba(255,255,255,0.8);
            border-radius: 8px;
        }

        .priority-badge {
            display: inline-block;
            padding: 3px 10px;
            border-radius: 12px;
            font-weight: bold;
            font-size: 0.85em;
        }

        .priority-critical {
            background: #e74c3c;
            color: white;
        }

        .priority-high {
            background: #f39c12;
            color: white;
        }

        .priority-medium {
            background: #f1c40f;
            color: #333;
        }

        .priority-low {
            background: #3498db;
            color: white;
        }

        .queue-section {
            margin-top: 20px;
        }

        .queue-table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
            background: white;
            border-radius: 8px;
            overflow: hidden;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }

        .queue-table th {
            background: #667eea;
            color: white;
            padding: 15px;
            text-align: left;
            font-weight: bold;
        }

        .queue-table td {
            padding: 12px 15px;
            border-bottom: 1px solid #ddd;
        }

        .queue-table tr:hover {
            background: #f5f5f5;
        }

        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin-top: 20px;
        }

        .stat-card {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 25px;
            border-radius: 12px;
            text-align: center;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }

        .stat-value {
            font-size: 2.5em;
            font-weight: bold;
            margin-bottom: 5px;
        }

        .stat-label {
            font-size: 1em;
            opacity: 0.9;
        }

        .progress-bar {
            background: #ecf0f1;
            border-radius: 10px;
            height: 30px;
            overflow: hidden;
            margin-top: 20px;
        }

        .progress-fill {
            background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
            height: 100%;
            transition: width 0.5s;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-weight: bold;
        }

        .fibonacci-info {
            background: #e8eaf6;
            padding: 20px;
            border-radius: 12px;
            border-left: 5px solid #3f51b5;
            margin-top: 20px;
        }

        .fibonacci-info h3 {
            color: #3f51b5;
            margin-bottom: 10px;
        }

        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(20px); }
            to { opacity: 1; transform: translateY(0); }
        }

        .fade-in {
            animation: fadeIn 0.5s ease-out;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🏥 Smart Hospital Triage System</h1>
            <p>Real-time ML-Based Patient Allocation with Fibonacci Heap</p>
        </div>

        <div class="controls">
            <button class="btn-primary" onclick="nextIteration()">▶ Next Iteration</button>
            <button class="btn-secondary" onclick="previousIteration()">◀ Previous</button>
            <button class="btn-info" onclick="autoPlay()">⏯ Auto Play</button>
            <button class="btn-danger" onclick="reset()">🔄 Reset</button>
            <button class="btn-secondary" onclick="jumpToEnd()">⏭ Jump to End</button>
        </div>

        <div class="iteration-info" id="iteration-info">
            Iteration: <span id="current-iteration">1</span> / <span id="total-iterations">0</span>
        </div>

        <div class="main-content">
            <!-- Statistics Section -->
            <div class="section">
                <h2 class="section-title">📊 Statistics</h2>
                <div class="stats-grid" id="stats-grid">
                    <!-- Stats will be inserted here -->
                </div>
                <div class="progress-bar">
                    <div class="progress-fill" id="progress-fill">0%</div>
                </div>
            </div>

            <!-- Fibonacci Heap Info -->
            <div class="fibonacci-info">
                <h3>🔷 Fibonacci Heap Operations</h3>
                <p><strong>Insert:</strong> O(1) amortized | <strong>Find-Max:</strong> O(1) | <strong>Extract-Max:</strong> O(log n) amortized</p>
                <p>This system uses a Fibonacci Heap to efficiently manage patient priorities, ensuring the highest priority patient is always treated first.</p>
            </div>

            <!-- Doctors Section -->
            <div class="section">
                <h2 class="section-title">👨‍⚕️ Doctor Status</h2>
                <div class="doctors-grid" id="doctors-grid">
                    <!-- Doctor cards will be inserted here -->
                </div>
            </div>

            <!-- Queue Section -->
            <div class="section">
                <h2 class="section-title">⏳ Waiting Queue (Fibonacci Heap)</h2>
                <div class="queue-section" id="queue-section">
                    <!-- Queue tables will be inserted here -->
                </div>
            </div>
        </div>
    </div>

    <script>
        let data = DATA_PLACEHOLDER;
        let currentIndex = 0;
        let autoPlayInterval = null;

        function getPriorityClass(priority) {
            if (priority >= 90) return 'priority-critical';
            if (priority >= 75) return 'priority-high';
            if (priority >= 60) return 'priority-medium';
            return 'priority-low';
        }

        function getPriorityLabel(priority) {
            if (priority >= 90) return 'CRITICAL';
            if (priority >= 75) return 'HIGH';
            if (priority >= 60) return 'MEDIUM';
            return 'LOW';
        }

        function updateDisplay() {
            if (currentIndex < 0 || currentIndex >= data.iterations.length) return;

            const iteration = data.iterations[currentIndex];

            // Update iteration counter
            document.getElementById('current-iteration').textContent = iteration.iteration;
            document.getElementById('total-iterations').textContent = data.iterations.length;

            // Update statistics
            const stats = iteration.stats;
            const statsHTML = `
                <div class="stat-card fade-in">
                    <div class="stat-value">${stats.totalPatients}</div>
                    <div class="stat-label">Total Patients</div>
                </div>
                <div class="stat-card fade-in">
                    <div class="stat-value">${stats.treated}</div>
                    <div class="stat-label">Treated</div>
                </div>
                <div class="stat-card fade-in">
                    <div class="stat-value">${stats.waiting}</div>
                    <div class="stat-label">Waiting</div>
                </div>
                <div class="stat-card fade-in">
                    <div class="stat-value">${stats.critical}</div>
                    <div class="stat-label">Critical Cases</div>
                </div>
                <div class="stat-card fade-in">
                    <div class="stat-value">${stats.high}</div>
                    <div class="stat-label">High Priority</div>
                </div>
            `;
            document.getElementById('stats-grid').innerHTML = statsHTML;

            // Update progress bar
            const progress = (stats.treated / stats.totalPatients * 100).toFixed(1);
            const progressFill = document.getElementById('progress-fill');
            progressFill.style.width = progress + '%';
            progressFill.textContent = progress + '%';

            // Update doctors
            let doctorsHTML = '';
            iteration.doctors.forEach(doctor => {
                const statusClass = doctor.isBusy ? 'busy' : 'available';
                const statusBadge = doctor.isBusy ? 'status-busy' : 'status-available';
                const statusText = doctor.isBusy ? 'BUSY' : 'AVAILABLE';

                let patientInfo = '';
                if (doctor.isBusy && doctor.currentPatient) {
                    const priorityClass = getPriorityClass(doctor.patientPriority);
                    const priorityLabel = getPriorityLabel(doctor.patientPriority);

                    patientInfo = `
                        <div class="patient-info">
                            <strong>Treating:</strong> ${doctor.currentPatient}<br>
                            <strong>Priority:</strong> <span class="priority-badge ${priorityClass}">${doctor.patientPriority.toFixed(1)} - ${priorityLabel}</span><br>
                            <strong>Time Left:</strong> ⏱ ${doctor.timeLeft}s
                        </div>
                    `;
                }

                doctorsHTML += `
                    <div class="doctor-card ${statusClass} fade-in">
                        <div class="doctor-name">👨‍⚕️ ${doctor.name}</div>
                        <div class="doctor-status ${statusBadge}">${statusText}</div>
                        <div><strong>Department:</strong> ${doctor.department}</div>
                        ${patientInfo}
                    </div>
                `;
            });
            document.getElementById('doctors-grid').innerHTML = doctorsHTML;

            // Update queues
            let queueHTML = '';
            for (const [dept, patients] of Object.entries(iteration.queues)) {
                if (patients.length === 0) {
                    queueHTML += `
                        <h3 style="color: #27ae60; margin-top: 20px;">✓ ${dept} - No patients waiting</h3>
                    `;
                } else {
                    queueHTML += `
                        <h3 style="margin-top: 20px;">${dept} Department (${patients.length} waiting)</h3>
                        <table class="queue-table fade-in">
                            <thead>
                                <tr>
                                    <th>#</th>
                                    <th>Patient Name</th>
                                    <th>Age</th>
                                    <th>Priority</th>
                                    <th>Status</th>
                                </tr>
                            </thead>
                            <tbody>
                    `;

                    patients.forEach((patient, idx) => {
                        const priorityClass = getPriorityClass(patient.priority);
                        const priorityLabel = getPriorityLabel(patient.priority);

                        queueHTML += `
                            <tr>
                                <td>${idx + 1}</td>
                                <td><strong>${patient.name}</strong></td>
                                <td>${patient.age}</td>
                                <td>${patient.priority.toFixed(1)}</td>
                                <td><span class="priority-badge ${priorityClass}">${priorityLabel}</span></td>
                            </tr>
                        `;
                    });

                    queueHTML += `
                            </tbody>
                        </table>
                    `;
                }
            }
            document.getElementById('queue-section').innerHTML = queueHTML;
        }

        function nextIteration() {
            if (currentIndex < data.iterations.length - 1) {
                currentIndex++;
                updateDisplay();
            }
        }

        function previousIteration() {
            if (currentIndex > 0) {
                currentIndex--;
                updateDisplay();
            }
        }

        function reset() {
            currentIndex = 0;
            updateDisplay();
        }

        function jumpToEnd() {
            currentIndex = data.iterations.length - 1;
            updateDisplay();
        }

        function autoPlay() {
            if (autoPlayInterval) {
                clearInterval(autoPlayInterval);
                autoPlayInterval = null;
            } else {
                autoPlayInterval = setInterval(() => {
                    if (currentIndex < data.iterations.length - 1) {
                        nextIteration();
                    } else {
                        clearInterval(autoPlayInterval);
                        autoPlayInterval = null;
                    }
                }, 2000);
            }
        }

        // Keyboard shortcuts
        document.addEventListener('keydown', (e) => {
            if (e.key === 'ArrowRight') nextIteration();
            if (e.key === 'ArrowLeft') previousIteration();
            if (e.key === ' ') { e.preventDefault(); autoPlay(); }
        });

        // Initialize
        updateDisplay();
    </script>
</body>
</html>
"""

def main():
    print("=" * 70)
    print("  CREATING INTERACTIVE HTML DASHBOARD")
    print("=" * 70)
    print()

    # Load JSON data
    try:
        with open('dashboard_data.json', 'r') as f:
            data = json.load(f)
        print("✓ Loaded dashboard data")
    except FileNotFoundError:
        print("❌ Error: dashboard_data.json not found!")
        print("   Please run hospital_html.exe first!")
        return
    except json.JSONDecodeError as e:
        print(f"❌ Error parsing JSON: {e}")
        return

    # Create HTML
    html_content = HTML_TEMPLATE.replace('DATA_PLACEHOLDER', json.dumps(data))

    with open('dashboard.html', 'w', encoding='utf-8') as f:
        f.write(html_content)

    print("✓ Created dashboard.html")
    print()
    print("=" * 70)
    print("✅ DASHBOARD CREATED SUCCESSFULLY!")
    print("=" * 70)
    print()
    print("To view the dashboard:")
    print("  1. Open 'dashboard.html' in your web browser")
    print("  2. Use the buttons to navigate iterations")
    print("  3. Keyboard shortcuts:")
    print("     - Arrow Right: Next iteration")
    print("     - Arrow Left: Previous iteration")
    print("     - Spacebar: Auto play")
    print()
    print("Opening dashboard in browser...")
    os.system('start dashboard.html')

if __name__ == "__main__":
    main()
