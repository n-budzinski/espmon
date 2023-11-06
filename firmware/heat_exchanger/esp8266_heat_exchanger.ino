//Sensors
#include <OneWire.h>
#include <DallasTemperature.h>

//WiFi client
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;

//GPIO4
#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// Number of temperature devices found
int numberOfDevices;
// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress; 

char *string_append(char *s1, char *s2);
void printAddress(DeviceAddress deviceAddress);
void sensor_data();

char ssid[] = "GosiaP";
char password[] = "akacja78";

unsigned long previousMillis = 0;
const long interval = 5000; //millis

void setup() {
  Serial.begin(9600);

  // Start up the library
  sensors.begin();
  //resolution to fix the latency
  sensors.setResolution(11);
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  connectToWiFi();

  // Loop through each device, print out address (DEBUG)
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
     // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      String sensor_data = "http://192.168.0.114/set_values";
      sensors.requestTemperatures();
      if(numberOfDevices>0){
        for(int i=0;i<numberOfDevices; i++){
          // Search the wire for address
          if(sensors.getAddress(tempDeviceAddress, i)){
            if(i==0){
              sensor_data+="?";
            }
            else{
              sensor_data+="&";
            }
            float temp = sensors.getTempC(tempDeviceAddress);
            sensor_data += getAddressString(tempDeviceAddress) + "=" + temp;
          }
        }
        sensor_data += "}";
      }
      httpGETRequest(sensor_data.c_str());
      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
      connectToWiFi();
    }
  }
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
  http.begin(client, serverName);
  http.GET();
  http.end();
}

String getAddressString(DeviceAddress deviceAddress) {
  String temp = "";
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) temp+= String(0, HEX);
      temp += String(deviceAddress[i], HEX);
  }
  Serial.print(temp);
  return temp;
}

void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

char *string_append(char *s1, char *s2)
{
  int str1_len = strlen(s1);
  int str2_len = strlen(s2);
  int size = str1_len + str2_len + 1;
  char *s = (char*)calloc(size, sizeof(char));
  for(int i=0; i<str1_len; i++)
  {
      s[i] = s1[i];
  }
  for(int i=0; i<str2_len; i++)
  {
      s[str1_len + i] = s2[i];
  }
  s[size - 1] = '\0';
  return s;
}