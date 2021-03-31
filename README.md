/////////////////////////////////////////////////////////INFO/////////////////////////////////////////////////////////

30-day report New-EIR Germicidal UV-C and air quality monitoring
Program measures UV output with VEML6075, collects air quality reading from SGP30 and BME680, uses i2c connection to ESP32 displays data on wedserver
To set up ESP32 open IDE press "ctrl + comma" or File, Preferences, Additional Boards Manager URLs: enter the URL for json file
  URL for json file:  https://dl.espressif.com/dl/package_esp32_index.json
Press "alt + T" or go to Tools, Boards, Boards Manager install ESP32 by Espressif Systems. Then go to Tools, Boards, ESP32 Arduino, "Adafruit ESP32 Feather"
Right click on "Start menu" to open "Device Manager" expaned "Ports(COM and LPT)" find which COM is used by "Silicon Labs CP210x USB to UART Bridge"
Press "alt + T" or go to Tools, Port: select the Serial ports COM used by "Silicon Labs CP210x USB to UART Bridge" (e.i. COM5)
Install all Libraries to header files where is says "// Click here to get the library: http://librarymanager/All#Blynk_Async_ESP32_BT_WF"
Replace ssid "network" and "password" with your oun WiFi network id and password.
const char* ssid = "network";
const char* pwd  = "password";
ESP32 will set up webserver and display "Local IP address" in serial monitor with baud rate "115200"

/////////////////////////////////////////////////////Confirgure IDE/////////////////////////////////////////////////////

 * File
    Preferences
      Additional Boards Manager URLs: 
  https://dl.espressif.com/dl/package_esp32_index.json
  
 * Tools
    Boards
      ESP32 Arduino
        "Adafruit ESP32 Feather"
  Upload speed: "921600"
  Flash Frequincy: "80MHz"
  Partion Scheme: "Default"
  Core Debug Level: "None"

