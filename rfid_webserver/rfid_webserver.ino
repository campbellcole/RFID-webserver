#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>

ESP8266WebServer server(80);

WebSocketsServer webSocket(25565);

void setup() {
  // serial
  Serial.begin(115200);

  // wifi
  WiFi.begin("Mangos House", "13away25");
  Serial.println("connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("connected");
  Serial.println(WiFi.localIP());

  //spiffs
  SPIFFS.begin();
  Serial.println("SPIFFS started");

  // websocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("websocket started");

  // server
  server.onNotFound(handleRequest);
  server.begin();
  Serial.println("server started");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t len) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] disconnected\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] %d.%d.%d.%d connected to %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get \"%s\"\n", num, payload);
      break;
  }
}

void handleRequest() {
  String path = server.uri();
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  if (SPIFFS.exists((path + ".gz"))) path += ".gz";
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "file not found");
  }
}

String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void loop() {
  webSocket.loop();
  server.handleClient();
  checkSerial();
}

String uid = "";

void checkSerial() {
  while (0 < Serial.available()) {
    char r = Serial.read();
    if (r == '\n') {
      webSocket.broadcastTXT(uid);
      uid = "";
    } else {
      uid += (char) r;
    }
  }
}
