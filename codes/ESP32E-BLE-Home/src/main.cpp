//=======================================================================================================//
// Projet/Tutorial        - DIY Project - Smart Home System using DFRobot Fermion BLE Sensor Beacon 
// Author                 - https://www.hackster.io/maheshyadav216
// Hardware               - Fermion: BLE Sensor Beacon Module, 3.3V USB-TTL Serial Adapter,
//                        - FireBeetle_ESP32_E
// Sensors                - Ambient Light, Soil Moisture, Temperature & Humidity Sensor, Smoke Sensor
// Softare                - VSCode PlatformIO, 
// GitHub Repo of Project - https://github.com/maheshyadav216/Project-Smart-Home-using-BLE-Sensor-Beacon
// Code last Modified on  - 14/03/2024
//=======================================================================================================//

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEEddystoneURL.h>
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))
//Setting up ESP32 to scan for Bluetooth devices once every 7 seconds
int scanTime = 7; //In seconds
BLEScan *pBLEScan;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YOUR_SSID";
char pass[] = "PASWORD";

BlynkTimer timer;

float temperature;
float humidity;
float light;
float soilMoisture;
String smokeStatus;
bool smokeValue;

const unsigned long sendInterval = 9000;
unsigned long previousTime = 0;

uint8_t buzzer = D9;
uint8_t bulb = D7;
uint8_t cooler = D6;
uint8_t heater = D3;
uint8_t fan = D2;

void smallBeep(){
  digitalWrite(buzzer, LOW);
  delay(500);
  digitalWrite(buzzer, HIGH);
}

void longBeep(){
  digitalWrite(buzzer, LOW);
  delay(1000);
  digitalWrite(buzzer, HIGH);
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      if (advertisedDevice.haveName())
      {
        if(String(advertisedDevice.getName().c_str()) == "SHT40")//Scan for a Bluetooth device named SHT40
        {
          Serial.print("Device name: ");
          Serial.println(advertisedDevice.getName().c_str());
          std::string strManufacturerData = advertisedDevice.getManufacturerData();
          uint8_t cManufacturerData[100];
          strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);
          Serial.printf("strManufacturerData: %d ", strManufacturerData.length());

          for (int i = 0; i < strManufacturerData.length(); i++)
          {
            Serial.printf("[%X]", cManufacturerData[i]);
          }

          //Getting raw data from SHT40
          float TemperatureData = int(cManufacturerData[2]<<8 | cManufacturerData[3]);
          float HumidityData = int(cManufacturerData[5]<<8 | cManufacturerData[6]);

          //Convert raw data into temperature and humidity data
          temperature = (175 * TemperatureData/65535) - 45;
          humidity = (125 * HumidityData/65535) - 6;

          if(temperature>=30){
            smallBeep();
            digitalWrite(cooler, LOW);
            digitalWrite(heater, HIGH);
          } else if (temperature<=20){
            digitalWrite(cooler, HIGH);
            digitalWrite(heater, LOW);
          } else {
            digitalWrite(cooler, HIGH);
            digitalWrite(heater, HIGH);
          }

          if(humidity>=50){
            digitalWrite(fan, LOW);
          } else {
            digitalWrite(fan, HIGH);
          }

          Serial.println();
          Serial.print("TemperatureData:");Serial.print(temperature);Serial.println("℃");   
          Serial.print("HumidityData:");Serial.print(humidity);Serial.println("%");      
          Serial.println("------------------");
          delay(250);       
        }

        if(String(advertisedDevice.getName().c_str()) == "BLE-Light-Beacon"){
          Serial.print("Device name: ");
          Serial.println(advertisedDevice.getName().c_str());
          std::string strManufacturerData = advertisedDevice.getManufacturerData();
          uint8_t cManufacturerData[100];
          strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);
          Serial.printf("strManufacturerData: %d ", strManufacturerData.length());
            for (int i = 0; i < strManufacturerData.length(); i++)
            {
              Serial.printf("[%X]", cManufacturerData[i]);
            }
            float Sensor_Data = int(cManufacturerData[2]<<8 | cManufacturerData[3]);
            Serial.println();
            light = map(Sensor_Data, 0, 2900, 0, 100);
            Serial.print("Light:");Serial.print(light);Serial.println(" %");
            if(light<50){
              digitalWrite(bulb, LOW);
              Serial.println("Light Level: Low Light");
            }
            else {            
              digitalWrite(bulb, HIGH);
              Serial.println("Light Level: OK");
            }      
            Serial.println("------------------");
            delay(250);        
        }  

        if(String(advertisedDevice.getName().c_str()) == "BLE-SoilMoisture-Beacon"){
          Serial.print("Device name: ");
          Serial.println(advertisedDevice.getName().c_str());
          std::string strManufacturerData = advertisedDevice.getManufacturerData();
          uint8_t cManufacturerData[100];
          strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);
          Serial.printf("strManufacturerData: %d ", strManufacturerData.length());
            for (int i = 0; i < strManufacturerData.length(); i++)
            {
              Serial.printf("[%X]", cManufacturerData[i]);
            }
            float Sensor_Data = int(cManufacturerData[2]<<8 | cManufacturerData[3]);
            Serial.println();
            soilMoisture = map(Sensor_Data, 0, 2900, 0, 100);
            Serial.print("Soil Moisture:");Serial.print(soilMoisture);Serial.println(" %");
            if(soilMoisture>50){
              Serial.println("Soil Moisture: OK");
            }
            else {
              Serial.println("Soil Moisture: LOW");
            }      
            Serial.println("------------------");
            delay(250);        
        }

        if(String(advertisedDevice.getName().c_str()) == "BLE-Smoke-Beacon"){
          Serial.print("Device name: ");
          Serial.println(advertisedDevice.getName().c_str());
          std::string strManufacturerData = advertisedDevice.getManufacturerData();
          uint8_t cManufacturerData[100];
          strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);
          Serial.printf("strManufacturerData: %d ", strManufacturerData.length());
            for (int i = 0; i < strManufacturerData.length(); i++)
            {
              Serial.printf("[%X]", cManufacturerData[i]);
            }
            float Sensor_Data = int(cManufacturerData[2]<<8 | cManufacturerData[3]);
            Serial.println();
            uint8_t smoke = map(Sensor_Data, 0, 2900, 0, 100);
            Serial.print("Smoke:");Serial.print(smoke);Serial.println(" %");
            if(smoke>50){
              smokeStatus = "Yes";
              smokeValue = 1;
              longBeep();
              Serial.println("Smoke: Present");
            }
            else {
              smokeStatus = "No";
              smokeValue = 0;  
              Serial.println("Smoke Level: Low");
            }      
            Serial.println("------------------");
            delay(250);        
        }  

      }
    }
};

