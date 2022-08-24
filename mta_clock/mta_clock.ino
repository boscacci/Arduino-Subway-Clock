// include the library code:
#include <Adafruit_CharacterOLED.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "time.h"
#include "preferences.h" //file containing User preferences

/*
  Adafruit OLED Wiring Guide (https://www.adafruit.com/product/823):

   OLED pin  <-->   ESP32 pin
    1 (gnd)         GND
    2 (vin)         VIN
    4 (rs)          14
    5 (rw)          32
    6 (en)          26
   11 (d4)          33
   12 (d5)          27
   13 (d6)          12
   14 (d7)          13

*/
Adafruit_CharacterOLED lcd(OLED_V2, 14, 32, 26, 33, 27, 12, 13);

#define SWITCH_PIN 25 // sets pin for direction toggle switch connect other end to GND

// This defines the direction variable differently depending on if you are using a toggle switch
#ifndef DIRECTION
char *direction = "N";
#else
const char *direction = DIRECTION;
#endif

// Declaring global variables
unsigned long lastRequestTime = 0;
unsigned long lastDisplayTime = 0;
time_t currentEpochTime;
struct tm currentTime;
char display[20];
char displayList[8][20];
byte listCount = 1;
bool forceRefresh = true;
bool switchState = true;
char url[sizeof(serverIP) + sizeof(stationID) + 19];
byte numberOfArrivals;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("Initializing...");

  // Sets up toggle switch pin with internal pullup resistor
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  // Initialize the lcd
  Serial.print("LCD - ");
  lcd.begin(16, 2);
  Serial.println("Success!");

  // Connect to the WiFi network
  connectWifi();

  // Configure DST-aware datetime for NYC
  initTime("EST5EDT,M3.2.0,M11.1.0");

  Serial.print("Local time: ");
  printLocalTime();
  Serial.println(" - Success!");

  // Make url for MTA data request
  sprintf(url, "http://%s:8080/by-id/%s", serverIP, stationID);

  delay(1000);
  lcd.clear();
}

void loop()
{
// If you have a direction toggle switch this function handles it
#ifndef DIRECTION
  switchHandler();
#endif

  // Send an HTTP GET request every time interval
  if (forceRefresh || (millis() - lastRequestTime) > requestInterval)
  {

    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {

      // gets current local time in epoch and tm format respectively
      time(&currentEpochTime);
      getLocalTime(&currentTime);

      // Make server request and parse into JSON object
      JSONVar obj = JSON.parse(httpGETRequest(url));

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(obj) == "undefined")
      {
        Serial.println("Parsing input failed!");
        return;
      }

      // Pulls out the relevant data as an JSONVar array
      JSONVar arrivalsArr = obj["data"][direction];
      numberOfArrivals = arrivalsArr.length();

      // Initiate a count of arrivals that will be missed per timeToStation
      byte missed = 0;

      // Clear the displayList
      memset(&displayList, 0, sizeof(displayList));

      // Iterates through each pending arrival
      for (byte i = 0; i < numberOfArrivals; i++)
      {

        // Extracts the name of the train for the given arrival
        String trainName = JSON.stringify(arrivalsArr[i]["route"]).substring(1, 2);

        // Extracts the arrival time of the train in epoch time

        // DEBUG
        // Serial.print("Arrival time stringified: ");
        // Serial.println(JSON.stringify(arrivalsArr[i]["time"]));

        long arrivalTime = convertToEpoch(JSON.stringify(arrivalsArr[i]["time"]));

        // Calculates how many minutes to arrival by comparing arrival time to current time
        int minutesAway = (arrivalTime - currentEpochTime) / 60;

        // Filters out trains that you can't possibly catch
        if (minutesAway >= timeToStation)
        {

          // Constructs the display string
          sprintf(display, "%d. (%s) %s %dMin", i + 1 - missed, trainName, (direction == "N") ? "UP" : "DN", minutesAway);
          Serial.println(display);

          // Adds the given arrival to the display list for the lcd
          strcpy(displayList[i - missed], display);
        }
        else
        {
          missed++; // increment count of missed trains per timeToStation
        }
      }

      // Display the next arriving train on the first line of the lcd
      lcd.setCursor(0, 0);
      lcd.print("                "); // needed to clear the first line
      lcd.setCursor(0, 0);
      lcd.print(displayList[0]);
    }
    else
    {
      Serial.println("WiFi Disconnected");
      delay(1000);
      connectWifi();
    }
    lastRequestTime = millis();
  }

  // Rotate the arrival displayed on the second line at specified time interval
  if (forceRefresh || (millis() - lastDisplayTime) > displayInterval)
  {

    lcd.setCursor(0, 1);
    lcd.print("                "); // needed to clear the line if the previous display was longer
    lcd.setCursor(0, 1);
    lcd.print(displayList[listCount]);

    listCount++;
    if (listCount > moreArrivals || listCount >= numberOfArrivals)
      listCount = 1;

    lastDisplayTime = millis();

    forceRefresh = false;
  }
}

void connectWifi()
{
  Serial.print("Connecting to Wifi: ");
  Serial.print(ssid);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Joining Wifi");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }

  lcd.setCursor(0, 0);
  lcd.print("Connected to:");
  Serial.println("Success!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

String httpGETRequest(char *_url)
{
  HTTPClient http;

  Serial.print("Pinging: ");
  Serial.println(_url);

  // Connect to server url
  http.begin(_url);

  // Send HTTP GET request
  byte httpResponseCode = http.GET();

  if (httpResponseCode == 200)
  {
    Serial.print("Updated: ");
    Serial.println(&currentTime);
    return http.getString();
  }
  else
  {
    Serial.print("HTTP Error code: ");
    Serial.println(httpResponseCode);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HTTP ERROR: ");
    lcd.print(httpResponseCode);
    lcd.setCursor(0, 1);
    lcd.print("SERVER DOWN");
    delay(1000);
  }
}

// Manually parses the timeStamp from the train arrival and returns in epoch time
long convertToEpoch(String timeStamp)
{
  struct tm t;
  memset(&t, 0, sizeof(tm));                            // Initalize to all 0's
  t.tm_year = timeStamp.substring(1, 5).toInt() - 1900; // This is year-1900, so 112 = 2012
  t.tm_mon = timeStamp.substring(6, 8).toInt() - 1;     // It has -1 because the months are 0-11
  t.tm_mday = timeStamp.substring(9, 11).toInt();
  t.tm_hour = timeStamp.substring(12, 14).toInt();
  t.tm_min = timeStamp.substring(15, 17).toInt();
  t.tm_sec = timeStamp.substring(18, 20).toInt();
  time_t epoch = mktime(&t);

  return epoch;
}

// Function to handle direction toggle switch
#ifndef DIRECTION
void switchHandler()
{
  // toggles with switch
  if (digitalRead(SWITCH_PIN) != switchState)
  {
    switchState = !switchState;

    // Sets direcdtion per the switch state
    if (switchState)
      direction = "N";
    else
      direction = "S";

    // Notifies in serial monitor
    Serial.print("Direction: ");
    Serial.println(direction);

    // Notifies on LCD screen
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Direction set: ");
    lcd.print(direction);
    delay(1000);

    // Forces refresh on the next loop
    forceRefresh = true;
  }
}
#endif

void setTimezone(String timezone)
{
  Serial.printf("  Setting Timezone to %s\n", timezone.c_str());
  setenv("TZ", timezone.c_str(), 1); //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void initTime(String timezone)
{
  struct tm timeinfo;
  Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org"); // First connect to NTP server, with 0 TZ offset
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("  Failed to obtain time");
    return;
  }
  Serial.println("  Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
}