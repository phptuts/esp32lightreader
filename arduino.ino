#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>  // ESP32-Compatible Servo Library

const char* ssid = "Noisebridge"; 
const char* password = "noisebridge";

WebSocketsServer webSocket = WebSocketsServer(81);

Servo myServo;

const int ldrPin = 34;   // LDR sensor on GPIO 34
const int servoPin = 23; // Servo signal on GPIO 18

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.print("Connected! IP Address: ");
    Serial.println(WiFi.localIP());
    delay(10000);
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    myServo.attach(servoPin, 500, 2500);  // Attach servo (Min pulse: 500µs, Max pulse: 2500µs)
}

void loop() {
    webSocket.loop();

    int lightValue = analogRead(ldrPin); 
    int angle = map(lightValue, 0, 4095, 0, 180); 
    Serial.println(angle);
    myServo.write(angle); // Move servo based on light value

    // Send JSON data over WebSockets
    StaticJsonDocument<200> doc;
    doc["light"] = lightValue;
    doc["angle"] = angle;

    char buffer[256];
    serializeJson(doc, buffer);
    webSocket.broadcastTXT(buffer);

    delay(100);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_CONNECTED) {
        Serial.println("Client connected");
    }
}
