/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
   Changed to a beacon scanner to report iBeacon, EddystoneURL and EddystoneTLM beacons by beegee-tokyo
*/

//=======================================================================================================//
// Projet/Tutorial        - Getting started with DFRobot Fermion BLE Sensor Beacon 
// Author                 - https://www.hackster.io/maheshyadav216
// Hardware               - Fermion: BLE Sensor Beacon Module, 3.3V USB-TTL Serial Adapter,
//                        - FireBeetle_ESP32_E, SHT40 Temperature & Humidity Sensor
// Softare                - VSCode PlatformIO, 
// GitHub Repo of Project - https://github.com/maheshyadav216/DFRobot-Fermion-BLE-Sensor-Beacon
// Code last Modified on  - 13/03/2024
//=======================================================================================================//
// All Sensors

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEEddystoneURL.h>
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>
#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

float TemperatureData,HumidityData;
float Temperature,Humidity;

//Setting up ESP32 to scan for Bluetooth devices once every 5 seconds
int scanTime = 5; //In seconds
BLEScan *pBLEScan;

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
          TemperatureData = int(cManufacturerData[2]<<8 | cManufacturerData[3]);
          HumidityData = int(cManufacturerData[5]<<8 | cManufacturerData[6]);

          //Convert raw data into temperature and humidity data
          Temperature = (175 * TemperatureData/65535) - 45;
          Humidity = (125 * HumidityData/65535) - 6;

          Serial.println();
          Serial.print("TemperatureData:");Serial.print(Temperature);Serial.println("℃");   
          Serial.print("HumidityData:");Serial.print(Humidity);Serial.println("%");      
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
            uint8_t Light = map(Sensor_Data, 0, 2900, 0, 100);
            Serial.print("Light:");Serial.print(Light);Serial.println(" %");
            if(Light>50){
              Serial.println("Light: OK");
            }
            else {
              Serial.println("Light Level: LOW");
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
            uint8_t soilMoisture = map(Sensor_Data, 0, 2900, 0, 100);
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
              Serial.println("Smoke: Present");
            }
            else {
              Serial.println("Smoke Level: Low");
            }      
            Serial.println("------------------");
            delay(250);        
        }  

      }
    }
};
void setup()
{
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
}
void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}