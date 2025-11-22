# Auto-updating Graphviz Viewer
# This window automatically refreshes when heap_state.dot changes

import tkinter as tk
from tkinter import Label
import subprocess
import os
import time

# Find Graphviz dot.exe
def find_graphviz():
    # Common installation paths on Windows
    possible_paths = [
        "dot",  # If in PATH
        r"C:\Program Files\Graphviz\bin\dot.exe",
        r"C:\Program Files (x86)\Graphviz\bin\dot.exe",
        r"C:\Graphviz\bin\dot.exe",
        os.path.expanduser(r"~\AppData\Local\Programs\Graphviz\bin\dot.exe"),
    ]

    for path in possible_paths:
        try:
            result = subprocess.run([path, "-V"], capture_output=True, text=True)
            if result.returncode == 0 or "graphviz" in result.stderr.lower():
                print(f"Found Graphviz at: {path}")
                return path
        except:
            continue

    return None

class HeapViewer:
    def __init__(self, dot_path):
        self.dot_path = dot_path
        self.root = tk.Tk()
        self.root.title("Fibonacci Heap - Live View")
        self.root.geometry("800x600")
        self.root.configure(bg='white')

        # Title
        title = Label(self.root, text="Fibonacci Heap Structure",
                     font=('Arial', 16, 'bold'), bg='white')
        title.pack(pady=10)

        # Image label
        self.img_label = Label(self.root, bg='white')
        self.img_label.pack(expand=True, fill='both', padx=20, pady=10)

        # Status label
        self.status = Label(self.root, text="Waiting for heap_state.dot...",
                           font=('Arial', 10), bg='white', fg='gray')
        self.status.pack(pady=5)

        # Track file modification time
        self.last_modified = 0

        # Start checking for updates
        self.check_for_updates()

    def check_for_updates(self):
        dot_file = "heap_state.dot"
        png_file = "heap_view.png"

        try:
            if os.path.exists(dot_file):
                # Check if file was modified
                mod_time = os.path.getmtime(dot_file)

                if mod_time > self.last_modified:
                    self.last_modified = mod_time

                    # Convert .dot to .png using Graphviz
                    result = subprocess.run(
                        [self.dot_path, '-Tpng', dot_file, '-o', png_file],
                        capture_output=True, text=True
                    )

                    if result.returncode == 0 and os.path.exists(png_file):
                        # Load and display image
                        self.photo = tk.PhotoImage(file=png_file)

                        # Scale if too large
                        width = self.photo.width()
                        height = self.photo.height()

                        if width > 750 or height > 500:
                            scale = max(width // 750, height // 500) + 1
                            self.photo = self.photo.subsample(scale, scale)

                        self.img_label.config(image=self.photo)
                        self.status.config(text=f"Updated: {time.strftime('%H:%M:%S')}", fg='green')
                    else:
                        self.status.config(text="Error rendering graph", fg='red')
            else:
                self.status.config(text="Waiting for heap_state.dot...", fg='gray')

        except Exception as e:
            self.status.config(text=f"Error: {str(e)}", fg='red')

        # Check again in 500ms
        self.root.after(500, self.check_for_updates)

    def run(self):
        self.root.mainloop()

if __name__ == "__main__":
    print("=" * 50)
    print("  Fibonacci Heap Live Viewer")
    print("=" * 50)
    print()

    # Find Graphviz
    dot_path = find_graphviz()

    if dot_path is None:
        print("ERROR: Graphviz not found!")
        print()
        print("Please tell me where you installed Graphviz.")
        print("Check these locations:")
        print("  - C:\\Program Files\\Graphviz\\bin\\")
        print("  - C:\\Program Files (x86)\\Graphviz\\bin\\")
        print()
        input("Press ENTER to exit...")
        exit(1)

    print("This window will automatically update when")
    print("heap_state.dot changes.")
    print()
    print("Run hospital.exe in another terminal!")
    print("=" * 50)

    viewer = HeapViewer(dot_path)
    viewer.run()
