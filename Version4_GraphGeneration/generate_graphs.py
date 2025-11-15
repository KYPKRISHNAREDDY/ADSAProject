#!/usr/bin/env python3
"""
Graph Generation Script for Hospital Triage System
Generates professional visualizations from CSV data
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
from datetime import datetime

# Set style
sns.set_style("whitegrid")
plt.rcParams['figure.figsize'] = (12, 8)
plt.rcParams['font.size'] = 10

def ensure_output_dir():
    """Create output directory for graphs"""
    if not os.path.exists('graphs'):
        os.makedirs('graphs')
    print("✓ Output directory ready: graphs/")

def load_data():
    """Load all CSV files"""
    try:
        timeline = pd.read_csv('graph_data/allocation_timeline.csv')
        queue_sizes = pd.read_csv('graph_data/queue_sizes.csv')
        priority_dist = pd.read_csv('graph_data/priority_distribution.csv')
        doctor_util = pd.read_csv('graph_data/doctor_utilization.csv')
        ml_scores = pd.read_csv('graph_data/ml_scores.csv')

        print("✓ All data files loaded successfully")
        return timeline, queue_sizes, priority_dist, doctor_util, ml_scores
    except Exception as e:
        print(f"Error loading data: {e}")
        return None, None, None, None, None

def create_queue_timeline(queue_sizes):
    """Graph 1: Queue sizes over time"""
    plt.figure(figsize=(14, 6))

    plt.plot(queue_sizes['Iteration'], queue_sizes['TotalWaiting'],
             marker='o', linewidth=2, markersize=4, label='Total Waiting', color='#2E86AB')
    plt.plot(queue_sizes['Iteration'], queue_sizes['CardiologyWaiting'],
             marker='s', linewidth=1.5, markersize=3, label='Cardiology',
             color='#A23B72', linestyle='--')
    plt.plot(queue_sizes['Iteration'], queue_sizes['EmergencyWaiting'],
             marker='^', linewidth=1.5, markersize=3, label='Emergency',
             color='#F18F01', linestyle='--')

    plt.xlabel('Iteration', fontsize=12, fontweight='bold')
    plt.ylabel('Number of Patients Waiting', fontsize=12, fontweight='bold')
    plt.title('Queue Size Over Time\n(Fibonacci Heap Dynamic Priority Queue)',
              fontsize=14, fontweight='bold', pad=20)
    plt.legend(loc='upper right', fontsize=11, framealpha=0.9)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    plt.savefig('graphs/1_queue_timeline.png', dpi=300, bbox_inches='tight')
    print("✓ Created: graphs/1_queue_timeline.png")
    plt.close()

def create_priority_distribution(priority_dist):
    """Graph 2: Priority distribution pie chart"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    # Pie chart
    colors = ['#E63946', '#F77F00', '#FCBF49', '#06A77D']
    explode = (0.1, 0.05, 0, 0)

    ax1.pie(priority_dist['Count'], labels=priority_dist['Category'],
            autopct='%1.1f%%', startangle=90, colors=colors, explode=explode,
            textprops={'fontsize': 11, 'fontweight': 'bold'})
    ax1.set_title('Patient Priority Distribution\n(ML Triage Scores)',
                  fontsize=13, fontweight='bold', pad=15)

    # Bar chart
    bars = ax2.bar(priority_dist['Category'], priority_dist['Count'],
                   color=colors, edgecolor='black', linewidth=1.5)
    ax2.set_xlabel('Priority Category', fontsize=12, fontweight='bold')
    ax2.set_ylabel('Number of Patients', fontsize=12, fontweight='bold')
    ax2.set_title('Patient Count by Priority\n(Random Forest ML Model)',
                  fontsize=13, fontweight='bold', pad=15)
    ax2.grid(axis='y', alpha=0.3)

    # Add value labels on bars
    for bar in bars:
        height = bar.get_height()
        ax2.text(bar.get_x() + bar.get_width()/2., height,
                f'{int(height)}', ha='center', va='bottom',
                fontsize=11, fontweight='bold')

    plt.xticks(rotation=15, ha='right')
    plt.tight_layout()
    plt.savefig('graphs/2_priority_distribution.png', dpi=300, bbox_inches='tight')
    print("✓ Created: graphs/2_priority_distribution.png")
    plt.close()

