#include <esp_now.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEClient.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

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

static BLEUUID serviceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEUUID    charUUID_RX("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
//static BLEUUID    charUUID_TX("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

static BLERemoteCharacteristic* pRemoteCharacteristic_RX;
//static BLERemoteCharacteristic* pRemoteCharacteristic_TX;
static BLEAdvertisedDevice* myDevice;

esp_now_peer_info_t peerInfo;

const long interval = 10000;
unsigned long previousMillis = 0; 

// Callback when data is sent
/*void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
*/
// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  //memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  //Serial.print("Message received: ");
  //Serial.println(incomingMessage.message);

  //StaticJsonDocument<200> doc;

  char jsonString[len + 1];
  memcpy(jsonString, incomingData, len);
  jsonString[len] = '\0';

  if(connected){

    pRemoteCharacteristic_RX->writeValue(jsonString, len);

  }else if(doScan){
    BLEDevice::getScan()->start(0);
  }

  
  /*if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  */
  
  // Parse incoming JSON
  /*incomingTemp = doc["temperature"];
  incomingHum = doc["humidity"];
  incomingHix = doc["heatindex"];
  */
  //doc["random"] = 5;
  //printIncomingReadings();
}

/*static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.write(pData, length);
    Serial.println();
}
*/

/*class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {
  }

  void onDisconnect(BLEClient* pClient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};
*/

bool connectToServer() {
    //Serial.print("Forming a connection to ");
    //Serial.println(myDevice->getAddress().toString().c_str());

    BLEClient*  pClient  = BLEDevice::createClient();
    //Serial.println(" - Created client");

    //pClient->setClientCallbacks(new MyClientCallback());

    pClient->connect(myDevice);
    //Serial.println(" - Connected to server");

    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    /*if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    */
    //Serial.println(" - Found our service");

    pRemoteCharacteristic_RX = pRemoteService->getCharacteristic(charUUID_RX);
    /*if (pRemoteCharacteristic_RX == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID_RX.toString().c_str());
      pClient->disconnect();
      return false;
    }
    */
    //Serial.println(" - Found our RX characteristic");

    //pRemoteCharacteristic_TX = pRemoteService->getCharacteristic(charUUID_TX);
    /*if (pRemoteCharacteristic_TX == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID_TX.toString().c_str());
      pClient->disconnect();
      return false;
    }
    */
    //Serial.println(" - Found our TX characteristic");

    /*if(pRemoteCharacteristic_TX->canNotify())
      pRemoteCharacteristic_TX->registerForNotify(notifyCallback);
    */
    connected = true;
    return true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.print("BLE Advertised Device found: ");
    //Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {
      //Serial.print("Found our device!  address: ");
      advertisedDevice.getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }
  }
};

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  //pinMode(23,OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  //Serial.println("Starting Arduino BLE Client application...");

  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  // Init ESP-NOW
  /*if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  */

  // Register for Send CB to get the status of transmitted packet
  //esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  /*if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  */

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    //if (doConnect) {
    if (connectToServer()) {
      //Serial.println("We are now connected to the BLE Server.");
    //} else {
    //  Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    //}
    doConnect = false;
  }
  

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Read the message from Serial Monitor
    //String message = Serial.readString();
    //message.trim();

    // Convert the message to char array
    //message.toCharArray(outgoingMessage.message, 250);

    // Send message via ESP-NOW

    /*StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["heatindex"] = heatindex;
    */

    // Serialize JSON to buffer

    // Send JSON message via ESP-NOW
    //esp_now_send(broadcastAddress, (uint8_t *)buffer, len);
  
    //esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) buffer, len);
    /*if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
  */
  /*
  if(int(incomingHix) >= 23){
    digitalWrite(23,HIGH);
  }
  else{
    digitalWrite(23,LOW);
  }
  */
}
}