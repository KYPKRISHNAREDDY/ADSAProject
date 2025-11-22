# Simple viewer - generates PNG and opens in browser
# Auto-refreshes every time the .dot file changes

import subprocess
import os
import time
import webbrowser
import shutil

def find_graphviz():
    paths = [
        "dot",
        r"C:\Program Files\Graphviz\bin\dot.exe",
        r"C:\Program Files (x86)\Graphviz\bin\dot.exe",
    ]
    for path in paths:
        try:
            result = subprocess.run([path, "-V"], capture_output=True)
            if result.returncode == 0 or b"graphviz" in result.stderr.lower():
                return path
        except:
            continue
    return None

def create_html():
    html = '''<!DOCTYPE html>
<html>
<head>
    <title>Fibonacci Heap Viewer</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background: #f5f5f5;
            margin: 0;
            padding: 20px;
        }
        h1 {
            color: #333;
        }
        .container {
            background: white;
            border-radius: 10px;
            padding: 20px;
            margin: 20px auto;
            max-width: 95%;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        img {
            max-width: 100%;
            height: auto;
        }
        .legend {
            margin-top: 15px;
            font-size: 14px;
        }
        .red { color: #E74C3C; font-weight: bold; }
        .blue { color: #4A90D9; font-weight: bold; }
        .status {
            color: #666;
            font-size: 12px;
            margin-top: 10px;
        }
    </style>
    <script>
        // Auto-refresh every 1 second
        setTimeout(function() {
            location.reload();
        }, 1000);
    </script>
</head>
<body>
    <h1>Fibonacci Heap - Live View</h1>
    <div class="container">
        <img src="heap_view.png" alt="Fibonacci Heap">
        <div class="legend">
            <span class="red">RED = Maximum Priority</span> |
            <span class="blue">BLUE = Other Nodes</span>
        </div>
        <div class="status">Auto-refreshing... Press ENTER in the program to see changes.</div>
    </div>
</body>
</html>'''
    with open('viewer.html', 'w') as f:
        f.write(html)

def main():
    print("=" * 50)
    print("  Fibonacci Heap - Browser Viewer")
    print("=" * 50)

    dot_path = find_graphviz()
    if not dot_path:
        print("ERROR: Graphviz not found!")
        input("Press ENTER to exit...")
        return

    print(f"Using Graphviz: {dot_path}")

    # Create HTML file
    create_html()

    # Create initial empty image
    if not os.path.exists("heap_state.dot"):
        with open("heap_state.dot", "w") as f:
            f.write('digraph G { empty [label="Waiting..."] }')

    # Generate initial PNG (copy first to avoid locking)
    shutil.copy("heap_state.dot", "heap_temp.dot")
    subprocess.run([dot_path, '-Tpng', '-Gdpi=200', 'heap_temp.dot', '-o', 'heap_view.png'])

    # Open browser
    print("Opening browser...")
    webbrowser.open('file://' + os.path.abspath('viewer.html'))

    print()
    print("Browser opened! Now run hospital.exe in this terminal.")
    print("The browser will auto-refresh to show changes.")
    print()
    print("=" * 50)

    last_modified = 0

    # Keep updating PNG when .dot file changes
    print("Watching for changes... (Press Ctrl+C to stop)")
    try:
        while True:
            if os.path.exists("heap_state.dot"):
                try:
                    mod_time = os.path.getmtime("heap_state.dot")
                    if mod_time > last_modified:
                        last_modified = mod_time
                        # Copy file first to avoid locking the original
                        shutil.copy("heap_state.dot", "heap_temp.dot")
                        subprocess.run([dot_path, '-Tpng', '-Gdpi=200', 'heap_temp.dot', '-o', 'heap_view.png'],
                                     capture_output=True)
                        print(f"Updated: {time.strftime('%H:%M:%S')}")
                except (PermissionError, OSError):
                    pass  # File is being written, skip this update
            time.sleep(0.3)
    except KeyboardInterrupt:
        print("\nStopped.")

if __name__ == "__main__":
    main()