def create_doctor_gantt(doctor_util):
    """Graph 3: Doctor utilization Gantt chart"""
    plt.figure(figsize=(14, 8))

    doctors = doctor_util['DoctorName'].unique()
    colors_map = {'Cardiology': '#D90368', 'Emergency': '#00CC66'}

    y_pos = 0
    for doctor in doctors:
        doc_data = doctor_util[doctor_util['DoctorName'] == doctor]

        for _, row in doc_data.iterrows():
            duration = row['EndTime'] - row['StartTime']
            color = colors_map.get(row['Department'], '#888888')

            plt.barh(y_pos, duration, left=row['StartTime'], height=0.6,
                    color=color, edgecolor='black', linewidth=0.5,
                    label=row['Department'] if row['Department'] not in plt.gca().get_legend_handles_labels()[1] else "")

            # Add patient name
            plt.text(row['StartTime'] + duration/2, y_pos,
                    row['PatientName'].split()[0],  # First name only
                    ha='center', va='center', fontsize=8,
                    fontweight='bold', color='white')

        y_pos += 1

    plt.yticks(range(len(doctors)), doctors, fontsize=11)
    plt.xlabel('Time (seconds)', fontsize=12, fontweight='bold')
    plt.ylabel('Doctor', fontsize=12, fontweight='bold')
    plt.title('Doctor Utilization Timeline\n(Real-time Dynamic Allocation)',
              fontsize=14, fontweight='bold', pad=20)
    plt.legend(loc='upper right', fontsize=11)
    plt.grid(axis='x', alpha=0.3)
    plt.tight_layout()

    plt.savefig('graphs/3_doctor_utilization.png', dpi=300, bbox_inches='tight')
    print("✓ Created: graphs/3_doctor_utilization.png")
    plt.close()

def create_allocation_order(timeline):
    """Graph 4: Allocation timeline showing order"""
    plt.figure(figsize=(14, 7))

    # Sort by priority (highest first)
    timeline_sorted = timeline.sort_values('Priority', ascending=False).head(20)

    # Color code by priority
    colors = []
    for priority in timeline_sorted['Priority']:
        if priority >= 90:
            colors.append('#E63946')  # Red - Critical
        elif priority >= 75:
            colors.append('#F77F00')  # Orange - High
        elif priority >= 60:
            colors.append('#FCBF49')  # Yellow - Medium
        else:
            colors.append('#06A77D')  # Green - Low

    bars = plt.barh(range(len(timeline_sorted)), timeline_sorted['Priority'],
                    color=colors, edgecolor='black', linewidth=1)

    plt.yticks(range(len(timeline_sorted)),
               [f"{row['PatientName']}" for _, row in timeline_sorted.iterrows()],
               fontsize=9)
    plt.xlabel('Priority Score (ML Triage)', fontsize=12, fontweight='bold')
    plt.ylabel('Patient Name', fontsize=12, fontweight='bold')
    plt.title('Top 20 Patients by Priority\n(Highest Priority Treated First - Fibonacci Heap Extract-Max)',
              fontsize=13, fontweight='bold', pad=20)
    plt.grid(axis='x', alpha=0.3)

    # Add priority values
    for i, (_, row) in enumerate(timeline_sorted.iterrows()):
        plt.text(row['Priority'] + 1, i, f"{row['Priority']:.1f}",
                va='center', fontsize=9, fontweight='bold')

    plt.tight_layout()
    plt.savefig('graphs/4_allocation_order.png', dpi=300, bbox_inches='tight')
    print("✓ Created: graphs/4_allocation_order.png")
    plt.close()

