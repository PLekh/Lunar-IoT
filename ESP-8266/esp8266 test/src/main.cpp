#include <ESP8266WiFi.h>
#include <espnow.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Digital pin connected to the DHT sensor
#define DHTPIN 5    

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// Define variables to store DHT readings
float temperature;
float humidity;

// Define variables to store incoming readings
float incomingTemp;
float incomingHum;

// Updates DHT readings every 10 seconds
const long interval = 10000; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, incomingData, len);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  
  // Parse incoming JSON
  incomingTemp = doc["temperature"];
  incomingHum = doc["humidity"];
  
  //printIncomingReadings();
  Serial.println("INCOMING READINGS");
  Serial.print("Temperature: ");
  Serial.print(incomingTemp);
  Serial.println(" ºC");
  Serial.print("Humidity: ");
  Serial.print(incomingHum);
  Serial.println(" %");
}

void getReadings(){
  // Read Temperature
  temperature = dht.readTemperature();
  if (isnan(temperature)){
    Serial.println("Failed to read from DHT");
    temperature = 0.0;
  }
  humidity = dht.readHumidity();
  if (isnan(humidity)){
    Serial.println("Failed to read from DHT");
    humidity = 0.0;
  }
}

/*void printIncomingReadings(){
  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("Temperature: ");
  Serial.print(incomingTemp);
  Serial.println(" ºC");
  Serial.print("Humidity: ");
  Serial.print(incomingHum);
  Serial.println(" %");
}*/

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Init DHT sensor
  dht.begin();
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Register for Send Callback to get the status of transmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for Receive Callback
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    // Get DHT readings
    getReadings();

    // Create JSON object
    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;

    // Serialize JSON to buffer
    char buffer[200];
    memset(buffer, 0, sizeof(buffer)); // Clear buffer
    size_t len = serializeJson(doc, buffer, sizeof(buffer));

    // Send JSON message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *)buffer, len);

    // Print outgoing readings
    Serial.println("OUTGOING READINGS");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" ºC");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }
}
