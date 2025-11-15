#!/usr/bin/env python3
"""
Tkinter GUI Visualizer for Hospital Triage System
Creates a popup window with visual representation
"""

import tkinter as tk
from tkinter import ttk, messagebox
import random
import time

class HospitalGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("🏥 Smart Hospital Triage System")
        self.root.geometry("1200x800")
        self.root.configure(bg='#f0f0f0')

        # Load data
        self.load_data()

        # State
        self.current_iteration = 0
        self.doctor_status = {doc['id']: {'busy': False, 'patient': None, 'time': 0}
                            for doc in self.doctors}
        self.waiting_queue = self.patients.copy()
        self.treated = []

        # Create GUI
        self.create_widgets()
        self.update_display()

    def load_data(self):
        """Load data from gui_data.txt"""
        try:
            with open('gui_data.txt', 'r') as f:
                lines = [line.strip() for line in f.readlines()]

            idx = 0
            self.total_patients = int(lines[idx])
            idx += 1
            self.critical_count = int(lines[idx])
            idx += 1
            self.high_count = int(lines[idx])
            idx += 1

            # Load doctors
            num_doctors = int(lines[idx])
            idx += 1
            self.doctors = []
            for i in range(num_doctors):
                parts = lines[idx].split('|')
                self.doctors.append({
                    'id': int(parts[0]),
                    'name': parts[1],
                    'department': parts[2]
                })
                idx += 1

            # Load patients
            num_patients = int(lines[idx])
            idx += 1
            self.patients = []
            for i in range(num_patients):
                parts = lines[idx].split('|')
                self.patients.append({
                    'id': int(parts[0]),
                    'name': parts[1],
                    'age': int(parts[2]),
                    'department': parts[3],
                    'priority': float(parts[4])
                })
                idx += 1

            # Sort by priority (highest first)
            self.patients.sort(key=lambda x: x['priority'], reverse=True)

        except FileNotFoundError:
            messagebox.showerror("Error", "gui_data.txt not found!\nPlease run hospital_gui_data.exe first!")
            self.root.quit()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load data: {e}")
            self.root.quit()

    def create_widgets(self):
        """Create all GUI widgets"""

        # Header
        header = tk.Frame(self.root, bg='#667eea', height=80)
        header.pack(fill=tk.X)

        title = tk.Label(header, text="🏥 SMART HOSPITAL TRIAGE SYSTEM",
                        font=('Arial', 24, 'bold'), bg='#667eea', fg='white')
        title.pack(pady=10)

        subtitle = tk.Label(header, text="Real-time ML-Based Patient Allocation using Fibonacci Heap",
                          font=('Arial', 12), bg='#667eea', fg='white')
        subtitle.pack()

        # Controls
        controls = tk.Frame(self.root, bg='#f0f0f0', height=60)
        controls.pack(fill=tk.X, pady=10)

        tk.Button(controls, text="▶ Next Patient", command=self.next_patient,
                 bg='#4CAF50', fg='white', font=('Arial', 12, 'bold'),
                 padx=20, pady=10).pack(side=tk.LEFT, padx=5)

        tk.Button(controls, text="⏩ Next Iteration", command=self.next_iteration,
                 bg='#2196F3', fg='white', font=('Arial', 12, 'bold'),
                 padx=20, pady=10).pack(side=tk.LEFT, padx=5)

        tk.Button(controls, text="⏭ Auto Play", command=self.auto_play,
                 bg='#FF9800', fg='white', font=('Arial', 12, 'bold'),
                 padx=20, pady=10).pack(side=tk.LEFT, padx=5)

        tk.Button(controls, text="🔄 Reset", command=self.reset,
                 bg='#f44336', fg='white', font=('Arial', 12, 'bold'),
                 padx=20, pady=10).pack(side=tk.LEFT, padx=5)

        # Iteration counter
        self.iteration_label = tk.Label(controls, text="Iteration: 0",
                                       font=('Arial', 14, 'bold'), bg='#f0f0f0')
        self.iteration_label.pack(side=tk.RIGHT, padx=20)

        # Main container with scrollbar
        main_container = tk.Frame(self.root, bg='white')
        main_container.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        canvas = tk.Canvas(main_container, bg='white')
        scrollbar = ttk.Scrollbar(main_container, orient="vertical", command=canvas.yview)
        scrollable_frame = tk.Frame(canvas, bg='white')

        scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )

        canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)

        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        self.main_frame = scrollable_frame

    def update_display(self):
        """Update all visual elements"""

        # Clear existing widgets
        for widget in self.main_frame.winfo_children():
            widget.destroy()

        # Statistics Section
        stats_frame = tk.LabelFrame(self.main_frame, text="📊 STATISTICS",
                                   font=('Arial', 16, 'bold'), bg='white',
                                   fg='#667eea', padx=10, pady=10)
        stats_frame.pack(fill=tk.X, padx=10, pady=10)

        stats_grid = tk.Frame(stats_frame, bg='white')
        stats_grid.pack()

        stats = [
            ("Total Patients", self.total_patients, '#667eea'),
            ("Treated", len(self.treated), '#4CAF50'),
            ("Waiting", len(self.waiting_queue), '#FF9800'),
            ("Critical", self.critical_count, '#f44336'),
            ("High Priority", self.high_count, '#FF6B6B')
        ]

        for idx, (label, value, color) in enumerate(stats):
            frame = tk.Frame(stats_grid, bg=color, width=150, height=100)
            frame.grid(row=0, column=idx, padx=10, pady=5)
            frame.pack_propagate(False)

            tk.Label(frame, text=str(value), font=('Arial', 32, 'bold'),
                    bg=color, fg='white').pack(expand=True)
            tk.Label(frame, text=label, font=('Arial', 10),
                    bg=color, fg='white').pack()

        # Progress bar
        progress = len(self.treated) / self.total_patients if self.total_patients > 0 else 0
        progress_frame = tk.Frame(stats_frame, bg='white', height=40)
        progress_frame.pack(fill=tk.X, pady=10)

        tk.Label(progress_frame, text="Progress:", font=('Arial', 12, 'bold'),
                bg='white').pack(side=tk.LEFT, padx=5)

        progress_bar = ttk.Progressbar(progress_frame, length=800, mode='determinate')
        progress_bar['value'] = progress * 100
        progress_bar.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)

        tk.Label(progress_frame, text=f"{progress*100:.1f}%",
                font=('Arial', 12, 'bold'), bg='white').pack(side=tk.LEFT, padx=5)

        # Fibonacci Heap Info
        fib_frame = tk.Frame(self.main_frame, bg='#e8eaf6', padx=15, pady=15)
        fib_frame.pack(fill=tk.X, padx=10, pady=10)

        tk.Label(fib_frame, text="🔷 FIBONACCI HEAP OPERATIONS",
                font=('Arial', 14, 'bold'), bg='#e8eaf6', fg='#3f51b5').pack(anchor=tk.W)

        tk.Label(fib_frame,
                text="INSERT: O(1) amortized  |  FIND-MAX: O(1)  |  EXTRACT-MAX: O(log n) amortized",
                font=('Arial', 11), bg='#e8eaf6').pack(anchor=tk.W, pady=5)

        tk.Label(fib_frame,
                text="This system uses Fibonacci Heap for optimal priority-based patient scheduling.",
                font=('Arial', 10), bg='#e8eaf6', fg='#555').pack(anchor=tk.W)

        # Doctors Section
        doctors_frame = tk.LabelFrame(self.main_frame, text="👨‍⚕️ DOCTOR STATUS",
                                     font=('Arial', 16, 'bold'), bg='white',
                                     fg='#667eea', padx=10, pady=10)
        doctors_frame.pack(fill=tk.X, padx=10, pady=10)

        doctors_grid = tk.Frame(doctors_frame, bg='white')
        doctors_grid.pack()

        for idx, doc in enumerate(self.doctors):
            status = self.doctor_status[doc['id']]
            is_busy = status['busy']

            # Doctor card
            card_bg = '#ffcccc' if is_busy else '#ccffcc'
            card = tk.Frame(doctors_grid, bg=card_bg, relief=tk.RAISED,
                          borderwidth=2, width=280, height=150)
            card.grid(row=idx//2, column=idx%2, padx=10, pady=10, sticky='nsew')
            card.pack_propagate(False)

            # Doctor name
            tk.Label(card, text=f"👨‍⚕️ {doc['name']}", font=('Arial', 13, 'bold'),
                    bg=card_bg).pack(pady=5)

            # Department
            tk.Label(card, text=doc['department'], font=('Arial', 10),
                    bg=card_bg, fg='#666').pack()

            # Status
            status_color = '#f44336' if is_busy else '#4CAF50'
            status_text = 'BUSY' if is_busy else 'AVAILABLE'
            status_label = tk.Label(card, text=status_text,
                                   font=('Arial', 11, 'bold'),
                                   bg=status_color, fg='white',
                                   padx=15, pady=3)
            status_label.pack(pady=5)

            # Patient info if busy
            if is_busy and status['patient']:
                patient = status['patient']
                info_frame = tk.Frame(card, bg='white', padx=5, pady=5)
                info_frame.pack(fill=tk.X, padx=5, pady=5)

                tk.Label(info_frame, text=f"Treating: {patient['name']}",
                        font=('Arial', 9, 'bold'), bg='white').pack(anchor=tk.W)

                priority_color = self.get_priority_color(patient['priority'])
                tk.Label(info_frame,
                        text=f"Priority: {patient['priority']:.1f}",
                        font=('Arial', 9), bg=priority_color, fg='white',
                        padx=5).pack(anchor=tk.W, pady=2)

                tk.Label(info_frame, text=f"Time left: {status['time']}s",
                        font=('Arial', 9), bg='white').pack(anchor=tk.W)

        # Waiting Queue Section
        queue_frame = tk.LabelFrame(self.main_frame, text="⏳ WAITING QUEUE (Fibonacci Heap)",
                                   font=('Arial', 16, 'bold'), bg='white',
                                   fg='#667eea', padx=10, pady=10)
        queue_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        if not self.waiting_queue:
            tk.Label(queue_frame, text="✓ No patients waiting!",
                    font=('Arial', 14, 'bold'), bg='white',
                    fg='#4CAF50').pack(pady=20)
        else:
            # Create table
            columns = ('Pos', 'Name', 'Age', 'Department', 'Priority', 'Status')
            tree = ttk.Treeview(queue_frame, columns=columns, show='headings', height=10)

            # Column headings
            for col in columns:
                tree.heading(col, text=col)
                tree.column(col, width=120 if col != 'Name' else 200)

            # Add patients (top 15)
            for idx, patient in enumerate(self.waiting_queue[:15]):
                priority = patient['priority']
                status = self.get_priority_label(priority)
                values = (
                    idx + 1,
                    patient['name'],
                    patient['age'],
                    patient['department'],
                    f"{priority:.1f}",
                    status
                )

                # Color tag
                tag = 'critical' if priority >= 90 else 'high' if priority >= 75 else 'medium'
                tree.insert('', tk.END, values=values, tags=(tag,))

            # Configure tags
            tree.tag_configure('critical', background='#ffcccc')
            tree.tag_configure('high', background='#ffe6cc')
            tree.tag_configure('medium', background='#ffffcc')

            tree.pack(fill=tk.BOTH, expand=True)

            if len(self.waiting_queue) > 15:
                tk.Label(queue_frame,
                        text=f"... and {len(self.waiting_queue) - 15} more patients",
                        font=('Arial', 10, 'italic'), bg='white',
                        fg='#666').pack(pady=5)

    def get_priority_color(self, priority):
        """Get color for priority"""
        if priority >= 90:
            return '#f44336'  # Red
        elif priority >= 75:
            return '#FF9800'  # Orange
        elif priority >= 60:
            return '#FFC107'  # Yellow
        else:
            return '#4CAF50'  # Green

    def get_priority_label(self, priority):
        """Get label for priority"""
        if priority >= 90:
            return 'CRITICAL'
        elif priority >= 75:
            return 'HIGH'
        elif priority >= 60:
            return 'MEDIUM'
        else:
            return 'LOW'

    def next_patient(self):
        """Allocate next patient"""
        # Update doctor timers
        for doc_id in self.doctor_status:
            if self.doctor_status[doc_id]['busy']:
                self.doctor_status[doc_id]['time'] -= 1
                if self.doctor_status[doc_id]['time'] <= 0:
                    patient = self.doctor_status[doc_id]['patient']
                    self.treated.append(patient)
                    self.doctor_status[doc_id] = {'busy': False, 'patient': None, 'time': 0}

        # Try to allocate
        if self.waiting_queue:
            # Find available doctor
            for doc in self.doctors:
                if not self.doctor_status[doc['id']]['busy']:
                    # Find next patient for this department
                    for i, patient in enumerate(self.waiting_queue):
                        if patient['department'] == doc['department']:
                            # Allocate
                            self.waiting_queue.pop(i)
                            treatment_time = 3 if patient['priority'] >= 90 else 5
                            self.doctor_status[doc['id']] = {
                                'busy': True,
                                'patient': patient,
                                'time': treatment_time
                            }
                            break
                    break

        self.current_iteration += 1
        self.update_display()
        self.iteration_label.config(text=f"Iteration: {self.current_iteration}")

    def next_iteration(self):
        """Run full iteration"""
        for _ in range(5):  # Process 5 steps
            self.next_patient()
            self.root.update()
            time.sleep(0.3)

    def auto_play(self):
        """Auto-play through all iterations"""
        while self.waiting_queue or any(self.doctor_status[d]['busy'] for d in self.doctor_status):
            self.next_patient()
            self.root.update()
            time.sleep(0.5)

        messagebox.showinfo("Complete", f"All {self.total_patients} patients treated!")

    def reset(self):
        """Reset simulation"""
        self.current_iteration = 0
        self.doctor_status = {doc['id']: {'busy': False, 'patient': None, 'time': 0}
                            for doc in self.doctors}
        self.waiting_queue = self.patients.copy()
        self.treated = []
        self.update_display()
        self.iteration_label.config(text="Iteration: 0")

def main():
    root = tk.Tk()
    app = HospitalGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
