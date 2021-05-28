#include <Arduino.h>
#include <WiFi.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define ONE_WIRE_BUS 2
#define ADC_PIN 4  
#define voltage_divider_offset 2.10

const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "io.adafruit.com";
const char* mqtt_user = "mqttUser";
const char* mqtt_key = "mqttKey";
const char* outTopicGroup = "groupTopic";
const unsigned long long updateInterval = 60ULL * 60 * 1000000;  // e.g. 60 * 60 * 1000000; for a 1 hour update interval (60-mins x 60-secs * 1000000uS)
bool readyToSleep = false;
unsigned long previousMillis = 0; // last time update
unsigned long currentMillis = 0;
int publishAttempts = 0;
int connectionAttempts = 0;

WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.println("Published Message Recieved");
  readyToSleep = true;
}

void publish(float temp, float adjustedVoltage, float unadjustedVoltage)
{
  publishAttempts++;

  previousMillis = currentMillis;

  Serial.print("Group publish attempt ");
  Serial.print(publishAttempts);
  Serial.println("...");

  DynamicJsonDocument doc(256);

  JsonObject feeds  = doc.createNestedObject("feeds");
  feeds["temp"] = temp;
  feeds["battery"] = adjustedVoltage;
  feeds["rawbattery"] = unadjustedVoltage;
  feeds["connect"] = (connectionAttempts * (float)500) / 1000;
  feeds["publishattempts"] = publishAttempts;

  char buffer[256];
  size_t n = serializeJson(doc, buffer);

  if (client.publish(outTopicGroup, buffer, n)) {
    Serial.println("Group Published");
  }
  else {
    Serial.println("Group Publishing Failed");
  }
}

bool setup_wifi() 
{
  delay(100);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Starting WiFi");

  while (WiFi.status() != WL_CONNECTED) 
  {
    if (connectionAttempts > 25)
    {
      return false;
    }

    delay(500);
    Serial.print(".");
    connectionAttempts++;
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
  delay(500);

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

  if (setup_wifi())
  {
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    String clientId = "ESP32-PoolTempMon-";
    clientId += String(random(0xffff), HEX);

    Serial.println();
    Serial.print("Connecting to mqtt...");

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_key)) {
      Serial.println("done");

      if(client.subscribe(outTopicGroup))
      { 
         Serial.println("Subscribed");
      }
      else
      {
         Serial.println("Subscribe failed");
      }

      delay(500);
      currentMillis = millis();
      publish(tempF, adjustedVoltage, unadjustedVoltage);
      Serial.print("Waiting for response...");
    }
  }
  else
  {
    readyToSleep = true;
  }

  while(readyToSleep == false)
  {
    currentMillis = millis();
    if (publishAttempts > 3)
    {
      break;
    }

    if((currentMillis - previousMillis) > 2000)
    {
      //Try to publish again
      publish(tempF, adjustedVoltage, unadjustedVoltage);
      Serial.print("Waiting for response...");
    }

    client.loop();
    delay(200);
  }
  
  client.disconnect();
  delay(200);
  Serial.print("Going to sleep...");
  esp_sleep_enable_timer_wakeup(updateInterval); // Deep-Sleep time in microseconds
  esp_deep_sleep_start();
}

void loop() {
}
