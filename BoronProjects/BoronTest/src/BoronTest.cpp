#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

const char* serviceUuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* characteristicUuid = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

BleUuid serviceUuidObj(serviceUuid);
BleUuid characteristicUuidObj(characteristicUuid);

BleCharacteristic peerCharacteristic;
BlePeerDevice peer;

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer) {
  String receivedValue((const char*)data, len);
  Serial.println("Received Value: " + receivedValue);
  // Respond to the received value if needed
  peerCharacteristic.setValue("ACK from Boron");
}

void setup() {
  Serial.begin(115200);
  BLE.on();
  peerCharacteristic.onDataReceived(onDataReceived);
  BLE.addCharacteristic(peerCharacteristic);
  peerCharacteristic.setProperties(BLERead | BLEWrite | BLENotify);
  peerCharacteristic.setUuid(characteristicUuidObj);

  BLE.scanForUuid(serviceUuidObj);
}

void loop() {
  if (BLE.connected()) {
    if (peer.connected() == false) {
      peer = BLE.connectedPeer();
      Serial.println("Connected to peer device!");
    }
  }
  else {
    if (peer.connected()) {
      peer.disconnect();
      Serial.println("Disconnected from peer device!");
      BLE.scanForUuid(serviceUuidObj);
    }
  }
}