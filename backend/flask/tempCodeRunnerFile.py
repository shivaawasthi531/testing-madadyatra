from flask import Flask, request, jsonify, send_from_directory
import subprocess
import os

app = Flask(__name__)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
CPP_DIR = os.path.join(BASE_DIR, "../cpp")
BUS_ENGINE_PATH = os.path.join(CPP_DIR, "bus_engine.exe")
METRO_ENGINE_PATH = os.path.join(CPP_DIR, "metro_engine.exe")

FRONTEND_DIR = os.path.abspath(
    os.path.join(BASE_DIR, "../../frontend")
)

# ================= HOME =================
@app.route("/")
def home():
    return send_from_directory(FRONTEND_DIR, "index.html")

# ================= STATIC FILES =================
@app.route("/<path:path>")
def static_files(path):
    return send_from_directory(FRONTEND_DIR, path)

# ================= BUS ROUTE API (unchanged) =================
@app.route("/bus-route", methods=["POST"])
def bus_route():
    data = request.json
    start = data.get("start")
    end = data.get("end")

    try:
        result = subprocess.run(
            [BUS_ENGINE_PATH, start, end],
            cwd=CPP_DIR,
            capture_output=True,
            text=True
        )

        output = result.stdout.strip()

        if "No bus found" in output or output == "":
            return jsonify({"error": "No bus found for this route"})

        return jsonify({
            "bus": "DTC",
            "raw": output
        })

    except Exception as e:
        return jsonify({"error": str(e)})

# ================= METRO ROUTE API (fixed for spaces) =================
@app.route("/metro-route", methods=["POST"])
def metro_route():
    data = request.json
    start = data.get("start")
    end = data.get("end")

    if not start or not end:
        return jsonify({"error": "Start and End cannot be empty"})

    try:
        # Windows exe + spaces handling
        cmd = f'"{METRO_ENGINE_PATH}" "{start}" "{end}"'
        result = subprocess.run(
            cmd,
            cwd=CPP_DIR,
            capture_output=True,
            text=True,
            shell=True   # Important for arguments with spaces
        )

        output = result.stdout.strip()

        if "No metro found" in output or output == "":
            return jsonify({"error": "No metro found for this route"})

        # Split lines to return as array for frontend
        lines = output.splitlines()
        route_lines = [line for line in lines]

        return jsonify({
            "metro": "Delhi Metro",
            "raw": output,
            "lines": route_lines
        })

    except Exception as e:
        return jsonify({"error": str(e)})

# ================= RUN =================
if __name__ == "__main__":
    app.run(debug=True)
