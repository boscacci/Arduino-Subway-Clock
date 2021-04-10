// Enter your Wifi SSID and password here
const char ssid[] = "your-wifi-network-here";
const char password[] = "your-wifi-password-here";

//Server IP address
const char serverIP[] = "your-server-ip-here"

// Find the four-character station ID from:
// https://github.com/boscacci/MTAPI/blob/read-plaintext-stops-transfers/scripts/stations.json
// And enter it here
const char stationID[] = "b2e2";

//Choose prefered train direction, either "N" for Northbound, "S" for Southbound
#define DIRECTION "N";  //Comment out this line if you've set up a toggle switch instead

// Set server request interval time in ms
const unsigned long requestInterval = 15000;

// Set display interval for rotating second line in ms
const unsigned long displayInterval = 3000;

// Filter out arrivals that are less than your time (in minutes) to get to station
const byte timeToStation = 0;

// Set number of additional arrivals to cycle through on the display
const byte moreArrivals = 3;

//NTP Server set up for time getting
const char ntpServer[] = "pool.ntp.org";
const long  gmtOffset_sec = -18000;  //Time zone in seconds from UTC
const int   daylightOffset_sec = 3600; //set to 3600 if your country observes DST