def create_ml_score_distribution(ml_scores):
    """Graph 5: ML score distribution histogram and box plot"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    # Histogram
    ax1.hist(ml_scores['MLScore'], bins=20, color='#4361EE',
            edgecolor='black', linewidth=1.2, alpha=0.7)
    ax1.axvline(ml_scores['MLScore'].mean(), color='red',
               linestyle='--', linewidth=2, label=f'Mean: {ml_scores["MLScore"].mean():.1f}')
    ax1.axvline(ml_scores['MLScore'].median(), color='green',
               linestyle='--', linewidth=2, label=f'Median: {ml_scores["MLScore"].median():.1f}')
    ax1.set_xlabel('ML Triage Score', fontsize=12, fontweight='bold')
    ax1.set_ylabel('Frequency', fontsize=12, fontweight='bold')
    ax1.set_title('ML Score Distribution\n(Random Forest Regressor Output)',
                 fontsize=13, fontweight='bold', pad=15)
    ax1.legend(fontsize=10)
    ax1.grid(axis='y', alpha=0.3)

    # Box plot by department
    departments = ml_scores['Department'].unique()
    dept_data = [ml_scores[ml_scores['Department'] == dept]['MLScore'].values
                 for dept in departments]

    bp = ax2.boxplot(dept_data, labels=departments, patch_artist=True,
                     boxprops=dict(facecolor='#4CC9F0', edgecolor='black', linewidth=1.5),
                     medianprops=dict(color='red', linewidth=2),
                     whiskerprops=dict(linewidth=1.5),
                     capprops=dict(linewidth=1.5))

    ax2.set_xlabel('Department', fontsize=12, fontweight='bold')
    ax2.set_ylabel('ML Triage Score', fontsize=12, fontweight='bold')
    ax2.set_title('ML Score by Department\n(Statistical Distribution)',
                 fontsize=13, fontweight='bold', pad=15)
    ax2.grid(axis='y', alpha=0.3)

    plt.tight_layout()
    plt.savefig('graphs/5_ml_score_analysis.png', dpi=300, bbox_inches='tight')
    print("✓ Created: graphs/5_ml_score_analysis.png")
    plt.close()

def create_summary_stats(timeline, queue_sizes, priority_dist, ml_scores):
    """Graph 6: Summary statistics dashboard"""
    fig = plt.figure(figsize=(14, 10))
    gs = fig.add_gridspec(3, 2, hspace=0.3, wspace=0.3)

    # Total patients over time
    ax1 = fig.add_subplot(gs[0, :])
    cumulative = list(range(1, len(timeline) + 1))
    ax1.plot(timeline['Iteration'], cumulative, marker='o',
            linewidth=2, markersize=4, color='#06A77D')
    ax1.fill_between(timeline['Iteration'], cumulative, alpha=0.3, color='#06A77D')
    ax1.set_xlabel('Iteration', fontsize=11, fontweight='bold')
    ax1.set_ylabel('Patients Treated', fontsize=11, fontweight='bold')
    ax1.set_title('Cumulative Patients Treated Over Time', fontsize=12, fontweight='bold')
    ax1.grid(True, alpha=0.3)

    # Priority category counts
    ax2 = fig.add_subplot(gs[1, 0])
    colors = ['#E63946', '#F77F00', '#FCBF49', '#06A77D']
    ax2.bar(priority_dist['Category'], priority_dist['Count'],
           color=colors, edgecolor='black', linewidth=1.5)
    ax2.set_title('Priority Categories', fontsize=12, fontweight='bold')
    ax2.set_ylabel('Count', fontsize=11, fontweight='bold')
    plt.setp(ax2.xaxis.get_majorticklabels(), rotation=30, ha='right', fontsize=9)
    ax2.grid(axis='y', alpha=0.3)

    # Department distribution
    ax3 = fig.add_subplot(gs[1, 1])
    dept_counts = ml_scores['Department'].value_counts()
    ax3.pie(dept_counts.values, labels=dept_counts.index, autopct='%1.1f%%',
           colors=['#D90368', '#00CC66'], textprops={'fontsize': 10, 'fontweight': 'bold'})
    ax3.set_title('Patients by Department', fontsize=12, fontweight='bold')

    # Statistics table
    ax4 = fig.add_subplot(gs[2, :])
    ax4.axis('tight')
    ax4.axis('off')

    stats_data = [
        ['Total Patients', str(len(ml_scores))],
        ['Patients Treated', str(len(timeline))],
        ['Average ML Score', f"{ml_scores['MLScore'].mean():.2f}"],
        ['Median ML Score', f"{ml_scores['MLScore'].median():.2f}"],
        ['Highest Priority', f"{ml_scores['MLScore'].max():.2f}"],
        ['Lowest Priority', f"{ml_scores['MLScore'].min():.2f}"],
        ['Total Iterations', str(len(queue_sizes))],
        ['Critical Cases', str(priority_dist[priority_dist['Category'].str.contains('Critical')]['Count'].values[0])],
    ]

    table = ax4.table(cellText=stats_data, colLabels=['Metric', 'Value'],
                     cellLoc='left', loc='center',
                     colWidths=[0.6, 0.4])
    table.auto_set_font_size(False)
    table.set_fontsize(11)
    table.scale(1, 2)

    for i in range(len(stats_data) + 1):
        if i == 0:
            table[(i, 0)].set_facecolor('#4361EE')
            table[(i, 1)].set_facecolor('#4361EE')
            table[(i, 0)].set_text_props(weight='bold', color='white')
            table[(i, 1)].set_text_props(weight='bold', color='white')
        else:
            table[(i, 0)].set_facecolor('#E8F4F8' if i % 2 == 0 else 'white')
            table[(i, 1)].set_facecolor('#E8F4F8' if i % 2 == 0 else 'white')

    ax4.set_title('Summary Statistics', fontsize=13, fontweight='bold', pad=20)

    plt.savefig('graphs/6_summary_dashboard.png', dpi=300, bbox_inches='tight')
    print("✓ Created: graphs/6_summary_dashboard.png")
    plt.close()

def create_report_html(timeline, queue_sizes, priority_dist, ml_scores):
    """Create HTML report with all graphs"""
    html_content = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Hospital Triage System - Analysis Report</title>
        <style>
            body {{
                font-family: 'Segoe UI', Arial, sans-serif;
                max-width: 1200px;
                margin: 0 auto;
                padding: 20px;
                background-color: #f5f5f5;
            }}
            h1 {{
                color: #2E86AB;
                text-align: center;
                border-bottom: 3px solid #2E86AB;
                padding-bottom: 10px;
            }}
            h2 {{
                color: #A23B72;
                margin-top: 30px;
            }}
            .graph {{
                background: white;
                padding: 20px;
                margin: 20px 0;
                border-radius: 8px;
                box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            }}
            img {{
                max-width: 100%;
                height: auto;
                display: block;
                margin: 0 auto;
            }}
            .stats {{
                background: #E8F4F8;
                padding: 15px;
                border-left: 4px solid #2E86AB;
                margin: 20px 0;
            }}
            .footer {{
                text-align: center;
                margin-top: 40px;
                padding-top: 20px;
                border-top: 2px solid #ccc;
                color: #666;
            }}
        </style>
    </head>
    <body>
        <h1>🏥 Smart Hospital Triage System - Analysis Report</h1>
        <div class="stats">
            <h3>Key Statistics</h3>
            <p><strong>Total Patients:</strong> {len(ml_scores)}</p>
            <p><strong>Average ML Score:</strong> {ml_scores['MLScore'].mean():.2f}</p>
            <p><strong>Total Iterations:</strong> {len(queue_sizes)}</p>
            <p><strong>Report Generated:</strong> {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        </div>

        <div class="graph">
            <h2>1. Queue Size Timeline</h2>
            <p>Shows how the waiting queue decreased over time as patients were treated.</p>
            <img src="1_queue_timeline.png" alt="Queue Timeline">
        </div>

        <div class="graph">
            <h2>2. Priority Distribution</h2>
            <p>Distribution of patients across different priority categories based on ML triage scores.</p>
            <img src="2_priority_distribution.png" alt="Priority Distribution">
        </div>

        <div class="graph">
            <h2>3. Doctor Utilization</h2>
            <p>Gantt chart showing when each doctor was treating patients.</p>
            <img src="3_doctor_utilization.png" alt="Doctor Utilization">
        </div>

        <div class="graph">
            <h2>4. Top Patients by Priority</h2>
            <p>Highest priority patients treated first using Fibonacci Heap extract-max operation.</p>
            <img src="4_allocation_order.png" alt="Allocation Order">
        </div>

        <div class="graph">
            <h2>5. ML Score Analysis</h2>
            <p>Statistical distribution of ML triage scores across all patients.</p>
            <img src="5_ml_score_analysis.png" alt="ML Score Analysis">
        </div>

        <div class="graph">
            <h2>6. Summary Dashboard</h2>
            <p>Comprehensive overview of system performance and statistics.</p>
            <img src="6_summary_dashboard.png" alt="Summary Dashboard">
        </div>

        <div class="footer">
            <p><strong>Smart Hospital Triage System</strong></p>
            <p>Using Fibonacci Heap Data Structure + Machine Learning</p>
            <p>Data Structures and Algorithms Project</p>
        </div>
    </body>
    </html>
    """

    with open('graphs/REPORT.html', 'w', encoding='utf-8') as f:
        f.write(html_content)

    print("✓ Created: graphs/REPORT.html")

