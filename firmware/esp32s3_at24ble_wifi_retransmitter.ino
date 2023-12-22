//WiFi client
#include <WiFi.h>
//#include <ESPmDNS.h>
//#include <WiFiUdp.h>
#include "BLEDevice.h"
#include <HTTPClient.h>

//BLE
#include "BLEDevice.h"
#define bleServerName "AT24_BLE"

static BLEUUID srvUUID("0000ffe0-0000-1000-0000-00805f9b34fb");
static BLEUUID chrUUID("0000ffe1-0000-1000-0000-00805f9b34fb");
static String BLEMAC = "79:80:18:8e:2f:30";

static BLEAddress* Server_BLE_Address;
String Scanned_BLE_Address;
static BLEAdvertisedDevice* meter;
static BLERemoteCharacteristic* remoteChr;
static boolean doConnect = false;
static boolean connected = false;
String output;

char ssid[] = "GosiaP";
char password[] = "akacja78";
unsigned long previousMillis = 0;
const long interval = 5000;  //millis

static void chrCB(BLERemoteCharacteristic* remoteChr, uint8_t* pData, size_t length, bool isNotify) {
  unsigned long currentMillis = millis();
  output = "?";
  output.reserve(length);
  for (int i = 0; i < length; i++) {
    output += "&" + pData[i];
  }
  Serial.println(*pData);
  if (currentMillis - previousMillis >= interval) {
    String payload = "http://192.168.0.114/set_values";
    httpGETRequest(output.c_str());
    previousMillis = currentMillis;
  }
}

class deviceCB : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.println(advertisedDevice.getName().c_str());
    Server_BLE_Address = new BLEAddress(advertisedDevice.getAddress());
    Scanned_BLE_Address = Server_BLE_Address->toString().c_str();
    if (Scanned_BLE_Address == BLEMAC) {
      BLEDevice::getScan()->stop();
      meter = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

class ClientCB : public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {
    Serial.println("Connected to the device");
  }
  void onDisconnect(BLEClient* pClient) {
    connected = false;
  }
};

bool connectToMeter() {
  BLEClient* pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new ClientCB());
  Serial.println("Connecting to AT24_BLE");
  pClient->connect(meter);

  BLERemoteService* pRemoteService = pClient->getService(srvUUID);
  if (pRemoteService == nullptr) {
    pClient->disconnect();
    return false;
  }

  remoteChr = pRemoteService->getCharacteristic(chrUUID);
  if (remoteChr == nullptr) {
    pClient->disconnect();
    return false;
  }
  remoteChr->registerForNotify(chrCB);
  return true;
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected.");
  }
}

void httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, serverName+output);
  http.GET();
  http.end();
}

void setup() {
  Serial.begin(9600);
  connectToWiFi();
}

void loop() {
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new deviceCB());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, true);
  while (true) {
    if (doConnect && !connected) {
      connected = connectToMeter();
    }
    delay(1000);
  }
}