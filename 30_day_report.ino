/******************************************************************************************************************************************
  More at
  ESP32              https://www.sparkfun.com/products/15663
  SGP30 IAQ Sensor   https://learn.sparkfun.com/tutorials/sparkfun-air-quality-sensor---sgp30-qwiic-hookup-guide
  UV light Sensor    https://learn.sparkfun.com/tutorials/qwiic-uv-sensor-veml6075-hookup-guide
  SPDT RELAY         https://learn.sparkfun.com/tutorials/qwiic-single-relay-hookup-guide
  BME680 AQIndex     https://learn.sparkfun.com/tutorials/sparkfun-environmental-sensor-breakout---bme680-qwiic-hookup-guide
  
  File
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
******************************************************************************************************************************************/
#include <Wire.h>
#include "time.h"                                      // Click here to get the library: http://librarymanager/All#Blynk_Async_ESP32_BT_WF

// Memory where data can be stored even when power is off.
#include <EEPROM.h>                                    // https://www.arduino.cc/en/Reference/EEPROM

// WiFi ID, pwd, port number.
#include <WiFi.h>                                      // Click here to get the library: http://librarymanager/All#Blynk_Async_ESP32_BT_WF
const char* ssid = "ATT37FMI4R";
const char* pwd  = "password";

WiFiServer server(80);  // port

// Adafruit Unified Sensor (used for all Adafruit sensors)
#include <Adafruit_Sensor.h>                                    // Click here to get the library: http://librarymanager/All#Adafruit_Sensor

// Sensor of temperature, pressure, humidity.
#include "Adafruit_BME680.h"                                    // Click here to get the library: http://librarymanager/All#Adafruit_BME680

// Turn on or off high voltage device (110V).
#include "SparkFun_Qwiic_Relay.h"                               // Click here to get the library: http://librarymanager/All#SparkFun_SGP30

// TVOC and CO2 sensors.
#include "SparkFun_SGP30_Arduino_Library.h"                     // Click here to get the library: http://librarymanager/All#SparkFun_Qwiic_Relay

// UV sensor.
#include <SparkFun_VEML6075_Arduino_Library.h>                  // Click here to get the library: http://librarymanager/All#SparkFun_VEML6075

// Instantiate device objects.
#define RELAY_ADDR 0x18                                         // Alternate address 0x19
Qwiic_Relay relay(RELAY_ADDR);

Adafruit_BME680 bme;  // Pressure, Temperature, Humidity

SGP30 tvoc_co2;

VEML6075 uv;

// Control and operating variables.
unsigned long timeout = 2000;  // assume WiFi client message is done if no more characters come in this number of milliseconds
int  client_refresh   =    4;  // seconds between refresh of client (minimum=3)
int  header_interval  =   20;  // header rewritten to serial output after this many lines
int  serial_interval  =    4;  // seconds between serial output of sensor values  (minimum=2)
uint16_t relay_on_tvoc  = 40;  // relay on AUTO turns on  if TVOC is above this value
uint16_t relay_off_tvoc = 30;  // relay on AUTO turns off if TVOC is below this value

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = (-8) * 3600;  // Pacific is 8 hours behind GMT
const int   daylightOffset_sec = 3600;

int  iheader = 99999;  // begin serial output by writing header
int  irelauto = 0;  // relay state based on sensor values, 1=on, 0=off
char message[4096];
char relON[12];
char relOFF[12];
char relAUTO[12];
char refMANUAL[12];
char refAUTO[12];
char meta_refresh[48];
char relay_state[8];
unsigned long currentTime, lastChrTime, msecNow, msecInc;
unsigned long nextSerialTime = 0;
unsigned long msecPrev = 0;
char month[12][4];
union { uint32_t store32; char storec[4]; } store32c;
uint32_t lightsec = 0;  // time (sec) light intensity has been above minimum
float uvindexmin;


