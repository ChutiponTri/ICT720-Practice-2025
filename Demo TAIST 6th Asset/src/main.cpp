#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include <M5StickC.h>

#define name0 "Asset-T6"
#define name1 "Asset-T7"
#define name2 "Asset-T8"

void show_qr_code(String name, String url);

void setup(void) {
  // init HW
  M5.begin();
  M5.Lcd.begin();
  
  // init BLE
  BLEDevice::init(name0);
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  show_qr_code("TON-M5", "www.24av.net");
}

void loop(void) {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    // change broadcast name
    BLEDevice::stopAdvertising();
    esp_ble_gap_set_device_name(name1);
    BLEDevice::startAdvertising(); 
    show_qr_code("TON-M6", "https://www.youtube.com/watch?v=dQw4w9WgXcQ");
  }
  if (M5.BtnB.wasPressed()) {
    // change broadcast name
    BLEDevice::stopAdvertising();
    esp_ble_gap_set_device_name(name2);
    BLEDevice::startAdvertising(); 
    show_qr_code("TON-M7", "https://www.youtube.com/watch?v=9BalEldzE8o");
  }
  delay(100);
}

void show_qr_code(String name, String url) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(5, 5);
  M5.Lcd.println(name);
  uint16_t sz = 0;
  M5.Lcd.qrcode(url, 0, 80, 80);
}
