from flask import Flask, request, jsonify, send_from_directory
import subprocess
import os

app = Flask(__name__)

# ================= PATH SETUP =================
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# cpp folder (Linux compatible)
CPP_DIR = os.path.abspath(os.path.join(BASE_DIR, "../cpp"))

# Linux binaries (NO .exe on Render)
BUS_ENGINE_PATH = os.path.join(CPP_DIR, "bus_engine")
METRO_ENGINE_PATH = os.path.join(CPP_DIR, "metro_engine")

# frontend folder
FRONTEND_DIR = os.path.abspath(os.path.join(BASE_DIR, "../../frontend"))

# CSV files
STATION_CSV = os.path.join(CPP_DIR, "metro_stations.csv")
LINE_CSV    = os.path.join(CPP_DIR, "metro_lines.csv")

# ================= HOME =================
@app.route("/")
def home():
    return send_from_directory(FRONTEND_DIR, "index.html")

# ================= STATIC FILES =================
@app.route("/<path:path>")
def static_files(path):
    return send_from_directory(FRONTEND_DIR, path)

# ================= BUS ROUTE API =================
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

# ================= METRO ROUTE API =================
@app.route("/metro-route", methods=["POST"])
def metro_route():
    data = request.json
    start = data.get("start")
    end = data.get("end")

    if not start or not end:
        return jsonify({"error": "Start and End cannot be empty"})

    try:
        cmd = [
            METRO_ENGINE_PATH,
            start,
            end,
            STATION_CSV,
            LINE_CSV
        ]

        result = subprocess.run(
            cmd,
            cwd=CPP_DIR,
            capture_output=True,
            text=True
        )

        output = result.stdout.strip()

        if "No metro found" in output or output == "":
            return jsonify({"error": "No metro found for this route"})

        return jsonify({
            "metro": "Delhi Metro",
            "raw": output,
            "lines": output.splitlines()
        })

    except Exception as e:
        return jsonify({"error": str(e)})

# ================= RUN (RENDER COMPATIBLE) =================
if __name__ == "__main__":
    port = int(os.environ.get("PORT", 10000))
    app.run(host="0.0.0.0", port=port)

