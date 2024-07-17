#include "utils-tg-bot.h"

// NTP configuration
const int utcOffsetInSeconds = 7200;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Function to print the json file in Serial monitor
void printJSONFileToSerial(){

}

String getMeasureUnit(String dataType){
  String result = "";
  if (dataType == "temperatureExt" || dataType == "temperatureInt") {
      result = "°C";
  } else if (dataType == "luminosity") {
      result = "lux";
  } else if (dataType == "waterLevel") {
      result = "cm";
  }
  return result;
}

void getTodayDataMean(String dataType, float& currentValue, float& meanValue){
  float sumValues = 0.0;
  int count = 0;

  // Read the JSON file
  File file = LittleFS.open(FILE_PATH_JSON, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    file.close();
    return;
  }

  file.close();

  // Get current date in DD-MM-YYYY format
  
  String currentDateTime = getCurrentDate();
  String currentDate = "";
  for(char c: currentDateTime){
    // To retrieve only the current date
    if(c == ' ')
      break;
    else
      currentDate += c;
  }

  // Iterate through JSON array
  for (JsonVariant data : doc.as<JsonArray>()) {
    String dataTime = data["dateTime"].as<String>();
    dataType = dataType.substring(1); // Trim the string
    if (dataTime.startsWith(currentDate)) {
      float value = data[dataType.c_str()].as<float>();
      sumValues += value;
      count++;
      // Set current value to the latest value in the JSON file for the current day
      currentValue = value; // Assuming `value` is the last value found for the current day
    }
  }

  // Calculate mean value
  if (count > 0) {
    meanValue = sumValues / count;
  }
}

String dataToJson(ReadingSensors data){
  StaticJsonDocument<512> doc;
  JsonArray readings = doc.to<JsonArray>();

  JsonObject readingToSave = readings.createNestedObject();
  readingToSave["temperatureExt"] = data.temperatureExt;
  readingToSave["temperatureInt"] = data.temperatureInt;
  readingToSave["luminosity"] = data.luminosity;
  readingToSave["dateTime"] = data.dataTime;

  String output;
  serializeJson(doc, output);
  return output;
}

void saveDataToFile(const char* filename, String data){
  File file = LittleFS.open(filename, "a");
  if (file) {
    file.println(data);
    Serial.println(data);
    file.close();
    Serial.println("Data appended to file.");
  } else {
    Serial.println("Error opening file for writing.");
  }
}

String readDataFromFile(const char* filename){
    String data = "";
    if(LittleFS.exists(filename)){
      File file = LittleFS.open(filename, "r");
      if (file) {
        while (file.available()) {
          data += file.readStringUntil('\n');
          data += '\n';
        }
        file.close();
        Serial.println("Data read from file.");
      } else {
        Serial.println("Error opening file for reading.");
      }
    }
    return data;
}

void clearFileContent(const char* filename){
  // Check if file exists
  if (LittleFS.exists(filename)) {
    // Open file in write mode which will clear the content
    File file = LittleFS.open(filename, "w");
    if (file) {
      file.close();
      Serial.println("File content deleted successfully.");
    } else {
      Serial.println("Failed to open file for writing.");
    }
  } else {
    Serial.println("File does not exist.");
  }
}

String getCurrentDate(){
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  String response = "";
  
  int currentDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  
  response += String(currentDay) + "/";
  if(currentMonth < 10){
    response += "0";
    response += String(currentMonth);
    response += "/";
  }
  else{
    response += String(currentMonth) + "/";
  }
  response += String(currentYear);
  response += " ";
  response += timeClient.getFormattedTime();

  return response;
}