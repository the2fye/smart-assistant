from flask import Flask, request, jsonify, render_template, send_from_directory
import sqlite3
import os
import struct
import time
# import whisper

app = Flask(__name__)

API_KEY = "1234"
AUDIO_FOLDER = "audio"
# ensure audio folder exists
os.makedirs(AUDIO_FOLDER, exist_ok=True)


# ---------- DATABASE ----------
def init_db():
    conn = sqlite3.connect("database.db")
    c = conn.cursor()

    c.execute("""
        CREATE TABLE IF NOT EXISTS todos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task TEXT
        )
    """)

    c.execute("""
        CREATE TABLE IF NOT EXISTS notes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            text TEXT,
            file TEXT
        )
    """)

    conn.commit()
    conn.close()

init_db()

# ---------- AUTH ----------
def authorized(req):
    return req.headers.get("x-api-key") == API_KEY

# ---------- HOME (PROTECTED) ----------
@app.route("/")
def home():
    key = request.args.get("key")

    if key != API_KEY:
        return "Unauthorized", 401

    return render_template("index.html")

# ---------- TODOS ----------
@app.route("/todos", methods=["GET"])
def get_todos():
    if not authorized(request):
        return "Unauthorized", 401

    conn = sqlite3.connect("database.db")
    c = conn.cursor()
    c.execute("SELECT * FROM todos")
    data = c.fetchall()
    conn.close()
    return jsonify(data)

@app.route("/todos", methods=["POST"])
def add_todo():
    if not authorized(request):
        return "Unauthorized", 401

    data = request.get_json(force=True)
    task = data.get("task")

    conn = sqlite3.connect("database.db")
    c = conn.cursor()
    c.execute("INSERT INTO todos (task) VALUES (?)", (task,))
    conn.commit()
    conn.close()

    return {"status": "added", "task": task}

@app.route("/done", methods=["POST"])
def done():
    if not authorized(request):
        return "Unauthorized", 401

    data = request.get_json(force=True)
    todo_id = data.get("id")

    conn = sqlite3.connect("database.db")
    c = conn.cursor()

    # get task text
    c.execute("SELECT task FROM todos WHERE id=?", (todo_id,))
    row = c.fetchone()

    if row:
        task_text = row[0]

        # delete todo
        c.execute("DELETE FROM todos WHERE id=?", (todo_id,))

        # delete matching note
        c.execute("DELETE FROM notes WHERE text=?", (task_text,))

    conn.commit()
    conn.close()

    return {"status": "deleted", "id": todo_id}

# ---------- AUDIO UPLOAD + TRANSCRIPTION ----------
@app.route("/audio", methods=["POST"])
def upload_audio():
    if not authorized(request):
        return "Unauthorized", 401

    raw_audio = request.data

    if not raw_audio:
        return {"error": "no data"}, 400

    filename = f"audio_{int(time.time())}.wav"
    path = os.path.join(AUDIO_FOLDER, filename)

    # WAV SETTINGS (match ESP32)
    sample_rate = 16000
    num_channels = 1
    bits_per_sample = 16

    byte_rate = sample_rate * num_channels * bits_per_sample // 8
    block_align = num_channels * bits_per_sample // 8
    data_size = len(raw_audio)

    # WAV HEADER
    wav_header = struct.pack(
        '<4sI4s4sIHHIIHH4sI',
        b'RIFF',
        36 + data_size,
        b'WAVE',
        b'fmt ',
        16,
        1,
        num_channels,
        sample_rate,
        byte_rate,
        block_align,
        bits_per_sample,
        b'data',
        data_size
    )

    # SAVE FILE
    with open(path, "wb") as f:
        f.write(wav_header + raw_audio)

    # ---------- TRANSCRIBE ----------
    text = "Voice note task"

    print("TRANSCRIBED:", text)

    # ---------- SAVE TO NOTES + TODOS ----------
    conn = sqlite3.connect("database.db")
    c = conn.cursor()

    # save to notes
    c.execute(
        "INSERT INTO notes (text, file) VALUES (?, ?)",
        (text, filename)
    )

    # also add to todos
    if text:
        c.execute(
            "INSERT INTO todos (task) VALUES (?)",
            (text,)
        )

    conn.commit()
    conn.close()

    return {"status": "saved", "text": text, "file": filename}

# ---------- SERVE AUDIO ----------
@app.route("/audio/<filename>")
def get_audio(filename):
    return send_from_directory(AUDIO_FOLDER, filename)

# ---------- NOTES ----------
@app.route("/notes", methods=["GET"])
def get_notes():
    if not authorized(request):
        return "Unauthorized", 401

    conn = sqlite3.connect("database.db")
    c = conn.cursor()
    c.execute("SELECT * FROM notes")
    data = c.fetchall()
    conn.close()
    return jsonify(data)

# ---------- RUN ----------
if __name__ == "__main__":
    import os
    port = int(os.environ.get("PORT", 5001))
    app.run(host="0.0.0.0", port=port)