void setup()
{
    Wire.begin();
    Serial.begin(115200);                                                              // baud rate serial monitor

    // Initialize relay and sensors.
    if(!relay.begin()) Serial.println("Relay did not initialize.");
    else               Serial.println("Relay initialized.");
    relay.turnRelayOff();
    strcpy(relOFF, " selected");
    relON[0]   = '\0';
    relAUTO[0] = '\0';

    if(!bme.begin()) Serial.println("BME680 did not initialize.");
    else             Serial.println("BME680 initialized.");
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms

    if(!tvoc_co2.begin()) Serial.println("SGP30 did not initialize.");
    else                  Serial.println("SGP30 initialized.");
    tvoc_co2.initAirQuality();

    if(!uv.begin()) Serial.println("VEML6075 did not initialize.");
    else            Serial.println("VEML6075 initialized.");

    // Start with manual refresh.
    meta_refresh[0] = '\0';
    strcpy(refMANUAL, " selected");
    refAUTO[0] = '\0';

    // Connect to Wi-Fi network with SSID and password.
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pwd);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    // Initialize time clock.
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    strcpy(month[0], "JAN"); strcpy(month[ 1], "FEB"); strcpy(month[ 2], "MAR");
    strcpy(month[3], "APR"); strcpy(month[ 4], "MAY"); strcpy(month[ 5], "JUN");
    strcpy(month[6], "JUL"); strcpy(month[ 7], "AUG"); strcpy(month[ 8], "SEP");
    strcpy(month[9], "OCT"); strcpy(month[10], "NOV"); strcpy(month[11], "DEC");

    // Initialize light time using value stored in EEPROM if it exists.
    char chr0 = EEPROM.read(0);
    char chr5 = EEPROM.read(5);
    if(chr0 == ':'  &&  chr5 == ';')
    {
        store32c.storec[0] = EEPROM.read(1);
        store32c.storec[1] = EEPROM.read(2);
        store32c.storec[2] = EEPROM.read(3);
        store32c.storec[3] = EEPROM.read(4);
        lightsec = store32c.store32;
    }

    // Calculate uvindex minimum based on bulb wattage.
    uvindexmin = 3;  // This needs to be updated.

    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("Local IP address:  ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop()
{
    int    i, isetrel, isetcli, isetser, isethed, isetzro, isetron, isetrof, len;
    int    iyear, imonth, imday, ihour, iminute;
    char   chr;
    char   refbutton[256], relay_control[512];
    char   temp[8], datenow[16], timenow[16], monthday[16], thisyear[16];
    float  uva, uvb, uvindex;
    double press, air_resistance, resistanceAQI, humidityAQI, AQI;
    struct tm timeinfo;
    uint8_t relayStateNow  = 0;
    uint8_t relayStatePrev = 0;
    uint32_t bulbdays, bulbhours, bulbminutes, bulbhourstot;

    // Get readings from sensors.
    bme.performReading();
    tvoc_co2.measureAirQuality();
    if(relay.getState() == 0) strcpy(relay_state, "OFF");
    else                      strcpy(relay_state, "ON" );
    press   = (double)bme.pressure;
    press  /= 1000.0;
    uva     = uv.a();
    uvb     = uv.b();
    uvindex = uv.index();

    // Calculate air quality index (AQI) from air resistance and humidity.
    // AQI:          0         500
    // resistance:  50K         5K        (75% of AQI)
    // humidity:    40%    100% or 0%     (25% of AQI)
    air_resistance = bme.gas_resistance;

    for(i=0; i<9; i++)
    {
        delay(10);
        air_resistance += (double)bme.readGas();
    }
    air_resistance = air_resistance / 10.0; // 0.1 second average
    if(air_resistance > 50000) air_resistance = 50000;
    if(air_resistance <  5000) air_resistance =  5000;

    resistanceAQI = (50000.0 - air_resistance) * (500.0 / 45000.0);

    if(bme.humidity > 40.0) humidityAQI = 8.33333 * (bme.humidity - 40.0);
    else                    humidityAQI = 12.5 * (40 - bme.humidity);

    AQI = (0.75 * resistanceAQI) + (0.25 * humidityAQI);

    // If relay is on, increment and save time that light intensity is above minimum.
    relayStateNow = relay.getState();
    msecNow       = millis();

    if(relayStateNow != 0)
    {
        if(relayStatePrev != 0)
        {
            if(uvindex >= uvindexmin)
            {
                if(msecNow >= msecPrev) msecInc = msecNow - msecPrev;
                else                    msecInc = msecNow + (0xffffffff - msecPrev);  // when millis overflows
                lightsec += (msecInc / 1000);
                store32c.store32 = lightsec;
                EEPROM.write(0, ':');
                EEPROM.write(1, store32c.storec[0]);
                EEPROM.write(2, store32c.storec[1]);
                EEPROM.write(3, store32c.storec[2]);
                EEPROM.write(4, store32c.storec[3]);
                EEPROM.write(5, ';');
            }
        }
    }
    relayStatePrev = relayStateNow;
    msecPrev       = msecNow;
    bulbdays       = lightsec / 86400;
    bulbhours      = (lightsec - (86400 * bulbdays)) / 3600;
    bulbminutes    = (lightsec - (86400 * bulbdays) - (3600 * bulbhours)) / 60;
    bulbhourstot   = lightsec /  3600;

    // Get date and time.
    if(getLocalTime(&timeinfo) != 0)
    {
        iyear   = timeinfo.tm_year + 1900;
        imonth  = timeinfo.tm_mon;
        imday   = timeinfo.tm_mday;
        ihour   = timeinfo.tm_hour;
        iminute = timeinfo.tm_min;
        sprintf(monthday, "%s %02d", month[imonth], imday);
        sprintf(thisyear, "%d", iyear);
        sprintf(datenow,  "%s %02d, %d", month[imonth], imday, iyear);
        sprintf(timenow,  "%0d:%02d", ihour, iminute);
    }
    else
    {
        datenow[0] = '\0';
        timenow[0] = '\0';
    }

    // Write header for sensor readings.
    if(iheader >= header_interval)
    {
        sprintf(message, "%s   C     kPa      %%      ppb     ppm     hrs\n"
                         " %s   Temp  Press    Hum    TVOC     CO2    Bulb   AQI     UVa     UVb   UVidx  Relay\n", monthday, thisyear);
        Serial.print(message);
        iheader = 0;
    }

    // Write sensor values and time to serial output.
    currentTime = millis();
    if(currentTime >= nextSerialTime)
    {
        sprintf(message, "%s %6.1f %6.1f %6.1f %7d %7d %7d %5.0f %7.1f %7.1f %7.1f   %s\n",
                timenow, bme.temperature, press, bme.humidity,
                tvoc_co2.TVOC, tvoc_co2.CO2, bulbhourstot, AQI, uva, uvb, uvindex, relay_state);
        Serial.print(message);
        nextSerialTime = currentTime + (1000 * serial_interval);
        iheader++;
    }

    // Listen for incoming clients
    WiFiClient client = server.available();

    // If a client is available and connected, read data coming from it.
    if(client)
    {
        currentTime = millis();
        lastChrTime = currentTime;
        len = 0;

        while(client.connected())
        {
            if(client.available() != 0)  // See if character ready to read
            {
                chr = client.read();
                message[len] = chr;
                len++;
                if(chr == '\n'  &&  message[len-2] == '\n') break;
            }
            currentTime = millis();
            if((currentTime - lastChrTime) > timeout) break;
        }
        message[len] = '\0';

        // Analyze message from client. Look for selections from dropdown menus.
        isetrel = 0;
        isetcli = 0;
        isetser = 0;
        isethed = 0;
        isetzro = 0;
        isetron = 0;
        isetrof = 0;

        for(i=0; i<len; i++)
        {
            if(message[i] == '?')
            {
                if(isetrel == 0  &&  memcmp(&message[i], "?relay=ON"  , 9) == 0)
                {
                    relay.turnRelayOn();
                    strcpy(relay_state, "ON");
                    strcpy(relON, " selected");
                    relOFF[0]  = '\0';
                    relAUTO[0] = '\0';
                    isetrel    = 1;
                    irelauto   = 0;
                }
                if(isetrel == 0  &&  memcmp(&message[i], "?relay=OFF" , 9) == 0)
                {
                    relay.turnRelayOff();
                    strcpy(relay_state, "OFF");
                    strcpy(relOFF, " selected");
                    relON[0]   = '\0';
                    relAUTO[0] = '\0';
                    isetrel    = 1;
                    irelauto   = 0;
                }
                if(isetrel == 0  &&  memcmp(&message[i], "?relay=AUTO", 9) == 0)
                {
                    strcpy(relAUTO, " selected");
                    relON[0]   = '\0';
                    relOFF[0]  = '\0';
                    isetrel    = 1;
                    irelauto   = 1;
                }

                if(isetcli == 0  &&  memcmp(&message[i], "?client_refresh=",  16) == 0)
                {
                    memcpy(temp, &message[i+16], 7);
                    isetcli = 1;
                    temp[7] = '\0';
                    client_refresh = atoi(temp);
                    if(client_refresh < 3) client_refresh = 3;
                }

                if(isetser == 0  &&  memcmp(&message[i], "?serial_interval=", 16) == 0)
                {
                    memcpy(temp, &message[i+17], 7);
                    isetser = 1;
                    temp[7] = '\0';
                    serial_interval = atoi(temp);
                    if(serial_interval < 3) serial_interval = 3;
                }

                if(isethed == 0  &&  memcmp(&message[i], "?header_interval=", 16) == 0)
                {
                    memcpy(temp, &message[i+17], 7);
                    isethed = 1;
                    temp[7] = '\0';
                    header_interval = atoi(temp);
                    if(header_interval < 2) client_refresh = 2;
                }

                if(isetzro == 0  &&  memcmp(&message[i], "?zero_bulb=", 10) == 0)
                {
                    isetzro = 1;
                    if(message[i+11]=='Z' && message[i+13]=='R' && message[i+12]=='E' && message[i+14]=='O')
                    {
                        EEPROM.write(0, ':');
                        EEPROM.write(1, 0x00);
                        EEPROM.write(2, 0x00);
                        EEPROM.write(3, 0x00);
                        EEPROM.write(4, 0x00);
                        EEPROM.write(5, ';');
                        lightsec = 0;
                    }
                }

                if(isetron == 0  &&  memcmp(&message[i], "?relay_on_tvoc=", 14) == 0)
                {
                    memcpy(temp, &message[i+15], 7);
                    isetron = 1;
                    temp[7] = '\0';
                    relay_on_tvoc = (uint16_t)atoi(temp);
                    if(relay_on_tvoc < relay_off_tvoc) relay_on_tvoc = relay_off_tvoc;
                }

                if(isetrof == 0  &&  memcmp(&message[i], "?relay_off_tvoc=", 14) == 0)
                {
                    memcpy(temp, &message[i+16], 7);
                    isetrof = 1;
                    temp[7] = '\0';
                    relay_off_tvoc = (uint16_t)atoi(temp);
                    if(relay_off_tvoc > relay_on_tvoc) relay_off_tvoc = relay_on_tvoc;
                }
            }
        }

        // Set relay based on sensor readings.
        relay_control[0] = '\0';

        if(irelauto != 0)
        {
            if(tvoc_co2.TVOC > relay_on_tvoc)
            {
                relay.turnRelayOn() ;
                strcpy(relay_state, "ON" );
            }
            else if(tvoc_co2.TVOC < relay_off_tvoc)
            {
                relay.turnRelayOff();
                strcpy(relay_state, "OFF");
            }

            sprintf(relay_control, "<br>"
                                   "<form>"
                                       "<label for=\"relay_on_tvoc\"><i class=\"fas fa-arrow-up\"></i> Relay on TVOC:</label>"
                                       "<input type=\"text\" id=\"relay_on_tvoc\" name=\"relay_on_tvoc\" size=\"4\" value=\"%d\"><br>"
                                   "</form>"
                                   "<br>"
                                   "<form>"
                                       "<label for=\"relay_off_tvoc\"><i class=\"fas fa-arrow-down\"></i> Relay off TVOC:</label>"
                                       "<input type=\"text\" id=\"relay_off_tvoc\" name=\"relay_off_tvoc\" size=\"4\" value=\"%d\"><br>"
                                   "</form>", relay_on_tvoc, relay_off_tvoc);
        }

        // Show refresh button if client refresh is greater than 10 seconds.
        if(client_refresh < 10)
        {
            refbutton[0] = '\0';
        }
        else
        {
            strcpy(refbutton, "<br><form>"
                              "<button style=\"background-color: #8844ff;\" type=\"submit\" "
                                       "name=\"manref\" value=\"REF\">REFRESH</button>"
                              "</form>");    // returned to server:  ?manref=REF
        }

        // Send web page to client.
        sprintf(message, "HTTP/1.1 200 OK\n"
                         "Server: TKE/0.0\n"
                         "Accept-Ranges: bytes\n"
                         "Connection: close\n"
                         "Content-Type: text/html\n"
                         "\n"
                         "<!DOCTYPE html>"
                         "<html style=\"font-family:verdana\">"
                         "<meta name=\"viewport\" content=\"initial-scale=1\">"
                         "<title>New-EIR UV-C</title>"
                         "<head>"
                         "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                         "<link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\" integrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr\" crossorigin=\"anonymous\">"
                         "<link rel=\"icon\" href=\"data:,\">"
                         "<style>"
                            "html {font-family: Arial; display: inline-block; text-align: center;}"
                            "p {  font-size: 1.2rem;}"
                            "body {  margin: 0;}"
                            ".topnav { overflow: hidden; background-color: #440088; color: white; font-size: 1.7rem; }"
                            ".content { padding: 20px; }"
                            ".card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(180,140,255,.5); }"
                            ".cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }"
                            ".reading { font-size: 2.8rem; }"
                            ".card.temperature { color: #0e7c7b; }"
                            ".card.humidity { color: #17bebb; }"
                            ".card.pressure { color: #4B1D3F; }"
                            ".card.gas { color: #440088; }"
                         "</style>"
                         "</head>"
                         "<br><br>"
                         "<body>"
                         "<div class=\"topnav\">"
                         "<h3>New-EIR UV-C Germicidal App</h3>"
                         "</div>"
                         "<div class=\"content\">"
                         "<div class=\"cards\">"
                         "<div class=\"card temperature\">"
                           "<h1>30-DAY Report<br>Welcome to:</h1>"
                           "<h2>Stinger's Cafe</h2>"
                           "<h3>6000 J St. Sacramento, CA <br> 95819-6055</h3>"
                           "<h4>This Business has air sanitized and monitored by <br> 'New-EIR Germicidal UV-C' <br> for the safety, health and comfort of your customers. Ozone Free!</h4>"
                           "<p style=\"font-size:10px\">Scan this QRcode to goto our webpage: <i class=\"fas fa-external-link-alt\"></i></p> <address><a href=\"https://sites.google.com/view/new-eiriaq\">sites.google.com/view/new-eiriaq</a></address><br>"
                           "<img src=\"https://docs.google.com/drawings/d/e/2PACX-1vTa5l5IPNuLwZHn7b-yIDdKVXMi40wL0mT7YrQBGJmv7PBa23s-kqaKXxT_WkLeiL65R8BQA4RPw0kq/pub?w=240&amp;h=295\"><br>"
                           "<p style=\"font-size:10px\">Keep this report on display for your customers' assurance of safety, health and comfort. Replace every week.</p><br>"
                         "</div>"
                         "<meta http-equiv=\"refresh\" content=\"%d\">"
                         "<div class=\"card pressure\">"
                           "<h1>Readings</h1>"
                             "<table class=\"center\">"
                               "<tbody><tr>"
                                 "<th class=\"fas fa-calendar-day\"></th>"
                                 "<td> %s </td>"
                                 "<th> %s </th>"
                               "</tr>"
                               "<tr>"
                                 "<td class=\"fas fa-thermometer-half\"></td>"
                                 "<td> Temperature (20-26 &deg;C)</td>"
                                 "<td>= %7.2f</td>"
                               "</tr>"
                               "<tr>"
                                 "<td class=\"fas fa-tachometer-alt\"></td>"
                                 "<td> Pressure (95-101 kPa) </td>"
                                 "<td>= %7.2f</td>"
                               "</tr>"
                               "<tr>"
                                 "<td class=\"fas fa-tint\"></td>"
                                 "<td> Humidity (30 - 45 &percnt;) </td>"
                                 "<td>= %7.2f</td>"
                               "</tr>"
                               "<tr>"
                                 "<td class=\"fas fa-poop\"></td>"
                                 "<td>TVOC (0 - 500 ppb) </td>"
                                 "<td>= %7.0f</td>"
                               "</tr>"
                               "<tr>"
                                 "<td class=\"fas fa-dizzy\"></td>"
                                 "<td> CO2 (400-1000 ppm) </td>"
                                 "<td>= %7.0f</td>"
                               "</tr>"
                             "<tr>"
                               "<td class=\"fas fa-air-freshener\"></td>"
                                 "<td> AQI (0 - 50 = Good) </td>"
                               "<td>= %7.0f</td>"
                             "</tr>"
                           "</tbody></table><br>"
                             "<iframe width=\"300\" height=\"260\" seamless frameborder=\"0\" scrolling=\"no\" src=\"https://docs.google.com/spreadsheets/d/e/2PACX-1vR9FhxYm7vf5wwHYgRqjywVEX_XNbWSjp_XNNh6TO3L29nXAS0WXhmC-SytIRtHyu1uat2mJoaybN9d/pubchart?oid=2126194809&amp;format=interactive\"></iframe>"
                             "<iframe width=\"300\" height=\"260\" seamless frameborder=\"0\" scrolling=\"no\" src=\"https://docs.google.com/spreadsheets/d/e/2PACX-1vR9FhxYm7vf5wwHYgRqjywVEX_XNbWSjp_XNNh6TO3L29nXAS0WXhmC-SytIRtHyu1uat2mJoaybN9d/pubchart?oid=350343&amp;format=interactive\"></iframe>"
                         "</div>"
                         "</div>"
                         "<br><br>"
                         "<div>"
                         "<button style=\"background-color: #8844ff;\" onclick=\"window.print()\">Print 30-Day Report</button><br><br>"
                         "</div>"
                         "<br><br>"
                         "<div style=\"color: #17bebb;\">"
                         "<form>"
                             "<label for=\"relay\"><i class=\"fas fa-toggle-on\"></i> Relay:  </label>"
                             "<select id=\"relay\" name=\"relay\" onchange=\"this.form.submit()\">"
                                 "<option%s>ON</option>"
                                 "<option%s>OFF</option>"
                                 "<option%s>AUTO</option>"
                             "</select>"
                         "</form>"
                             "<i class=\"far fa-lightbulb\"></i> bulb on: %d days %d hours %d minutes<br>"
                         "<br>"
                         "<form>"
                         "<label for=\"client_refresh\"><i class=\"fas fa-clock\"></i> Seconds between WiFi refresh:</label>"
                         "<input type=\"text\" id=\"client_refresh\" name=\"client_refresh\" size=\"4\" value=\"%d\"><br>"
                         "</form>"
                         "<br>"
                         "<form>"
                         "<label for=\"serial_interval\"><i class=\"fas fa-hourglass-half\"></i> Seconds between serial output:</label>"
                         "<input type=\"text\" id=\"serial_interval\" name=\"serial_interval\" size=\"4\" value=\"%d\"><br>"
                         "</form>"
                         "<br>"
                         "<form>"
                         "<label for=\"header_interval\"><i class=\"fas fa-list\"></i> Lines between serial output headers:</label>"
                         "<input type=\"text\" id=\"header_interval\" name=\"header_interval\" size=\"4\" value=\"%d\"><br>"
                         "</form>"
                         "<br>"
                         "<form>"
                         "<label for=\"zero_bulb\"><i class=\"fas fa-question\"></i> Password to zero bulb time:</label>"
                         "<input type=\"text\" id=\"zero_bulb\" name=\"zero_bulb\" size=\"4\" value=\"\">"
                         "</form>"
                         "<br>"
                         "%s"
                         "%s"
                         "<br>"
                         "</div>"
                         "</div>"
                         "</body>"
                         "</html>", client_refresh, datenow, timenow,
                                    bme.temperature, press, bme.humidity,
                                    tvoc_co2.TVOC, tvoc_co2.CO2, AQI, uvindex,
                                    relON, relOFF, relAUTO, bulbdays, bulbhours, bulbminutes,
                                    client_refresh, serial_interval, header_interval, relay_control, refbutton);

        client.println(message);

        // Close connection to the client
        client.stop();
    }
}
