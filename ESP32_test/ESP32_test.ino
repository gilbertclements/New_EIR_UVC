/*******************************************************************************************************
  SparkFun   
  Order product:          https://www.sparkfun.com/products/15663
  Hookup Guide:           https://learn.sparkfun.com/tutorials/esp32-thing-plus-hookup-guide
  ESP32-WROOM Datasheet:  https://cdn.sparkfun.com/assets/learn_tutorials/8/5/2/esp32-wroom-32_datasheet_en.pdf
  
  Program: ESP32 Test hall sensor, push button PIN_0 and LED PIN_13
    setup complete when flashing stobe
    push button_0 and PIN_13 LED goes off
  
  Setup instructions
  File
    Preferences
      Additional Boards Manager URLs: 
  https://dl.espressif.com/dl/package_esp32_index.json
  click here to install-> http://boardsmanager/All#ESP32 by Espressif Systems

 * Tools
    Boards
      ESP32 Arduino
        "Adafruit ESP32 Feather"
  Upload speed: "921600"
  Flash Frequincy: "80MHz"
  Partion Scheme: "Default"
  Core Debug Level: "None"
*******************************************************************************************************/
#include <WiFi.h>                                               // Click here to get the library: http://librarymanager/All#Blynk_Async_ESP32_BT_WF
#include "time.h"                                               // Click here to get the library: http://librarymanager/All#Blynk_Async_ESP32_BT_WF

/////////////////// WiFi ID, pwd, port number. ///////////////////
const char* ssid = "ATT37FMI4R";                                // Replace wifi network id!
const char* pwd  = "password";                                  // Replace wifi password!
WiFiServer server(80);  // port                                 // 
/////////////////// WiFi ID, pwd, port number. ///////////////////

int ESP_BUTTON_PIN = 0;                                                               // pin number of the push button is 0 on ESP32
int BUTTON_State = 0;                                                                 // variable for reading the pushbutton status

void setup() {///////////////////////setup////////////////////////////
  Serial.begin(115200);                                                               // baud rate serial monitor
  Serial.print("Magnetic field detected by Hall Effect sensor is ");
  Serial.println(String(hallRead()));                                                 // Magnetic field sensed by Hall sensor

  pinMode(LED_BUILTIN, OUTPUT);                                                       // initialize the LED pin as an output, LED pin 13 on ESP32:
  pinMode(ESP_BUTTON_PIN, INPUT);                                                     // initialize the pushbutton pin as an input:
  
  Serial.print("Connecting to ");  // We start by connecting to a WiFi network
  Serial.print(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println(".");
    }
    Serial.print("WiFi connected!\nIP address:\t");
    Serial.println(WiFi.localIP());
    server.begin();
  
  for (byte count = 0; count < 5; count++) {                                          // short strobe confirm i2c communication
    digitalWrite(LED_BUILTIN, HIGH);                                                  // turn the LED on (HIGH is the voltage level)
    delay(50);                                                                        // wait for a few msec
    digitalWrite(LED_BUILTIN, LOW);                                                   // turn the LED off by making the voltage LOW
    delay(50); }                                                                      // wait for a few msec
}
int value = 0;
int Toggle_LED = 0;

void loop() {////////////////////////loop///////////////////////////
 WiFiClient client = server.available();                                     // listen for incoming clients
  if (client) {                                                              // if you get a client,
    Serial.println("New Client.");                                           // then print a message out the serial port
    String currentLine = "";                                                 // make a String to hold incoming data from the client
    while (client.connected()) {                                             // loop while the client's connected
      if (client.available()) {                                              // if there's bytes to read from the client,
        char c = client.read();                                              // then read a byte, then
        Serial.write(c);                                                     // print it out the serial monitor
        if (c == '\n') {                                                     // if the byte is a newline character
          if (currentLine.length() == 0) {                                 // if the current line is blank, you got two newline characters in a row.
            client.println("HTTP/1.1 200 OK");                             // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) and a content-type so the client knows what's coming, then a blank line:
            client.println("Content-type:text/html\n");

            client.print("ESP32 Built-in LED Web server!<br>");            // the content of the HTTP response follows the header:
            client.print("<a href=\"/ON\"> <button class=\"button\">ON!</button></a>");
            client.print("<a href=\"/OFF\"><button class=\"button\">OFF</button></a><br>");
            client.print("Click <a href=\"/\">here</a> to return HOME.<br>");

            client.println();                                             // The HTTP response ends with another blank line:
            break;                                                        // break out of the while loop:
          } else {                                                        // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {                                           // if you got anything else but a carriage return character,
          currentLine += c;                                               // add it to the end of the currentLine
        }

        if (currentLine.endsWith("GET /ON")) {                            // Check to see if the client request was "GET /ON" or "GET /OFF":
          Toggle_LED = 1;                                                 // GET /ON turns the LED on
        }
        if (currentLine.endsWith("GET /OFF")) {
          Toggle_LED = 0;                                                 // GET /OFF turns the LED off
        }
      }
    }
    client.stop();    // close the connection:
    Serial.println("Client Disconnected.");
  }
    BUTTON_State = digitalRead(ESP_BUTTON_PIN);                           // read the state of the pushbutton value:
  if (BUTTON_State == LOW) {                                              // if pushbutton is pressed then LED is toggled on or off:
    Toggle_LED = !Toggle_LED;
    delay(1000);
  //  Serial.println(Toggle_LED);
    Serial.println(String(hallRead()));                                   // Magnetic field sensed by Hall sensor
  }
  if (Toggle_LED == 1) {                                                  // if toggled:
    digitalWrite(LED_BUILTIN, HIGH);                                      // turn LED on:
  } else if (Toggle_LED == 0) {
    digitalWrite(LED_BUILTIN, LOW);                                       // turn LED off:
  }
}
