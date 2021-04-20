# You're about to make your own MTA clock!

![](https://github.com/boscacci/MTA-clock-esp32/blob/main/mta-clock.jpg "Esp32 MTA Clock")

Here's how to do it:
1. Register for an MTA API key.
2. Host a server.
3. Put and run MTA API code on the server.
4. Buy parts.
5. Wire it up.
6. Install some software.
7. Set up your Arduino project.
8. Edit your MTA Clock preferences.
9. Compile and upload the code.
10. Make an enclosure.
11. Assemble.
12. Don't miss your train!

# 1. Register for an MTA API key.

  * Create an account at this URL: https://api.mta.info
  * Sign in to your account.
  * At the top, click on "Access Key"
  * Leave this open for later.

# 2. Host a server.

  * Make an AWS account here (it will want your credit card, but it's free for the first year): https://aws.amazon.com/free/
  * Log in to your account.
  * Launch a virtual machine with EC2
  * Select Ubuntu Server (highest number is fine)
  * Select the Free Tier Eligible instance type
  * Click through menus until you reach "Configure Security Group"
  * Change the "Type" to "All Traffic"
  * Enable access to ping your server from your personal IP address by googling "my ip address"
  * Copy your IP address to the Source column, but modify like this: "XX.XX.XXX.0/24"
  * Name your security group
  * Review and Launch your Server
  * Create a new key pair, name it, download it somewhere safe
  * Launch your server
  * Connect to your server using SSH client:
      * Check the box on the server Name
      * Go to SSH Client tab
      * Copy the example command, something like: `ssh -i "xxxxxxxx.pem" ubuntu@ec2-xxxxxxxxxx.us-east-2.compute.amazonaws.com`
      * Open terminal and cd into the directory with your key downloaded earlier
      * Paste the above command and say "yes" to the dubious message
      * You're driving a cloud server now!

# 3. Put and run MTA API code on the server.

  * Put the MTA API code on the server by cloning from git repo.
  * Use this command in terminal while controlling your server: `git clone https://github.com/boscacci/MTAPI.git`
  * cd into the repo top-level directory: `cd MTAPI/`
  * Update your Ubuntu server so it can install stuff: `sudo apt-get update && sudo apt-get upgrade`
  * Install python on your server: `sudo apt-get install python3-venv`
  * Create a new python virtual environment: `python3 -m venv .venv`
  * Create a new tmux session (allowing the code to run unattended): `tmux new -s session-name`
  * Activate the virtual environment: `source .venv/bin/activate`
  * Install necessary python libraries: `pip install -r requirements.txt`
  * Modify the settings.cfg file to include your personal API Key.
      * Open the file in a terminal text editor: `vim settings.cfg`
      * Go back to your API Key from before and copy it
      * Press `a` to enter insert mode and replace 'your-api-key-here' with your API key
      * Press `esc` to go back to command mode
      * Enter the command: `:wq` to save and quit the text editor
  * Launch the service and begin listening for requests: `flask run --host=0.0.0.0 --port=8080`
  * Detach from the tmux session: ctrl-b, then press d key
  * Disconnect from the ec2 server: `logout`
  * It should be running on its own now!

# 4. Buy parts.

  These are the parts you will need and some examples of where to find them:
  * ESP32 Dev Board (any ESP dev board will do) - https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z
  
  * LCD Screen (16x2 Character Display)- https://www.adafruit.com/product/823 - if they're not available through adafruit, try amazon or aliexpress for other 'standard' HD44780 1602 LCDs.
  
  * On/Off Switch (this is just the one we used) - https://www.amazon.com/Qiying-Miniature-SS-12F16-Position-Latching/dp/B08947137H
  
  * Jumperwires
  
  * USB Cable

# 5. Wire it up.

  __Adafruit OLED Wiring Guide:__

  | OLED pin    | ESP32 pin     |
  | :------------- | :------------- |
  |  1 (gnd)    |     GND         |
  |  2 (vin)    |     VIN         |
  |  4 (rs)     |     14          |
  |  5 (rw)     |     32          |
  |  6 (en)     |     26          |
  | 11 (d4)     |     33          |
  | 12 (d5)     |     27          |
  | 13 (d6)     |     12          |
  | 14 (d7)     |     13          |

   * If your ESP32 has different pins labeled, feel free to swap out pins per this pinout guide, but also update line 24 `Adafruit_CharacterOLED lcd(OLED_V2, 14, 32, 26, 33, 27, 12, 13);` in the mta_clock.ino accordingly: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

   * Optional: Connect, an on/off switch between ESP32 pin 25 (or any other digital pin) and GND.

# 6. Install some software.

## 6.1 Install ArduinoIDE.

  * Download and install the ArduinoIDE from here: https://www.arduino.cc/en/software

## 6.2 Install ESP board in Arduino.

  * Open ArduinoIDE
  * Copy the URL found at this git: https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md
  * In ArduinoIDE, paste to File > Preferences > Additional Board Manager URLs
  * Open Boards Manager from Tools > Board menu
  * Search for "esp32" and Install
  * Select your particular ESP32 board from Tools > Board menu

## 6.3 Install necessary libraries.

  * Open Library Manager from Tools > Manage Libraries...
  * Search for "Arduino_JSON" and Install
  * Download ZIP from here: https://github.com/ladyada/Adafruit_CharacterOLED
  * Add the library to Arduino from Sketch > Include Library > Add .ZIP Library...

# 7. Set up your Arduino project.

  * If you haven't already, download the ZIP of this Git Repo.
  * Move the folder "mta_clock" from the ZIP folder into your Sketchbook folder.
  * If you don't know where that is, it is listed at File > Preferences > Sketchbook location

# 8. Edit your MTA Clock preferences.

  * Open the mta_clock.ino sketch from File > Sketchbook > mta_clock
  * Go to the preferences.h tab.
  * Replace the default with your Wifi network info, server IP (found in your EC2 instances, use the IPv4 address), and other preferred settings. [You can find the codes for StationID's here](https://github.com/boscacci/MTAPI/blob/master/data/stations.json).
  * Save your changes.

# 9. Compile and upload the code.

  * Connect the ESP32 to the computer with the USB cable.
  * Ensure that the correct port is selected at Tools > Port > /dev/cu.usbserial*
  * Ensure that Tools > Upload Speed is set to 115200
  * Click the check mark to "Verify" (a.k.a. Compile) the code.
  * Click the => to upload the code to your ESP32.
  * Wait.... this will probably take a minute or two.
  * When it's done uploading, you can open the Serial monitor (set to Baud rate: 115200) with the magnifying glass button to see how it's doing.

  NOTE: For some reason, the LCD might not display properly while the serial monitor window is open.

  ANOTHER NOTE: The LCD won't reset when the ESP32 resets, so you might have to unplug and replug the usb cable to get it to display properly.

# 10. Make an enclosure.

  * You can buy something like this: https://www.amazon.com/CANADUINO-Character-Display-Acrylic-Enclosure/dp/B07C4XWX21

  NOTE: It did not fit my display and there isn't a great way to mount the ESP32.

  * Alternatively, you can 3D print the enclosure found in the git.
    To avoid supports:
    * Print main_enclosure_switch.stl or main_enclosure_noswitch.stl lcd-face down
    * Print enclosure_cover.stl on one end - the little tabs are tapered on one edge more than the other and that should face down.

# 11. Assemble.

  If you're happy with your preferences, and everything fits properly in the enclosure, you can solder the connections and install everything in the enclosure.

# 12. Don't miss your train!

  If you're still reading, thanks for reading me. Now, credit where credit is due! Thanks to:

  Zach Kuperstein for toiling in an unfamiliar language to make this, and making the instructions as clear as possible!

  Jon Thornton for making the MTA's GTFS protobuf accessible.

  Bill & George at NYC Resistor for making the first generation version of this.

  The MTA for making its data available to the public.
