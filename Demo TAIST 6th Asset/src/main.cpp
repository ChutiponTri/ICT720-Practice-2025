#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include <M5StickC.h>

void show_qr_code(String name);

void setup(void) {
  // init HW
  M5.begin();
  M5.Lcd.begin();
  
  // init BLE
  BLEDevice::init("Ton-M5StickC-0");
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  show_qr_code("Ton-M5StickC-0");
}

void loop(void) {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    // change broadcast name
    BLEDevice::stopAdvertising();
    esp_ble_gap_set_device_name("Ton-M5StickC-1");
    BLEDevice::startAdvertising(); 
    show_qr_code("Ton-M5StickC-1");
  }
  if (M5.BtnB.wasPressed()) {
    // change broadcast name
    BLEDevice::stopAdvertising();
    esp_ble_gap_set_device_name("Ton-M5StickC-2");
    BLEDevice::startAdvertising(); 
    show_qr_code("Ton-M5StickC-2");
  }
  delay(100);
}

void show_qr_code(String name) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(5, 5);
  M5.Lcd.println(name);
  uint16_t sz = 0;
  M5.Lcd.qrcode(name, 0, 80, 80);
}

