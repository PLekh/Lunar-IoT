#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEClient.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "SR04.h"

#define TRIG_PIN 18
#define ECHO_PIN 19
SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);

#define DHTPIN 25

#define DHTTYPE    DHT11

DHT dht(DHTPIN, DHTTYPE);

float temperature;
float humidity;
float heatindex;
float distance;
float light;

const long interval = 10000;
unsigned long previousMillis = 0;

static BLEUUID serviceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEUUID    charUUID_RX("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEUUID    charUUID_TX("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

static BLERemoteCharacteristic* pRemoteCharacteristic_RX;
static BLERemoteCharacteristic* pRemoteCharacteristic_TX;
static BLEAdvertisedDevice* myDevice;

void getReadings(){
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
  heatindex = dht.computeHeatIndex(temperature, humidity, false);

  distance = sr04.Distance();

  light = analogRead(36);
}

static void notifyCallback(
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

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {
  }

  void onDisconnect(BLEClient* pClient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    pClient->connect(myDevice);
    Serial.println(" - Connected to server");

    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");

    pRemoteCharacteristic_RX = pRemoteService->getCharacteristic(charUUID_RX);
    if (pRemoteCharacteristic_RX == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID_RX.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our RX characteristic");

    pRemoteCharacteristic_TX = pRemoteService->getCharacteristic(charUUID_TX);
    if (pRemoteCharacteristic_TX == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID_TX.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our TX characteristic");

    if(pRemoteCharacteristic_TX->canNotify())
      pRemoteCharacteristic_TX->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {
      Serial.print("Found our device!  address: ");
      advertisedDevice.getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  digitalWrite(5,HIGH);
  pinMode(33, OUTPUT);
  digitalWrite(33,HIGH);
  pinMode(32, OUTPUT);
  digitalWrite(32,LOW);
  Serial.println("Starting Arduino BLE Client application...");
  //pinMode(36, INPUT);
  BLEDevice::init("");

  dht.begin();

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
  previousMillis = currentMillis;
  getReadings();

  StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["heatindex"] = heatindex;
    doc["distance"] = distance;
    doc["light"] = light;
  
  char buffer[200];
  size_t len = serializeJson(doc, buffer);

  if (doConnect) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  if (connected) {
    if (Serial.available()) {
      //std::string newValue = Serial.readString().c_str();
      //pRemoteCharacteristic_RX->writeValue(newValue.c_str(), newValue.length());
      std::string newValue = buffer + "\n";
      pRemoteCharacteristic_RX->writeValue(newValue.c_str(), newValue.length());
    }
  } else if(doScan) {
    BLEDevice::getScan()->start(0); 
  }

  }
}
