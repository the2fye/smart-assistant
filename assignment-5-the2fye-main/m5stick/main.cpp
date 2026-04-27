#include <M5StickCPlus2.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// WIFI
const char* ssid = "WhiteSky-PublicWiFi";
const char* password = "";

// NGROK
const char* server = "https://imbecile-spew-crusader.ngrok-free.dev";

// API KEY
const char* apiKey = "1234";

// tracking
int currentIndex = 0;
int currentTodoId = -1;

// audio
const int MAX_SAMPLES = 48000;
std::vector<int16_t> audioSamples;

// ---------- MIC SETUP ----------
void setupMic() {
    auto micConfig = M5.Mic.config();
    micConfig.sample_rate = 16000;
    micConfig.stereo = false;
    M5.Mic.config(micConfig);
    M5.Mic.begin();
}

// ---------- RECORD ----------
bool recordAudio() {
    audioSamples.clear();
    audioSamples.resize(MAX_SAMPLES, 0);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("RECORDING...");
    M5.Lcd.println("Hold B to cancel");

    M5.Mic.record(audioSamples.data(), MAX_SAMPLES, 16000);

    while (M5.Mic.isRecording()) {
        M5.update();

        // cancel if B held
        if (M5.BtnB.pressedFor(400)) {
            M5.Mic.end();
            M5.Mic.begin();  // restart mic so next recording works
            M5.Lcd.fillScreen(BLACK);
            M5.Lcd.println("Cancelled");
            delay(800);
            fetchTodos();
            return false;
        }

        delay(10);
    }

    M5.Lcd.println("Done: " + String(audioSamples.size()) + " samples");
    return true;
}

// ---------- WIFI ----------
void connectWiFi() {
    WiFi.begin(ssid, password);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.println("Connecting...");
    M5.Lcd.setTextSize(2);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }

    M5.Lcd.println("\nConnected!");
}

// ---------- UPLOAD ----------
void uploadAudio() {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, String(server) + "/audio");

    http.addHeader("Content-Type", "application/octet-stream");
    http.addHeader("x-api-key", apiKey);

    int code = http.POST(
        (uint8_t*)audioSamples.data(),
        audioSamples.size() * sizeof(int16_t)
    );

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.println(code == 200 ? "Uploaded!" : "Upload Error");

    http.end();

    delay(800);
    currentIndex = 0;
    fetchTodos();
}

// ---------- TODOS ----------
void fetchTodos() {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, String(server) + "/todos");
    http.addHeader("x-api-key", apiKey);   // 🔥 ADD THIS
    int code = http.GET();

    M5.Lcd.fillScreen(BLACK);

    if (code == 200) {
        String payload = http.getString();
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, payload);

        if (doc.size() > 0) {
            if (currentIndex >= (int)doc.size()) currentIndex = 0;

            if (doc.size() > 0) {
    if (currentIndex >= (int)doc.size()) currentIndex = 0;

    // 🔥 PASTE NEW BLOCK HERE
    currentTodoId = doc[currentIndex][0];
    int total = doc.size();
    const char* task = doc[currentIndex][1];

    // ---------- CLEAN UI ----------
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextWrap(true);

    // header
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("[%d/%d]\n\n", currentIndex + 1, total);

    String fullTask = String(task);

    String category = "";
    if (fullTask.startsWith("[")) {
        int endIdx = fullTask.indexOf("]");
        if (endIdx != -1) {
            category = fullTask.substring(1, endIdx);
            fullTask = fullTask.substring(endIdx + 2);
        }
    }

    if (category != "") {
        M5.Lcd.setTextSize(2);
        M5.Lcd.println(category);
        M5.Lcd.println();
    }

    M5.Lcd.setTextSize(2);

    int maxCharsPerLine = 18;

    while (fullTask.length() > 0) {
        String line = fullTask.substring(0, maxCharsPerLine);
        M5.Lcd.println(line);
        fullTask = fullTask.substring(maxCharsPerLine);
    }
}
        } else {
            M5.Lcd.println("No Todos");
        }
    }

    http.end();
}

// ---------- DELETE ----------
void markDone() {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, String(server) + "/done");

    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", apiKey);

    String body = "{\"id\":" + String(currentTodoId) + "}";
    http.POST(body);

    http.end();
}

// ---------- SETUP ----------
void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Lcd.setRotation(1);


    connectWiFi();
    setupMic();
    fetchTodos();
}

// ---------- LOOP ----------
void loop() {
    M5.update();

    // BUTTON A → RECORD + UPLOAD
    if (M5.BtnA.wasPressed()) {
        if (recordAudio()) {
            uploadAudio();
        }
        return;
    }

    // HOLD B → DELETE
    if (M5.BtnB.pressedFor(800)) {
        markDone();
        fetchTodos();
        // wait for release to prevent re-trigger
        while (M5.BtnB.isPressed()) {
            M5.update();
            delay(10);
        }
        return;
    }

    // SHORT PRESS B → NEXT TODO
    if (M5.BtnB.wasReleased() && !M5.BtnB.pressedFor(800)) {
        currentIndex++;
        fetchTodos();
    }
}