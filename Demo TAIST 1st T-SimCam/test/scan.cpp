#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5;  //In seconds
BLEScan *pBLEScan;
bool flag = false;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    std::string dev_name = advertisedDevice.getName().c_str();
    int8_t rssi = advertisedDevice.getRSSI();
    if (dev_name.find("AIS M5StickC") != std::string::npos) {
      BLEDevice::getScan()->stop();
      if (rssi > -65){
        Serial.printf("Advertised Device: %s, rssi: %d\n", dev_name.c_str(), rssi);
      } else {
        Serial.println("Device is too far");
      }
    } 
  }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(false, false);
  // Serial.print("Devices found: ");
  // Serial.println(foundDevices.getCount());
  // Serial.println("Scan done!");
  pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory
  delay(2000);
}