void sendDataToBlynk(){
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, light);
  Blynk.virtualWrite(V3, soilMoisture);
  Blynk.virtualWrite(V4, smokeValue);
}

void endNextionCommand()
{
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

void sendTemperatureToNextion()
{
  String command = "temperature.txt=\""+String(temperature,2)+"\"";
  Serial2.print(command);
  endNextionCommand();
}


void sendHumidityToNextion()
{
  String command = "humidity.txt=\""+String(humidity,2)+"\"";
  Serial2.print(command);
  endNextionCommand();
}

void sendLightToNextion()
{
  String command = "light.txt=\""+String(light,2)+"\"";
  Serial2.print(command);
  endNextionCommand();
}


void sendSoilMoisturetToNextion()
{
  String command = "soilMoisture.txt=\""+String(soilMoisture,2)+"\"";
  Serial2.print(command);
  endNextionCommand();
}

void sendSmokeToNextion()
{
  String command = "smoke.txt=\""+smokeStatus+"\"";
  Serial.println(command);
  Serial2.print(command);
  endNextionCommand();
}

void sendDataToNextion(){
  sendTemperatureToNextion();
  sendHumidityToNextion();
  sendLightToNextion();
  sendSoilMoisturetToNextion();
  sendSmokeToNextion();
}

void init_gpio(){
  pinMode(buzzer, OUTPUT);
  pinMode(bulb, OUTPUT);
  pinMode(cooler, OUTPUT);
  pinMode(heater, OUTPUT);
  pinMode(fan, OUTPUT);

  digitalWrite(buzzer, HIGH);
  digitalWrite(bulb, HIGH);
  digitalWrite(cooler, HIGH);
  digitalWrite(heater, HIGH);
  digitalWrite(fan, HIGH);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial.println("Starting....");
  init_gpio();

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(11000L, sendDataToBlynk); // Send data every 11 seconds
}

void loop()
{
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory

  unsigned long currentTime = millis();
  if (currentTime - previousTime >= sendInterval) {
  sendDataToNextion();
  previousTime = currentTime;
  }

  Blynk.run();
  timer.run();
}