# ELEE 2045 Assignment 5: Smart Voice Assistant & Cloud Web Dashboard
<img src="images/headshot.jpg" alt="My Headshot" width="200"/>
**Name:** Arman Aryafar

---

## 🌐 Live Deployment Link
**Dashboard URL:** https://imbecile-spew-crusader.ngrok-free.dev/?key=1234

> This link is publicly accessible and used for grading. Authorization is required via API key.

---

## 🎥 Video Demonstration
Link: (insert your YouTube link here)

> Demo shows full system: M5Stick → backend → web dashboard sync

---

## 🚀 Project Description
This project is a Smart Voice Assistant built using an M5StickCPlus2, a Python Flask backend, and a cloud-hosted web dashboard.

The system allows users to:
- Record voice notes using the M5Stick
- Upload audio to a cloud server
- Automatically transcribe speech into text using OpenAI Whisper
- Convert transcriptions into actionable Todo items
- View and manage todos on both the web dashboard and embedded device

Key features:
- Real-time voice-to-text conversion
- Bidirectional sync between hardware and web UI
- Persistent storage using SQLite database
- Clean and responsive web interface

---

## ☁️ Cloud Deployment & Security Architecture
The backend is deployed using a monolithic architecture and exposed to the public internet via ngrok tunneling.

### Deployment:
- Local Flask server running on port 5001
- ngrok used to create a public HTTPS endpoint
- Web dashboard served directly from Flask templates

### Security:
- All API endpoints are protected using an API key
- Requests must include header: x-api-key: 1234
- Unauthorized requests return HTTP 401

---

## 🛠️ How to Build & Run Locally

Installation:
pip install flask openai-whisper %$%

Run Server:
python server.py %$%

Start Public Tunnel:
ngrok http 5001 %$%

Notes:
- Ensure port matches Flask app (5001)
- Copy ngrok HTTPS URL into M5Stick code
- Keep terminal running for deployment

---

## 📡 M5Stick Functionality

Controls:
- Button A → Record + upload voice
- Hold Button B → Mark todo as done
- Tap Button B → Cycle through todos

Features:
- Audio recorded at 16kHz mono
- Sent to backend and transcribed
- Display shows current todo + index
- Text wrapping enabled for readability

---

## ✨ Creative Extension — AI Voice Automation

This project integrates AI-powered speech recognition using Whisper.

Enhancement:
- Voice input automatically becomes Todo items
- Removes need for manual typing
- Optimized for embedded device interaction

Additionally:
- Voice notes stored with audio files
- Enables playback and verification

---

## ✅ System Summary

Pipeline:
M5Stick → WiFi → Flask API → Whisper → SQLite → Web UI + M5 Display

Features demonstrated:
- Embedded systems integration
- Cloud API deployment
- AI transcription
- Secure endpoints
- Full-stack sync

---

## ⚠️ Important Notes for Grading

- Access dashboard using:
  https://imbecile-spew-crusader.ngrok-free.dev/?key=1234

- Backend must be running (Flask + ngrok)

- Demo includes:
  - Adding todos
  - Voice recording
  - Auto transcription
  - Sync
  - Deletion