def main():
    print("=" * 70)
    print("  HOSPITAL TRIAGE SYSTEM - GRAPH GENERATION")
    print("=" * 70)
    print()

    ensure_output_dir()

    print("\nLoading data...")
    timeline, queue_sizes, priority_dist, doctor_util, ml_scores = load_data()

    if timeline is None:
        print("\n❌ Error: Could not load data files!")
        print("Make sure to run hospital_graphs.exe first!")
        return

    print("\nGenerating graphs...")
    print("-" * 70)

    create_queue_timeline(queue_sizes)
    create_priority_distribution(priority_dist)
    create_doctor_gantt(doctor_util)
    create_allocation_order(timeline)
    create_ml_score_distribution(ml_scores)
    create_summary_stats(timeline, queue_sizes, priority_dist, ml_scores)

    print("\nGenerating HTML report...")
    print("-" * 70)
    create_report_html(timeline, queue_sizes, priority_dist, ml_scores)

    print("\n" + "=" * 70)
    print("✅ ALL GRAPHS GENERATED SUCCESSFULLY!")
    print("=" * 70)
    print("\nOutput files in graphs/ folder:")
    print("  - 1_queue_timeline.png")
    print("  - 2_priority_distribution.png")
    print("  - 3_doctor_utilization.png")
    print("  - 4_allocation_order.png")
    print("  - 5_ml_score_analysis.png")
    print("  - 6_summary_dashboard.png")
    print("  - REPORT.html (Open in browser!)")
    print("\n📊 Open REPORT.html in your browser to see all graphs!")
    print()

if __name__ == "__main__":
    main()
