# New_EIR_UVC

CSUS Senior Design Project 2021
Team members: Gil Clements | Aaron Costello | Alex Garcia | Max Repko | Richard Torres

Airflow in public areas can quickly spread infectious airborne diseases, compromising the health and safety of the occupants.

HVAC systems can become breeding grounds for bacteria, viruses, and dangerous molds which can be circulated through the building's air ducts. UV-C radiation kills and deactivates germs and viruses by damaging molecules like nucleic acids and proteins, preventing them from infecting humans. By including this UV disinfection in existing heating and air systems, we can reduce the spread of dangerous pathogens within a building and maintain a safe environment for all its occupants. 

We have created a safely enclosed air sanitizer that can be installed in existing HVAC systems. It uses UV light to eliminate microbes before they recirculate into an occupied space. We designed it with air quality sensors to monitor and report effectiveness. The user can view these air quality readings using an Android App. 

We will be improving the design for installation, building housing for an outdoor sensor, and incorporating solar panels with back-up battery. The user will be able to control the power of the device remotely.

This design is an effective and affordable way to reduce the spread of dangerous pathogens, like coronavirus, within businesses and public buildings. It can also reduce odor and prolong the life of an existing HVAC system.

Restaurants, Shopping Centers, Hair and Nail Salons 
Schools, Churches,  Theaters and Community Centers 
Locker Rooms, Fitness and Athletic Centers 
Hotels, Dormitories and Offices  

/////////////////////////////////////////////////////////INFO/////////////////////////////////////////////////////////
30-day report New-EIR Germicidal UV-C and air quality monitoring
Program measures UV output with VEML6075, collects air quality reading from SGP30 and BME680, uses i2c connection to ESP32 displays data on wedserver
To set up ESP32 open IDE press "ctrl + comma" or File, Preferences, Additional Boards Manager URLs: enter the URL for json file
  URL for json file:  https://dl.espressif.com/dl/package_esp32_index.json
Press "alt + T" or go to Tools, Boards, Boards Manager install ESP32 by Espressif Systems. Then go to Tools, Boards, ESP32 Arduino, "Adafruit ESP32 Feather"
Right click on "Start menu" to open "Device Manager" expaned "Ports(COM and LPT)" find which COM is used by "Silicon Labs CP210x USB to UART Bridge"
Press "alt + T" or go to Tools, Port: select the Serial ports COM used by "Silicon Labs CP210x USB to UART Bridge" (e.i. COM5)
Install all Libraries to header files where is says "// Click here to get the library: http://librarymanager/All#Blynk_Async_ESP32_BT_WF"

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
  
  *Tools
    Port: 
      Serial ports 
        "COM5"
  
  *Tools
    Serial Monitor
      Baud rate "115200"
