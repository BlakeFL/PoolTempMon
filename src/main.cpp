#include <Arduino.h>
#include <WiFi.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

#define GREENLED 5
#define ONE_WIRE_BUS 2
#define ADC_PIN 4  
#define voltage_divider_offset 2.174 
#define calibration .99
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 3600   

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP_ADDRESS";

WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  pinMode(GREENLED, OUTPUT);
  Serial.println("Start");

  Serial.println("Dallas Temperature Control Library Demo - TwoPin_DS18B20");

  //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  sensor.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  //digitalWrite(GREENLED, HIGH);
  //Serial.println("The green led is ON");
  //delay(4000);
  //digitalWrite(GREENLED, LOW);
  //Serial.println("The green led is OFF");
  //delay(4000);
  Serial.print("Requesting temperatures...");
  sensor.requestTemperatures();
  Serial.println(" done");

  Serial.print("Pool Temp: ");
  Serial.println(sensor.getTempCByIndex(0));

  float unadjusted_voltage = (analogRead(ADC_PIN) / 4095.0 * 3.3 * voltage_divider_offset) * calibration;
  Serial.println("Voltage = " + String(unadjusted_voltage,3) + "v");
  Serial.println();

  delay(1000);
}

