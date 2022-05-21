#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define SDA 13
#define SCL 14
// WiFi
const char *ssid = "***";     // Enter your WiFi name
const char *password = "***"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "***";
const char *topic = "esp32/test";
const char *mqtt_username = "emqx";
const char *mqtt_password = "123456789";
const int mqtt_port = 1883;

// LCD
byte heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000};
byte human[8] = {
    0b00000,
    0b01110,
    0b01010,
    0b01110,
    0b00100,
    0b00100,
    0b01010,
    0b10001};
LiquidCrystal_I2C lcd(0x27, 16, 2);
int lcdColumns = 16;
int lcdRows = 2;
String messageStatic = "It's MernsWorld!";
String messageScroll = "please help im stuck in here";

// WIFI
WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
    // Set software serial baud to 115200;
    Serial.begin(115200);
    // connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
    // connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected())
    {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
        {
            Serial.println("Public emqx mqtt broker connected");
        }
        else
        {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    // LCD setup

    Wire.begin(SDA, SCL);
    lcd.init();
    lcd.backlight();
    client.publish(topic, "This is ESP32 speaking");
    client.subscribe(topic);
    delay(500);
    lcd.createChar(1, heart);
    lcd.createChar(0, human);
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void scrollText(int row, String message, int delayTime, int lcdColumns)
{
    for (int i = 0; i < lcdColumns; i++)
    {
        message = " " + message;
    }
    message = message + " ";
    for (int pos = 0; pos < message.length(); pos++)
    {
        lcd.setCursor(0, row);
        lcd.print(message.substring(pos, pos + lcdColumns));
        delay(delayTime);
    }
}

void loop()
{
    lcd.setCursor(0, 0);
    lcd.print(messageStatic);
    scrollText(1, messageScroll, 900, lcdColumns);
    client.loop();
}