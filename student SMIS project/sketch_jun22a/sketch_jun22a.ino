#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <MFRC522.h>

// ─── WiFi credentials ───────────────────────────────
#define SSID     "-mukush"
#define PASSWORD "qwertyui"

// ─── RC522 pins ─────────────────────────────────────
#define SS_PIN  17
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN);
WebServer server(80);

// ─── Student database ───────────────────────────────
// STEP: Replace these UIDs with the ones you wrote down
struct Student {
  const char* name;
  byte uid[4];
  bool present;
};

Student students[] = {
  { "Lucy Wahu",       {0xA1, 0x55, 0xC9, 0x1B}, false }, // ← real UID from DumpInfo
  { "Ivy Wanjiru",     {0x5E, 0x52, 0x4C, 0x59}, false }, // ← dummy for now
  { "Jasmine Mukuria", {0x4C, 0x5B, 0x53, 0x49}, false }, // ← dummy for now
  { "Rahab Wangongu",  {0x00, 0x00, 0x00, 0x03}, false }, // ← dummy for now
  { "Samuel Ngugi",    {0x00, 0x00, 0x00, 0x04}, false }  // ← dummy for now
};

const int STUDENT_COUNT = 5;

// ─── Match scanned UID to a student ─────────────────
int findStudent(byte* scannedUID) {
  for (int i = 0; i < STUDENT_COUNT; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (students[i].uid[j] != scannedUID[j]) {
        match = false;
        break;
      }
    }
    if (match) return i; // returns the index of matched student
  }
  return -1; // -1 means no match found
}

// ─── Build the attendance webpage ───────────────────
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='5'>"; // auto-refresh every 5 seconds
  html += "<title>Attendance</title>";
  html += "<style>";
  html += "body { font-family: sans-serif; padding: 2rem; background: #f0f4f8; }";
  html += ".card { background: white; border-radius: 1rem; padding: 2rem; max-width: 600px; margin: auto; box-shadow: 0 4px 20px rgba(0,0,0,0.1); }";
  html += "h1 { color: #0b2a3b; margin-bottom: 1rem; }";
  html += ".row { display: flex; justify-content: space-between; align-items: center; padding: 0.75rem 1rem; border-radius: 2rem; margin-bottom: 0.5rem; border: 1px solid #e0e0e0; }";
  html += ".present { background: #d7f0e4; border-color: #a3d9be; }";
  html += ".absent { background: #fce3e3; border-color: #f0b8b8; }";
  html += ".badge { padding: 0.2rem 0.8rem; border-radius: 1rem; font-size: 0.8rem; font-weight: 600; }";
  html += ".badge-present { background: #1f7b4d; color: white; }";
  html += ".badge-absent { background: #b13e3e; color: white; }";
  html += ".summary { display: flex; gap: 1rem; margin-bottom: 1.5rem; font-size: 1.1rem; }";
  html += "</style></head><body><div class='card'>";
  html += "<h1>📋 Attendance</h1>";

  // Count present students
  int presentCount = 0;
  for (int i = 0; i < STUDENT_COUNT; i++) {
    if (students[i].present) presentCount++;
  }

  html += "<div class='summary'>";
  html += "<span>👥 Total: <strong>" + String(STUDENT_COUNT) + "</strong></span>";
  html += "<span>✅ Present: <strong>" + String(presentCount) + "</strong></span>";
  html += "<span>❌ Absent: <strong>" + String(STUDENT_COUNT - presentCount) + "</strong></span>";
  html += "</div>";

  // List each student
  for (int i = 0; i < STUDENT_COUNT; i++) {
    String rowClass = students[i].present ? "present" : "absent";
    String badgeClass = students[i].present ? "badge-present" : "badge-absent";
    String badgeText = students[i].present ? "Present" : "Absent";

    html += "<div class='row " + rowClass + "'>";
    html += "<span>" + String(students[i].name) + "</span>";
    html += "<span class='badge " + badgeClass + "'>" + badgeText + "</span>";
    html += "</div>";
  }

  html += "<p style='margin-top:1rem; font-size:0.8rem; color:#888;'>Page refreshes every 5 seconds</p>";
  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

// ─── Setup ───────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  Serial.println("RFID reader ready. Tap a card...");

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected! Open this address in your browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started");
}

// ─── Loop ────────────────────────────────────────────
void loop() {
  server.handleClient(); // keep web server running

  // Check if a card is present
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Read the UID
  byte* scannedUID = rfid.uid.uidByte;

  // Print UID to Serial Monitor
  Serial.print("Card tapped. UID: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(scannedUID[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Find which student this card belongs to
  int index = findStudent(scannedUID);

  if (index >= 0) {
    students[index].present = true;
    Serial.print("✅ Marked present: ");
    Serial.println(students[index].name);
  } else {
    Serial.println("⚠️ Unknown card - not in database");
  }

  rfid.PICC_HaltA(); // stop reading this card until it's removed and tapped again
}
