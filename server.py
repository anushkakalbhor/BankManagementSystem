"""
server.py — Bridge between HTML frontend and bank.c
Requires: Python 3 (no extra libraries needed)

HOW TO USE:
1. Compile C:   gcc bank.c -o bank
2. Start server: python server.py
3. Open browser: http://localhost:8080
"""

import http.server
import subprocess
import json
import os
import urllib.parse
from http.server import HTTPServer, BaseHTTPRequestHandler

PORT = 8080
BANK_EXE = "bank.exe"  # Windows executable

class BankHandler(BaseHTTPRequestHandler):

    def log_message(self, format, *args):
        # Cleaner logging
        print(f"  {args[0]} {args[1]}")

    def send_json(self, data, status=200):
        body = json.dumps(data).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", len(body))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(body)

    def send_file(self, path, mime):
        try:
            with open(path, "rb") as f:
                content = f.read()
            self.send_response(200)
            self.send_header("Content-Type", mime)
            self.send_header("Content-Length", len(content))
            self.end_headers()
            self.wfile.write(content)
        except FileNotFoundError:
            self.send_response(404)
            self.end_headers()

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def do_GET(self):
        path = self.path.split("?")[0]

        # Serve static files
        if path == "/" or path == "/index.html":
            self.send_file("index.html", "text/html")
        elif path == "/style.css":
            self.send_file("style.css", "text/css")
        elif path == "/app.js":
            self.send_file("app.js", "application/javascript")
        else:
            self.send_response(404)
            self.end_headers()

    def do_POST(self):
        # Read body
        length = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(length)

        try:
            data = json.loads(body)
        except:
            self.send_json({"ok": False, "msg": "Invalid JSON"}, 400)
            return

        path = self.path

        # Check if bank.exe exists
        exe = BANK_EXE
        if not os.path.exists(exe):
            self.send_json({"ok": False, "msg": "bank.exe not found. Run: gcc bank.c -o bank.exe"})
            return

        # Build command args based on API endpoint
        args = self.build_args(path, data)
        if args is None:
            self.send_json({"ok": False, "msg": f"Unknown endpoint: {path}"}, 404)
            return

        # Run the C program
        try:
            result = subprocess.run(
                [exe] + args,
                capture_output=True,
                text=True,
                timeout=5
            )
            output = result.stdout.strip()

            # Parse JSON output from C program
            try:
                response = json.loads(output)
            except:
                response = {"ok": False, "msg": f"C program error: {output or result.stderr}"}

            self.send_json(response)

        except subprocess.TimeoutExpired:
            self.send_json({"ok": False, "msg": "Request timed out"})
        except Exception as e:
            self.send_json({"ok": False, "msg": str(e)})

    def build_args(self, path, data):
        """Map API endpoints to C program command-line arguments"""

        if path == "/api/login":
            return ["login", str(data.get("acc_no", "")), data.get("password", "")]

        elif path == "/api/create":
            name = data.get("name", "").replace(" ", "_")  # spaces not allowed in args
            return ["create", name, str(data.get("acc_no", "")), data.get("password", "")]

        elif path == "/api/deposit":
            return ["deposit", str(data.get("acc_no", "")),
                    data.get("password", ""), str(data.get("amount", 0))]

        elif path == "/api/withdraw":
            return ["withdraw", str(data.get("acc_no", "")),
                    data.get("password", ""), str(data.get("amount", 0))]

        elif path == "/api/transfer":
            return ["transfer", str(data.get("acc_no", "")), data.get("password", ""),
                    str(data.get("to_acc", "")), str(data.get("amount", 0))]

        elif path == "/api/balance":
            return ["balance", str(data.get("acc_no", "")), data.get("password", "")]

        elif path == "/api/history":
            return ["history", str(data.get("acc_no", "")), data.get("password", "")]

        elif path == "/api/update":
            new_name = data.get("new_name", "").replace(" ", "_")
            return ["update", str(data.get("acc_no", "")), data.get("password", ""),
                    new_name, data.get("new_password", "")]

        elif path == "/api/delete":
            return ["delete", str(data.get("acc_no", "")), data.get("password", "")]

        elif path == "/api/admin":
            return ["admin", data.get("admin_password", "")]

        return None


if __name__ == "__main__":
    print("=" * 50)
    print("  NovaBanc Server")
    print("=" * 50)
    print(f"\n  1. Compile C program:  gcc bank.c -o bank")
    print(f"  2. Server running at:  http://localhost:{PORT}")
    print(f"\n  Press Ctrl+C to stop\n")

    server = HTTPServer(("localhost", PORT), BankHandler)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n\n  Server stopped.")