#include "./utils-tg-bot.h"
// Manage the TelegramBot Server
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID "SSID wifi"
#define WIFI_PASSWORD "insert wifi password"

// Telegram Bot token
#define BOT_TOKEN "7257399139:AAFlpPbv9ReILzkCYz7xnjvIax6kgWqhSrA"

// Telegram chat_id permitted
String chat_id_permitted[] = {"6592024447", "1043326221"};

// Fingerprint for the telegram API
#define FINGERPRINT_TG "1F:77:5F:20:C5:D3:BD:67:DE:E8:07:9B:59:1D:22:E9:C0:E4:52:4B"

const unsigned long BOT_MTBS = 5000; // mean time between scan messages
const int ledPin = 13;

WiFiClientSecure secured_client;

UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// Use a simple web server to manage the data sent by the msp432
ESP8266WebServer server(80);

// Last time messages' scan has been done
unsigned long bot_lasttime;

// Handle HTTP post request to upload data to the bot
void handleDataReadings(){
  // Serial.println("=========== JSON data sent to bot ==========");
  if(server.hasArg("plain")){
    String body = server.arg("plain");

    // Parse JSON string
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, body);

    // Check for parsing errors
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
    } else {
      // Extract data and save to file
      ReadingSensors data;
      data.temperatureExt = doc["temperatureExt"];
      data.temperatureInt = doc["temperatureInt"];
      data.luminosity = doc["luminosity"];
      
      // Populate with current Date and time
      data.dataTime = getCurrentDate();

      // Convert data to JSON string
      String jsonData = dataToJson(data);

      // Save data to file
      saveDataToFile(FILE_PATH_JSON, jsonData);
    }
  }
}

void handleNewMessages(int numNewMessages){
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    bool chat_id_found = false;

    // The only chat_id permitted are the one for the MSP and the other for the telegram bot user
    for (int i = 0; i < 2; ++i) {
      // Compare each element with the target string
      if(chat_id_permitted[i] == chat_id) {
        chat_id_found = true;
        break; // Exit the loop early if found
      }
    }

    if (!chat_id_found) {
      bot.sendMessage(chat_id, "Utente non autorizzato", "");
      continue;
    }

    // Help command
    if (text == "/help")
    {
      String welcome = "Commands available: \n";
      welcome += "/getTodayInfo : to get the current values of the sensors and the daily mean\n";
      welcome += "/getAllData : to get all the data of the archive (of all the days)\n";
      welcome += "/testFan : to test the fan (aka. Cooling system)\n";
      welcome += "/testAlarm : to test the alarm(aka. critical event notificator)\n";
      welcome += "/testLight : to test the alarm(aka. critical event notificator)\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }

    // Retrieving all data from json file
    if (text == "/getAllData")
    {
      String jsonData = readDataFromFile(FILE_PATH_JSON);
      if(jsonData){
        bot.sendMessage(chat_id, jsonData, "Markdown");
        bot.sendMessage(chat_id, "Sending all data to user...", "");
      }
      else{
        bot.sendMessage(chat_id, "There aren't saved data yet", "");
      }
    }

    // Send options to user to choose the data to retrieve
    if(text == "/getTodayInfo"){
      String keyboardJson = "[[\"/getTodayInfo temperatureExt\", \"/getTodayInfo temperatureInt\"], [\"/getTodayInfo luminosity\"]";

      String response = "Select a data type to view:\n";
  
      // Send message with keyboard options
      bot.sendMessageWithReplyKeyboard(chat_id, response, "", keyboardJson, true);
    }

    // Get current and mean temperatures for today
    if (text.startsWith("/getTodayInfo ")){
      String dataType = text.substring(13); //obtain the parameter

      float currentValue = 0.0;
      float meanValue = 0.0;
      getTodayDataMean(dataType, currentValue, meanValue);
      if(currentValue == 0.0 || meanValue == 0.0){
        bot.sendMessage(chat_id, "There aren't data yet", "");
        return;
      }
      String response = "Today's " + String(dataType) + "\n";
      response += "Current " + String(dataType) + ": " + String(currentValue) + getMeasureUnit(dataType) + "\n";
      response += "Mean " + String(dataType) + ": " + String(meanValue) + getMeasureUnit(dataType)  + "\n";

      bot.sendMessage(chat_id, response, "");
    }

    // Testing fan of the MSP
    if (text == "/testFan")
    {
      // ???
      bot.sendMessage(chat_id, "Testing fan of MSP...", "");
    }

    // Testing servo motor of the MSP
    if (text == "/testServo")
    {
      // Serial or HTTP request comunication, to implement
      bot.sendMessage(chat_id, "Testing servo of MSP", "");
    }

    // Testing alarm of the MSP
    if (text == "/testAlarm")
    {
      // Serial or HTTP request comunication, to implement
      bot.sendMessage(chat_id, "Testing alarm of MSP", "");
    }

    // Testing servo motor of the MSP
    if (text == "/testLight")
    {
      // Serial or HTTP request comunication, to implement
      bot.sendMessage(chat_id, "Testing light of MSP", "");
    }

    if (text == "/start")
    {
      String welcome = "Welcome to the telegram bot for the project of Embedded of the IoT " + from_name + ".\n";
      welcome += "/getTodayInfo : to get the current values of the sensors and the daily mean\n";
      welcome += "/getAllData : to get all the data of the archive (of all the days)\n";
      welcome += "/testFan : to test the fan (aka. Cooling system)\n";
      welcome += "/testServo : to test the servo motor(aka. luminosity regulator)\n";
      welcome += "/testAlarm : to test the alarm(aka. critical event notificator)\n";
      welcome += "/testLight : to test the light(aka. critical event notificator)\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup(){
  Serial.begin(115200);

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("LittleFS mount failed!");
    return;
  }
  // Clear the JSON file, just to debug easier
  clearFileContent(FILE_PATH_JSON);

  // Set WiFi to station mode and disconnect from an AP if it was Previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  Serial.print(" ");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  secured_client.setFingerprint(FINGERPRINT_TG);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  //Start NTP client
  timeClient.begin();

  // Start the web server
  server.on("/upload", HTTP_POST, handleDataReadings);
  server.begin();
  Serial.println("HTTP server started!");
}

void loop(){
  timeClient.update();

  // Manage the http post request to the http server
  server.handleClient();

if (millis() - bot_lasttime > BOT_MTBS){
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
