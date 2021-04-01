#include <Arduino.h>
#include <WiFi.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define ONE_WIRE_BUS 2
#define ADC_PIN 4  
#define voltage_divider_offset 2.3050

const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "server";
const char* mqtt_user = "user";
const char* mqtt_key = "key";
const char* outTopicGroup = "topic";
const unsigned long long updateInterval = 60ULL * 60 * 1000000;  // e.g. 15 * 60 * 1000000; for a 15-Min update interval (15-mins x 60-secs * 1000000uS)

WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

bool setup_wifi() 
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.begin(ssid, password);
  Serial.println("Starting WiFi");
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED) 
  {
    if (attempts > 50)
    {
      return false;
    }

    delay(200);
    Serial.print(".");
    attempts++;
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  return true;
}

float readBattery()
{
  long sum = 0;
  float voltage = 0.0;
    
  for (int i = 0; i < 500; i++)
  {
    sum += analogRead(ADC_PIN);
    delayMicroseconds(1000);
  }

  voltage = sum / (float)500;
  voltage = (voltage / 4096.0) * 3.3;
  return voltage;
}

float readTemperature()
{
  sensor.begin();
  sensor.requestTemperatures();
  return roundf(sensor.getTempFByIndex(0)* 100) / 100;
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Start");

  float tempF = readTemperature();
  float voltage = readBattery();
  
  Serial.print("Pool Temp F: ");
  Serial.println(tempF);

  float unadjustedVoltage = roundf((voltage * 2) * 100) / 100;
  float adjustedVoltage = roundf((voltage * voltage_divider_offset) * 100) / 100;

  Serial.print("Raw Battery Voltage: ");
  Serial.println(unadjustedVoltage); 

  Serial.print("Adjusted Battery Voltage: ");
  Serial.println(adjustedVoltage); 

  if(voltage > 1 && tempF > 1)
  {
    if (setup_wifi())
    {
      client.setServer(mqtt_server, 1883);

      String clientId = "ESP32-PoolTempMon-";
      clientId += String(random(0xffff), HEX);

      Serial.println();
      Serial.print("Connecting to mqtt...");

      if (client.connect(clientId.c_str(), mqtt_user, mqtt_key)) {
        Serial.println("done");

        DynamicJsonDocument doc(256);

        JsonObject feeds  = doc.createNestedObject("feeds");
        feeds["temp"] = tempF;
        feeds["battery"] = adjustedVoltage;
        feeds["rawbattery"] = unadjustedVoltage;

        char buffer[256];
        size_t n = serializeJson(doc, buffer);

        if (client.publish(outTopicGroup, buffer, n)) {
          Serial.println("Group Published");
        }
        else {
          Serial.println("Group Publishing Failed");
        }
      }
    }
    
  }
  else
  {
    Serial.println("Temp or Voltage were not found");
  }
  
  Serial.print("Going to sleep...");
  esp_sleep_enable_timer_wakeup(updateInterval); // Deep-Sleep time in microseconds
  esp_deep_sleep_start();
}

void loop() {
}
