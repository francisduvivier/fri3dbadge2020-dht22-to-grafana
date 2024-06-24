# 1 "/tmp/tmpzu810tsp"
#include <Arduino.h>
# 1 "/home/fduvivier/psnl/gitProjects/fri3dbadge2020-dht22-to-grafana/DHT22_MQTT_Fri3d_Badge/src/DHT22_MQTT_Fri3d_Badge.ino"
#include <WiFi.h>
#include <SPI.h>
#include <ArduinoOTA.h>
#include "arduino_secrets.h"
#include <PubSubClient.h>
#include <AsyncTimer.h>
#include <Badge2020_TFT.h>
#include "DHT.h"

#define MQTT_SERVER "192.168.0.5"
#define MQTT_SERVERPORT 1883
#define MSG_BUFFER_SIZE (50)

AsyncTimer timer;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiClient wifiClient;

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

Badge2020_TFT tft;

#define DHT_PIN 27
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);
const int16_t SCREEN_WIDTH = 240;
const int16_t SCREEN_HEIGHT = 240;
const int16_t BACKGROUND_COLOR = ST77XX_BLACK;
#define feedPrefix "fri3dbadge_fd"
#define concat(first,second) first second
#define IR_PIN 25
const String mqqt_debug_topic = concat(feedPrefix, "debug");
unsigned long startTime;
bool firstCycleCompleted = false;
struct HTValues
{
  float humidity;
  float temperature;
};
void setTop(uint16_t color);
void setup(void);
void loop();
void setupTFT();
void setupDHT();
void setupOTA();
HTValues updateScreen();
void twoSecondsLoop();
void centerHorizontal(const String &buf, uint16_t y);
void centerHorizontalExt(const String &buf, uint16_t y, uint8_t size, uint16_t color);
void centerHorizontalOverWrite(const String &buf, uint16_t y);
void centerHorizontalOverWriteExt(const String &buf, uint16_t y, uint8_t size, uint16_t color);
void overWriteExt(const String &buf, uint16_t x, uint16_t y);
void overWrite(const String &buf);
void setupWifi();
void printWifiStatus();
void setupTimer();
void setupMQTT();
void reconnectWifi();
void mqttReconnect();
void mqttDebugLog(String msg);
void callback(char *topic, byte *payload, unsigned int length);
#line 43 "/home/fduvivier/psnl/gitProjects/fri3dbadge2020-dht22-to-grafana/DHT22_MQTT_Fri3d_Badge/src/DHT22_MQTT_Fri3d_Badge.ino"
void setTop(uint16_t color)
{
  int thickness = 0;
  for (int row = 0; row < 2; row++)
  {
    for (int col = 0; col < SCREEN_WIDTH; col++)
    {
      tft.drawPixel(col, row, color);
    }
  }
}
uint16_t redColor = tft.color565(255, 0, 0);
uint16_t greenColor = tft.color565(0, 255, 0);
uint16_t blueColor = tft.color565(0, 0, 255);
uint16_t purpleColor = tft.color565(255, 0, 255);

void setup(void)
{
  Serial.begin(115200);
  setupTFT();
  tft.fillScreen(BACKGROUND_COLOR);
  setTop(blueColor);
  setupDHT();
  setTop(blueColor);
  updateScreen();
  setupWifi();
  setTop(blueColor);
  setupMQTT();
  setTop(blueColor);
  setupOTA();
  setTop(blueColor);
  printWifiStatus();
  setTop(blueColor);
  setupTimer();
  startTime = millis();
  tft.fillScreen(BACKGROUND_COLOR);
  twoSecondsLoop();
}
void loop()
{
  setTop(redColor);
  reconnectWifi();
  if (!firstCycleCompleted)
  {
    ArduinoOTA.handle();
    timer.handle();
    setTop(redColor);
    if (millis() - startTime > 1000)
    {
      firstCycleCompleted = true;




      setTop(greenColor);

    }
  }
  else
  {

    if (!client.connected())
    {
      setTop(purpleColor);
      mqttReconnect();
      setTop(redColor);
    }
    twoSecondsLoop();

    setTop(greenColor);

  }
}
void setupTFT()
{
  Serial.println("START setupTFT!");
  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.setRotation(2);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  Serial.println("END setupTFT!");
}
void setupDHT()
{
  Serial.println("START setupDHT!");
  dht.begin();
  Serial.println("END setupDHT!");
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

HTValues updateScreen()
{
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  overWriteExt("IP: ", 0, 5);
  tft.println(ip);

  Serial.println("Before DHT Read!");


  float h = dht.readHumidity();

  float t = dht.readTemperature();

  float f = dht.readTemperature(true);

  float hi = dht.computeHeatIndex(t, h, false);

  HTValues values;
  values.humidity = h;
  values.temperature = t;

  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println("Failed to read from DHT sensor!");
    return values;
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

  tft.setTextSize(2);
  currY = 190;
  int currX = 10;
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);

  tft.setCursor(currX, currY);
  overWrite("Temp: ");
  tft.print(t);
  tft.print("C ");

  currY += 30;
  tft.setCursor(currX, currY);
  overWrite("Heat: ");
  tft.print(hi);
  tft.println("C");

  return values;
}

void twoSecondsLoop()
{
  HTValues sensorValues = updateScreen();
  client.publish(String("fri3dbadge1/dht22/humidity").c_str(), String(sensorValues.humidity).c_str());
  client.publish("fri3dbadge1/dht22/temp", String(sensorValues.temperature).c_str());
}



void centerHorizontal(const String &buf, uint16_t y)
{
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &size[0], &size[1]);
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
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &size[0], &size[1]);
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
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &size[0], &size[1]);
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

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  overWriteExt("SSID: ", 10, 100);
  tft.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  overWriteExt("IP Address: ", 10, 130);
  tft.println(ip);


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

    String clientId = "ESP32Client-Fri3d-Badge";

    if (client.connect(clientId.c_str()))
    {

      mqttDebugLog("connected");
      centerHorizontalOverWriteExt("MQTT connected", 30, 2, ST77XX_YELLOW);
    }
    else
    {
      centerHorizontalOverWriteExt("MQTT Fail", 30, 2, ST77XX_YELLOW);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 500 millis");

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