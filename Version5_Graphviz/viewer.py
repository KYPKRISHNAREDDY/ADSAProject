# Auto-updating Graphviz Viewer
# This window automatically refreshes when heap_state.dot changes

import tkinter as tk
from tkinter import Label, Canvas, Scrollbar, Frame
import subprocess
import os
import time
from PIL import Image, ImageTk

# Find Graphviz dot.exe
def find_graphviz():
    possible_paths = [
        "dot",
        r"C:\Program Files\Graphviz\bin\dot.exe",
        r"C:\Program Files (x86)\Graphviz\bin\dot.exe",
        r"C:\Graphviz\bin\dot.exe",
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
        self.root.geometry("1000x700")
        self.root.configure(bg='white')

        # Title
        title = Label(self.root, text="Fibonacci Heap Structure",
                     font=('Arial', 18, 'bold'), bg='white')
        title.pack(pady=10)

        # Frame for image with scrollbars
        self.frame = Frame(self.root, bg='white')
        self.frame.pack(expand=True, fill='both', padx=10, pady=10)

        # Canvas with scrollbars
        self.canvas = Canvas(self.frame, bg='white')
        self.scrollbar_y = Scrollbar(self.frame, orient='vertical', command=self.canvas.yview)
        self.scrollbar_x = Scrollbar(self.frame, orient='horizontal', command=self.canvas.xview)

        self.canvas.configure(yscrollcommand=self.scrollbar_y.set, xscrollcommand=self.scrollbar_x.set)

        self.scrollbar_y.pack(side='right', fill='y')
        self.scrollbar_x.pack(side='bottom', fill='x')
        self.canvas.pack(side='left', expand=True, fill='both')

        # Status label
        self.status = Label(self.root, text="Waiting for heap_state.dot...",
                           font=('Arial', 11), bg='white', fg='gray')
        self.status.pack(pady=5)

        # Legend
        legend = Label(self.root, text="RED = Maximum Priority Node  |  BLUE = Other Nodes",
                      font=('Arial', 10), bg='white', fg='#666')
        legend.pack(pady=5)

        self.last_modified = 0
        self.photo = None
        self.check_for_updates()

    def check_for_updates(self):
        dot_file = "heap_state.dot"
        png_file = "heap_view.png"

        try:
            if os.path.exists(dot_file):
                mod_time = os.path.getmtime(dot_file)

                if mod_time > self.last_modified:
                    self.last_modified = mod_time

                    # Generate HIGH QUALITY PNG (higher DPI)
                    result = subprocess.run(
                        [self.dot_path, '-Tpng', '-Gdpi=150', dot_file, '-o', png_file],
                        capture_output=True, text=True
                    )

                    if result.returncode == 0 and os.path.exists(png_file):
                        # Load image with PIL for better quality
                        img = Image.open(png_file)
                        self.photo = ImageTk.PhotoImage(img)

                        # Update canvas
                        self.canvas.delete("all")
                        self.canvas.create_image(0, 0, anchor='nw', image=self.photo)
                        self.canvas.configure(scrollregion=self.canvas.bbox("all"))

                        self.status.config(text=f"Updated: {time.strftime('%H:%M:%S')}", fg='green')
                    else:
                        self.status.config(text="Error rendering graph", fg='red')
            else:
                self.status.config(text="Waiting for heap_state.dot... (Run hospital.exe)", fg='gray')

        except Exception as e:
            self.status.config(text=f"Error: {str(e)}", fg='red')

        self.root.after(500, self.check_for_updates)

    def run(self):
        self.root.mainloop()

if __name__ == "__main__":
    print("=" * 50)
    print("  Fibonacci Heap Live Viewer")
    print("=" * 50)
    print()

    dot_path = find_graphviz()

    if dot_path is None:
        print("ERROR: Graphviz not found!")
        print("Install from: https://graphviz.org/download/")
        input("Press ENTER to exit...")
        exit(1)

    # Check if PIL is available
    try:
        from PIL import Image, ImageTk
    except ImportError:
        print("Installing Pillow for better image quality...")
        os.system("pip install Pillow")
        from PIL import Image, ImageTk

    print("Viewer ready! Run hospital.exe in another terminal.")
    print("=" * 50)

    viewer = HeapViewer(dot_path)
    viewer.run()
