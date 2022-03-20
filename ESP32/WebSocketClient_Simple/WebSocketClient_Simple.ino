#include <WiFi.h>
// #include <WebSocketsServer.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

// Constants
const char* ssid = "INFINITUME957";
const char* password = "3672360107";
// JS Server - Local IP address. Different than ESP32
const char* address = "192.168.1.92";
const char* entryPoint = "/";
const uint16_t port = 80;

// Globals
// WebSocketsServer webSocket = WebSocketsServer(80);
WebSocketsClient webSocket;


// Called when receiving any WebSocket message
/*
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {
*/
void onWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  // Figure out the type of WebSocket event
  switch(type) {


    /*
    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;

      // Echo text message back to client
      case WStype_TEXT:
        Serial.printf("[%u] Text: %s\n", num, payload);
        webSocket.sendTXT(num, payload);
        break;

      */
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
      webSocket.sendTXT("Connected");
      break;

     case WStype_TEXT:
        Serial.printf("[WSc] get text: %s\n", payload);
        break;


    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void setup() {

  // Start Serial port
  Serial.begin(115200);

  // Connect to access point
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }

  // Print our IP address
  Serial.println("Connected!");
  Serial.print("My IP address: ");
  Serial.println(WiFi.localIP());

  // Start WebSocket server and assign callback
  webSocket.begin(address, port, entryPoint);
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {

  // Look for and handle WebSocket data
  webSocket.loop();
}
