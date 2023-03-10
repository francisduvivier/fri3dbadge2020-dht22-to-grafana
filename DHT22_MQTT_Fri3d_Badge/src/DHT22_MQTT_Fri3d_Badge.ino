#include <WiFi.h>
#include <SPI.h>
#include <ArduinoOTA.h>
#include "arduino_secrets.h"
#include <PubSubClient.h>

#include <AsyncTimer.h>
AsyncTimer timer;

#define MQTT_SERVER "192.168.0.5"
#define MQTT_SERVERPORT 1883
#define MSG_BUFFER_SIZE (50)

/////// Wifi Settings ///////
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password
WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

#include <Badge2020_TFT.h>

Badge2020_TFT tft;
#include "DHT.h"

#define DHT_PIN 27     // what pin we're connected to
#define DHT_TYPE DHT22 // DHT 22  (AM2302)
DHT dht(DHT_PIN, DHT_TYPE);
const int16_t SCREEN_WIDTH = 240;
const int16_t SCREEN_HEIGHT = 240;
const int16_t BACKGROUND_COLOR = ST77XX_BLACK;
#define feedPrefix "fri3dbadge1"
#define concat(first, second) first second
#define IR_PIN 25
const String mqqt_debug_topic = concat(feedPrefix, "debug");

void setup(void)
{
  Serial.begin(115200);
  Serial.println("DHTxx test!");
  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.setRotation(2);
  dht.begin();
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  overWriteExt("dht.begin() done", 10, 10);

  // Anything from the Adafruit GFX library can go here, see
  // https://learn.adafruit.com/adafruit-gfx-graphics-library

  // check for the presence of the shield:
  setupWifi();
  overWriteExt("setupWifi done", 10, 40);

  setupMQTT();
  overWriteExt("setupMQTT done", 10, 70);

  // start the WiFi OTA library with internal (flash) based storage
  setupOTA();

  // you're connected now, so print out the status:
  printWifiStatus();
  setupTimer();

  tft.fillScreen(BACKGROUND_COLOR);
  mqttReconnect();
}
void loop()
{
  reconnectWifi();
  ArduinoOTA.handle();
  timer.handle();
}
void setupOTA()
{
  ArduinoOTA.onStart([]()
                     { Serial.println("ArduinoOTA Start"); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nArduinoOTA End"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("ArduinoOTA Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("ArduinoOTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("ArduinoOTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("ArduinoOTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("ArduinoOTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("ArduinoOTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("ArduinoOTA End Failed"); });
  ArduinoOTA.begin();
}

void twoSecondsLoop()
{
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  overWriteExt("IP: ", 0, 5);
  tft.println(ip);

  Serial.println("Before DHT Read!");
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  // Compute heat index
  float hi = dht.computeHeatIndex(t, h, false);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *C");

  int currY = 60;
  centerHorizontalOverWriteExt("Humidity(%):", currY, 2, ST77XX_WHITE);

  currY += 30;
  centerHorizontalOverWriteExt(String(h), currY, 8, ST77XX_WHITE);
  client.publish(String("fri3dbadge1/dht22/humidity").c_str(), String(h).c_str());

  tft.setTextSize(2);
  currY = 190;
  int currX = 10;
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);

  tft.setCursor(currX, currY);
  overWrite("Temp: ");
  tft.print(t);
  tft.print("C ");
  client.publish("fri3dbadge1/dht22/temp", String(t).c_str());

  currY += 30;
  tft.setCursor(currX, currY);
  overWrite("Heat: ");
  tft.print(hi);
  tft.println("C");
}

// Center text helper methods

void centerHorizontal(const String &buf, uint16_t y)
{
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &size[0], &size[1]); // calc width of new string
  tft.setCursor((SCREEN_WIDTH / 2) - size[0] / 2, y);
  tft.print(buf);
}
void centerHorizontalExt(const String &buf, uint16_t y, uint8_t size, uint16_t color)
{
  tft.setTextColor(color);
  tft.setTextSize(size);
  centerHorizontal(buf, y);
}
void centerHorizontalOverWrite(const String &buf, uint16_t y)
{
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &size[0], &size[1]); // calc width of new string
  tft.fillRect(0, y, SCREEN_WIDTH, size[1], BACKGROUND_COLOR);
  tft.setCursor((SCREEN_WIDTH / 2) - size[0] / 2, y);
  tft.print(buf);
}
void centerHorizontalOverWriteExt(const String &buf, uint16_t y, uint8_t size, uint16_t color)
{
  tft.setTextColor(color);
  tft.setTextSize(size);
  centerHorizontalOverWrite(buf, y);
}
void overWriteExt(const String &buf, uint16_t x, uint16_t y)
{
  tft.setCursor(x, y);
  overWrite(buf);
}
void overWrite(const String &buf)
{
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &size[0], &size[1]); // calc width of new string
  tft.fillRect(0, tft.getCursorY(), SCREEN_WIDTH, size[1], BACKGROUND_COLOR);
  tft.print(buf);
}

void setupWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! No OTA Possible...");
    overWriteExt("Connection Failed! No OTA Possible...", 10, 210);
    return;
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  overWriteExt("SSID: ", 10, 100);
  tft.println(WiFi.SSID());
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  overWriteExt("IP Address: ", 10, 130);
  tft.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  overWriteExt("signal strength (RSSI):", 10, 170);
  tft.print(rssi);
  tft.println(" dBm");
}

void setupTimer()
{
  timer.setInterval([]()
                    { twoSecondsLoop(); },
                    2 * 1000);
  timer.setInterval([]()
                    { if (!client.connected()){mqttReconnect();} },
                    5 * 60 * 1000);
}

void setupMQTT()
{
  client.setServer(MQTT_SERVER, MQTT_SERVERPORT);
  client.setCallback(callback);

  centerHorizontalOverWriteExt("MQTT connecting", 30, 2, ST77XX_YELLOW);
  mqttReconnect();
  client.publish("fri3dbadge1/status", "trying to say hi");

  mqttDebugLog("MQTT connect Done");
  centerHorizontalOverWriteExt("MQTT connected", 30, 2, ST77XX_YELLOW);

  mqttDebugLog(String(WiFi.localIP()));
}

void reconnectWifi()
{
  // if WiFi is down, try reconnecting
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
  }
}
void mqttReconnect()
{
  // Loop until we're reconnected (max 3 tries)
  int tries = 0;
  if (client.connected())
  {
    centerHorizontalOverWriteExt("MQTT connected", 30, 2, ST77XX_YELLOW);
  }
  else
  {
    centerHorizontalOverWriteExt("MQTT not conn", 30, 2, ST77XX_YELLOW);
  }
  while (!client.connected() && tries++ < 1)
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-Fri3d-Badge";
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      // Once connected, publish an announcement...
      mqttDebugLog("connected");
      centerHorizontalOverWriteExt("MQTT connected", 30, 2, ST77XX_YELLOW);
    }
    else
    {
      centerHorizontalOverWriteExt("MQTT Fail", 30, 2, ST77XX_YELLOW);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 500 millis");
      // Wait 5 seconds before retrying
      delay(500);
    }
  }
}

void mqttDebugLog(String msg)
{
  Serial.print("mqttDebugLog: ");
  Serial.println(msg);
  client.publish(mqqt_debug_topic.c_str(), msg.c_str());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}