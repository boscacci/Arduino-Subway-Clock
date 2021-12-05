// Enter your Wifi SSID and password here
// NOTE: ESP32 Does not work with a 5Ghz wifi network only 2.4Ghz
const char ssid[] = "YOUR_WIFI_NETWORK_NAME_HERE";
const char password[] = "YOUR_WIFI_PASSWORD_HERE";

// Server IP address
const char serverIP[] = "YOUR_SERVER_IP_ADDRESS_HERE";

// Find the four-character station ID from:
// https://github.com/boscacci/MTAPI/blob/master/data/stations.json
// And enter it here
const char stationID[] = "69ba";

// If not using a hardware toggle switch, choose prefered train direction, either "N" for Northbound, "S" for Southbound
//#define DIRECTION "N"; //Comment out this line if you've set up a toggle switch instead

// Set server request interval time in ms
const unsigned long requestInterval = 15000;

// Set display interval for rotating second line in ms
const unsigned long displayInterval = 3000;

// Filter out arrivals that are less than your time (in minutes) to get to station
const byte timeToStation = 4;

// Set number of additional arrivals to cycle through on the display
const byte moreArrivals = 3;

// NTP Server set up for time getting
const char ntpServer[] = "pool.ntp.org";