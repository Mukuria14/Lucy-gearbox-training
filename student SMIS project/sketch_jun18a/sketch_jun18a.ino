#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <LEAmDNS.h>
#include <StreamString.h>

#ifndef STASSID
#define STASSID "-mukush"
#define STAPSK "qwertyui"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

WebServer server(80);

void handleRoot() {
  static int cnt = 0;
  int sec = millis() / 1000;
  int hr = sec / 3600;
  int min = (sec / 60) % 60;
  sec = sec % 60;

  StreamString temp;
  temp.reserve(500); // Preallocate a large chunk to avoid memory fragmentation
  temp.printf("<!DOCTYPE html>\
<html lang='en'>\
<head>\
  <meta charset='UTF-8' />\
  <meta name='viewport' content='width=device-width, initial-scale=1.0' />\
  <title>Attendance · joined & missed</title>\
  <style>\
    * {\
      margin: 0;\
      padding: 0;\
      box-sizing: border-box;\
      font-family: system-ui, -apple-system, 'Segoe UI', Roboto, 'Helvetica Neue', sans-serif;\
    }\
\
    body {\
      min-height: 100vh;\
      background: linear-gradient(145deg, #f6f9fc 0%, #e9f0f5 100%);\
      display: flex;\
      justify-content: center;\
      align-items: center;\
      padding: 1.5rem;\
    }\
\
    .card {\
      max-width: 780px;\
      width: 100%;\
      background: rgba(255, 255, 255, 0.8);\
      backdrop-filter: blur(8px);\
      -webkit-backdrop-filter: blur(8px);\
      background: #ffffff;\
      border-radius: 2.5rem;\
      padding: 2rem 2rem 2.2rem;\
      box-shadow: 0 20px 40px -12px rgba(0, 20, 30, 0.25),\
                  0 8px 24px -6px rgba(0, 0, 0, 0.05);\
      transition: box-shadow 0.2s ease;\
      border: 1px solid rgba(255, 255, 255, 0.5);\
    }\
\
    h1 {\
      font-size: 2rem;\
      font-weight: 600;\
      letter-spacing: -0.01em;\
      color: #0b2a3b;\
      display: flex;\
      align-items: center;\
      gap: 0.4rem;\
      flex-wrap: wrap;\
      margin-bottom: 0.25rem;\
    }\
\
    .subhead {\
      color: #3e5a6b;\
      font-weight: 400;\
      font-size: 1rem;\
      border-left: 3px solid #7aa5b9;\
      padding-left: 0.75rem;\
      margin-left: 0.2rem;\
    }\
\
    .summary-grid {\
      display: flex;\
      flex-wrap: wrap;\
      justify-content: space-between;\
      gap: 0.75rem 1rem;\
      background: #f2f7fb;\
      padding: 0.9rem 1.5rem;\
      border-radius: 3rem;\
      margin: 1.2rem 0 1.8rem 0;\
      border: 1px solid #dbe7ef;\
    }\
\
    .stat-item {\
      display: flex;\
      align-items: baseline;\
      gap: 0.4rem;\
      font-size: 1rem;\
      color: #1e3b4b;\
    }\
\
    .stat-item span:first-child {\
      font-weight: 450;\
      color: #2e4d5f;\
    }\
\
    .stat-number {\
      font-weight: 700;\
      font-size: 1.4rem;\
      color: #0a293b;\
      letter-spacing: 0.3px;\
      min-width: 2rem;\
      display: inline-block;\
      text-align: center;\
    }\
\
    .stat-number.present-num { color: #1f7b4d; }\
    .stat-number.absent-num { color: #b13e3e; }\
\
    .student-list {\
      display: flex;\
      flex-direction: column;\
      gap: 0.65rem;\
      margin: 1.8rem 0 2rem 0;\
    }\
\
    .student-row {\
      display: flex;\
      align-items: center;\
      justify-content: space-between;\
      background: #fafdff;\
      padding: 0.65rem 1.2rem 0.65rem 1.8rem;\
      border-radius: 60px;\
      border: 1px solid #e2edf5;\
      transition: background 0.15s, border-color 0.15s;\
      box-shadow: 0 2px 4px rgba(0,0,0,0.02);\
    }\
\
    .student-row:hover {\
      background: #ffffff;\
      border-color: #b8d0dd;\
      box-shadow: 0 6px 12px -8px rgba(0, 40, 60, 0.12);\
    }\
\
    .student-name {\
      font-weight: 500;\
      font-size: 1.05rem;\
      color: #14303e;\
      letter-spacing: -0.2px;\
      display: flex;\
      align-items: center;\
      gap: 0.6rem;\
    }\
\
    .student-name .status-badge {\
      font-size: 0.7rem;\
      font-weight: 600;\
      padding: 0.2rem 0.6rem;\
      border-radius: 40px;\
      background: #eef4f9;\
      color: #2b4d5f;\
      letter-spacing: 0.3px;\
      text-transform: uppercase;\
    }\
\
    .badge-joined {\
      background: #d7f0e4;\
      color: #116a44;\
    }\
\
    .badge-missed {\
      background: #fce3e3;\
      color: #b13e3e;\
    }\
\
    .action-group {\
      display: flex;\
      gap: 0.5rem;\
      align-items: center;\
    }\
\
    .btn {\
      border: none;\
      background: transparent;\
      font-weight: 500;\
      padding: 0.4rem 1rem;\
      border-radius: 40px;\
      font-size: 0.85rem;\
      cursor: pointer;\
      transition: all 0.15s ease;\
      border: 1px solid transparent;\
      display: inline-flex;\
      align-items: center;\
      gap: 0.2rem;\
      background: #eef3f7;\
      color: #1f4053;\
    }\
\
    .btn-joined {\
      background: #1f7b4d;\
      color: white;\
      border-color: #1a6a41;\
    }\
\
    .btn-joined:hover {\
      background: #14663d;\
      transform: scale(0.96);\
    }\
\
    .btn-missed {\
      background: #b13e3e;\
      color: white;\
      border-color: #9e3535;\
    }\
\
    .btn-missed:hover {\
      background: #942e2e;\
      transform: scale(0.96);\
    }\
\
    .btn-outline {\
      background: transparent;\
      border: 1px solid #c6d8e3;\
      color: #2e4d5f;\
    }\
\
    .btn-outline:hover {\
      background: #e6f0f6;\
      border-color: #8eb0c2;\
    }\
\
    .btn:active { transform: scale(0.94); }\
\
    .footer-actions {\
      display: flex;\
      justify-content: flex-end;\
      gap: 0.8rem;\
      margin-top: 0.6rem;\
      flex-wrap: wrap;\
      border-top: 1px solid #deecf3;\
      padding-top: 1.6rem;\
    }\
\
    .btn-reset {\
      background: #eef2f5;\
      color: #234a5c;\
      padding: 0.5rem 1.5rem;\
      border-radius: 40px;\
      font-weight: 500;\
      border: 1px solid #d0dfe9;\
    }\
\
    .btn-reset:hover {\
      background: #e1eaf1;\
      border-color: #b0c9d7;\
    }\
\
    .note {\
      font-size: 0.8rem;\
      color: #4b6d7e;\
      margin-top: 0.4rem;\
      text-align: right;\
      opacity: 0.7;\
    }\
\
    @media (max-width: 550px) {\
      .card { padding: 1.5rem; }\
      .student-row { flex-wrap: wrap; gap: 0.5rem; padding: 0.8rem 1rem; }\
      .action-group { margin-left: auto; }\
      .summary-grid { flex-direction: column; align-items: flex-start; gap: 0.3rem; }\
    }\
  </style>\
</head>\
<body>\
  <div class='card' role='main'>\
    <header>\
      <h1>\
        📋 attendance\
        <span class='subhead'>joined / missed</span>\
      </h1>\
    </header>\
\
    <div class='summary-grid' id='summaryGrid'>\
      <div class='stat-item'><span>👥 total</span> <span class='stat-number' id='totalCount'>5</span></div>\
      <div class='stat-item'><span>✅ joined</span> <span class='stat-number present-num' id='joinedCount'>0</span></div>\
      <div class='stat-item'><span>❌ missed</span> <span class='stat-number absent-num' id='missedCount'>0</span></div>\
    </div>\
\
    <div class='student-list' id='studentList'></div>\
\
    <div class='footer-actions'>\
      <button class='btn btn-reset' id='resetAllBtn'>↺ reset all</button>\
      <span class='note'>mark each student as joined or missed</span>\
    </div>\
  </div>\
\
  <script>\
    (function() {\
      const STUDENT_NAMES = [\
        'lucy wahu',\
        'ivy wanjiru',\
        'jasmine mukuria',\
        'rahab wangongu',\
        'samuel ngugi'\
      ];\
\
      let students = [];\
\
      const studentListEl = document.getElementById('studentList');\
      const totalCountEl = document.getElementById('totalCount');\
      const joinedCountEl = document.getElementById('joinedCount');\
      const missedCountEl = document.getElementById('missedCount');\
      const resetBtn = document.getElementById('resetAllBtn');\
\
      function updateSummary() {\
        const total = students.length;\
        const joined = students.filter(s => s.status === 'joined').length;\
        const missed = total - joined;\
\
        totalCountEl.textContent = total;\
        joinedCountEl.textContent = joined;\
        missedCountEl.textContent = missed;\
      }\
\
      function render() {\
        let html = '';\
        students.forEach((student, index) => {\
          const isJoined = student.status === 'joined';\
          const badgeClass = isJoined ? 'badge-joined' : 'badge-missed';\
          const badgeText = isJoined ? 'joined' : 'missed';\
\
          html += `\
            <div class='student-row' data-index='${index}'>\
              <div class='student-name'>\
                ${student.name}\
                <span class='status-badge ${badgeClass}'>${badgeText}</span>\
              </div>\
              <div class='action-group'>\
                <button class='btn btn-joined' data-action='setJoined' data-index='${index}'>✅ joined</button>\
                <button class='btn btn-missed' data-action='setMissed' data-index='${index}'>❌ missed</button>\
              </div>\
            </div>\
          `;\
        });\
\
        studentListEl.innerHTML = html;\
        updateSummary();\
\
        document.querySelectorAll('[data-action='setJoined']').forEach(btn => {\
          btn.addEventListener('click', function(e) {\
            const idx = parseInt(this.getAttribute('data-index'), 10);\
            if (!isNaN(idx) && students[idx]) {\
              students[idx].status = 'joined';\
              render();\
            }\
          });\
        });\
\
        document.querySelectorAll('[data-action='setMissed']').forEach(btn => {\
          btn.addEventListener('click', function(e) {\
            const idx = parseInt(this.getAttribute('data-index'), 10);\
            if (!isNaN(idx) && students[idx]) {\
              students[idx].status = 'missed';\
              render();\
            }\
          });\
        });\
      }\
\
      function resetAllToMissed() {\
        students = STUDENT_NAMES.map(name => ({\
          name: name,\
          status: 'missed'\
        }));\
        render();\
      }\
\
      function init() {\
        students = STUDENT_NAMES.map(name => ({\
          name: name,\
          status: 'missed'\
        }));\
        render();\
\
        resetBtn.addEventListener('click', function() {\
          resetAllToMissed();\
        });\
      }\
\
      init();\
    })();\
  </script>\
</body>\
</html>", hr, min, sec, rp2040.getFreeHeap(), ++cnt);
  server.send(200, "text/html", temp);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

if (MDNS.begin("picow")) {
        Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  MDNS.update();
}
