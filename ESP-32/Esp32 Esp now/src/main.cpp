#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

SoftwareSerial softSerial(19,18);
//StaticJsonDocument<200> doc;
// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
/*typedef struct struct_message {
    char message[250];
} struct_message;
*/
// Create a struct_message called outgoingMessage to hold the message to send

//struct_message outgoingMessage;

// Create a struct_message to hold incoming message
//struct_message incomingMessage;

float incomingTemp;
float incomingHum;
float incomingHix;
float incomingdis;

esp_now_peer_info_t peerInfo;

const long interval = 10000;
unsigned long previousMillis = 0; 

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  //memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  //Serial.print("Message received: ");
  //Serial.println(incomingMessage.message);

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
  incomingHix = doc["heatindex"];
  incomingdis = doc["distance"];

  Serial.println("Temperature: ");
  Serial.print(incomingTemp);
  Serial.println("C");
  Serial.println("Humidity: ");
  Serial.print(incomingHum);
  Serial.println("%");
  Serial.println("Heat Index: ");
  Serial.print(incomingHix);
  Serial.println("C");
  Serial.println("Distance: ");
  Serial.print(incomingdis);
  Serial.println("cm");
  //doc["random"] = 5;
  //printIncomingReadings();
}



void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(23,OUTPUT);

  softSerial.begin(9600);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register for Send CB to get the status of transmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Read the message from Serial Monitor
    //String message = Serial.readString();
    //message.trim();

    // Convert the message to char array
    //message.toCharArray(outgoingMessage.message, 250);

    // Send message via ESP-NOW

    StaticJsonDocument<200> doc;
    doc["light"] = analogRead(36);
    
    // Serialize JSON to buffer
    
    char buffer[200];
    size_t len = serializeJson(doc, buffer);

    // Send JSON message via ESP-NOW
    //esp_now_send(broadcastAddress, (uint8_t *)buffer, len);
  
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) buffer, len);
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
  
  
  
}
softSerial.print(incomingdis);
//Serial.println("Serial send value");
//Serial.print(incomingdis);
if(int(incomingHix) >= 23){
    digitalWrite(23,HIGH);
  }
  else{
    digitalWrite(23,LOW);
  }
}