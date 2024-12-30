#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Adafruit_AHTX0.h>
#include <ScioSense_ENS160.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>

#define FAN_PIN D5
#define Motor_PIN D6

Adafruit_AHTX0 aht;
ScioSense_ENS160 ens160(ENS160_I2CADDR_1);
LiquidCrystal_I2C lcd(0x27,16,2);
Servo servo;

int pos = 0;    // variable to store the servo position

int tempC = 0;    // To store the temperature in C
int humidity; // To store the humidity*
boolean fanState = false;
boolean windowState = false;
boolean lastWindow = false;

IPAddress local_IP(10,43,170,212);
IPAddress gateway(10,43,170,1);
IPAddress subnet(255, 255, 255, 0);

const char *ssid = "Projet";
const char *password = "projet-porjet";

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
        if(strcmp((char *)payload, "on") == 0)
        {
            fanState = true;
        }
        else if(strcmp((char *)payload, "off") == 0)
        {
            fanState = false;
        }
        if(strcmp((char *)payload, "open") == 0)
        {
            windowState = true;
        }
        else if(strcmp((char *)payload, "close") == 0)
        {
            windowState = false;
        }
        break;
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(FAN_PIN, OUTPUT);
    servo.attach(Motor_PIN);
    analogWrite(FAN_PIN, 0);
    lcd.init();
    lcd.backlight();
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
    // ens160.begin();
    // Serial.println(ens160.available() ? "done." : "failed!");
    // if (ens160.available())
    // {
    //     Serial.print("\tRev: ");
    //     Serial.print(ens160.getMajorRev());

    //     Serial.print(".");
    //     Serial.print(ens160.getMinorRev());

    //     Serial.print(".");
    //     Serial.println(ens160.getBuild());

    //     Serial.print("\tStandard mode ");

    //     Serial.println(ens160.setMode(ENS160_OPMODE_STD) ? "done." : "failed!");
    // }
    // if (!aht.begin())
    // {

    //     Serial.println("Could not find AHT? Check wiring");
    // }

    // Serial.println("AHT10 or AHT20 found");
}

void loop()
{
    webSocket.loop();
    // sensors_event_t humidity1, temp;
    // aht.getEvent(&humidity1, &temp);
    // tempC = (temp.temperature);
    // humidity = (humidity1.relative_humidity);
    // Serial.print("Temperature: ");
    // Serial.print(tempC);
    // Serial.println(" degrees °C");
    if(fanState)
    {
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Fan is ON");
        analogWrite(FAN_PIN, 255);
    }
    else
    {
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Fan is OFF");
        analogWrite(FAN_PIN, 0);
    }
    if (tempC > 30)
    {
        analogWrite(FAN_PIN, 255);
    }
    else
    {
        analogWrite(FAN_PIN, 0);
    }
    // tempC+=1;
    // webSocket.sendTXT(0, (String(tempC)).c_str());
    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.print(tempC);
    
    if(windowState != lastWindow)
    {
        for (pos = 0; pos <= 90; pos += 1) // goes from 0 degrees to 180 degrees
        {                                  // in steps of 1 degree
            servo.write(pos);              // tell servo to go to position in variable 'pos'
            delay(15);                       // waits 15ms for the servo to reach the position
        }
    }
    else
    {
        for (pos = 90; pos >= 0; pos -= 1) // goes from 180 degrees to 0 degrees
        {
            servo.write(pos);              // tell servo to go to position in variable 'pos'
            delay(15);                       // waits 15ms for the servo to reach the position
        }
    }
    // Serial.println(tempC);
    // if (ens160.available())
    // {
    //     ens160.set_envdata(tempC, humidity);
    //     ens160.measure(true);
    //     ens160.measureRaw(true);

    //     Serial.print("AQI: ");
    //     Serial.print(ens160.getAQI());
    //     Serial.print("\t");

    //     Serial.print("TVOC: ");
    //     Serial.print(ens160.getTVOC());
    //     Serial.print("ppb\t");

    //     Serial.print("eCO2: ");
    //     Serial.print(ens160.geteCO2());
    //     Serial.println("ppm\t");
    // }
    lastWindow = windowState;
    delay(100);
}
