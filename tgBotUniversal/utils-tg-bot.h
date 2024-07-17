#ifndef UtilsTgBot_H
#define UtilsTgBot_H

#include <Arduino.h>

// Manage acquiring date and time
#include <NTPClient.h>
#include <WiFiUdp.h>

// Manage the filesystem of the ESP8266
#include <FS.h>
#include <LittleFS.h>
#define FILE_PATH_JSON "/dataReadings.json"

// Data structure for the data readings
typedef struct ReadingSensors{
  float temperatureExt;
  float temperatureInt;
  float luminosity;
  String dataTime;  // Assuming this is formatted as "DD-MM-YYYY HH:MM:SS"
} ReadingSensors;

// NTP configuration
extern const int utcOffsetInSeconds;
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;

String getCurrentDate();
String getMeasureUnit(String dataType);
void getTodayDataMean(String dataType, float& currentValue, float& meanValue);
String dataToJson(ReadingSensors data);
void saveDataToFile(const char* filename, String data);
String readDataFromFile(const char* filename);
void printJSONFileToSerial();
void clearFileContent(const char* filename);

#endif