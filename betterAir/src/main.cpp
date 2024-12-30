/*
 * This ESP8266 NodeMCU code was developed by newbiely.com
 *
 * This ESP8266 NodeMCU code is made available for public use without any restriction
 *
 * For comprehensive instructions and wiring diagrams, please visit:
 * https://newbiely.com/tutorials/esp8266/esp8266-websocket
 */

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Adafruit_AHTX0.h>
#include <ScioSense_ENS160.h>
#include <Wire.h>

#define FAN_PIN D6

Adafruit_AHTX0 aht;
ScioSense_ENS160 ens160(ENS160_I2CADDR_1);

int tempC;    // To store the temperature in C
int humidity; // To store the humidity

IPAddress local_IP(192, 168, 0, 142);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

const char *ssid = "Orange-00c22";
const char *password = "7g24EVY7";

WebSocketsServer webSocket = WebSocketsServer(81); // WebSocket server on port 81

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[%u] Disconnected!\n", num);
        break;
    case WStype_CONNECTED:
    {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
    }
    break;
    case WStype_TEXT:
        Serial.printf("[%u] Received text: %s\n", num, payload);
        // Send a response back to the client
        String echoMessage = "Received:  " + String((char *)payload);
        webSocket.sendTXT(num, echoMessage);

        break;
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(FAN_PIN, OUTPUT);
    analogWrite(FAN_PIN, 0);
    delay(500);
    if (!WiFi.config(local_IP, gateway, subnet))
    {
        Serial.println("STA Failed to configure");
    }
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Initialize WebSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.print("ESP8266 Web Server's IP address: ");
    Serial.println(WiFi.localIP());
    ens160.begin();
    Serial.println(ens160.available() ? "done." : "failed!");
    if (ens160.available())
    {
        Serial.print("\tRev: ");
        Serial.print(ens160.getMajorRev());

        Serial.print(".");
        Serial.print(ens160.getMinorRev());

        Serial.print(".");
        Serial.println(ens160.getBuild());

        Serial.print("\tStandard mode ");

        Serial.println(ens160.setMode(ENS160_OPMODE_STD) ? "done." : "failed!");
    }
    if (!aht.begin())
    {

        Serial.println("Could not find AHT? Check wiring");

        while (1)
            delay(10);
    }

    Serial.println("AHT10 or AHT20 found");
}

void loop()
{
    webSocket.loop();
    sensors_event_t humidity1, temp;
    aht.getEvent(&humidity1, &temp);
    tempC = (temp.temperature);
    humidity = (humidity1.relative_humidity);
    Serial.print("Temperature: ");
    // lcd.setCursor(3,0);
    // lcd.print("Temp: ");
    // lcd.print(tempC);
    Serial.print(tempC);
    Serial.println(" degrees °C");
    delay(1000);

    if (ens160.available())
    {
        ens160.set_envdata(tempC, humidity);
        ens160.measure(true);
        ens160.measureRaw(true);

        Serial.print("AQI: ");
        Serial.print(ens160.getAQI());
        Serial.print("\t");

        Serial.print("TVOC: ");
        Serial.print(ens160.getTVOC());
        Serial.print("ppb\t");

        Serial.print("eCO2: ");
        Serial.print(ens160.geteCO2());
        Serial.println("ppm\t");
    }

    if (tempC > 30)
    {
        analogWrite(FAN_PIN, 255);
    }
    else
    {
        analogWrite(FAN_PIN, 0);
    }
    delay(100);
}
