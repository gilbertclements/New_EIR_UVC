/********************************************************************************************************
  SparkFun Relay  
  Product:        https://www.sparkfun.com/products/15093
  Hookup Guide:   https://learn.sparkfun.com/tutorials/qwiic-single-relay-hookup-guide
  Datasheet:      https://cdn.sparkfun.com/assets/5/e/e/d/f/3V_Relay_Datasheet_en-g5le.pdf
  Library:        https://github.com/sparkfun/SparkFun_Qwiic_Relay_Arduino_Library/archive/master.zip
  Program         tests SPDT relay with sample code
********************************************************************************************************/
///////////////////////initialize////////////////////////////
#include "SparkFun_Qwiic_Relay.h"                               // Click here to get the library: http://librarymanager/All#SparkFun_Qwiic_Relay
#include <Wire.h>
#define RELAY_ADDR 0x18                                         // Alternate address 0x19 for more than one relay

Qwiic_Relay relay(RELAY_ADDR); 

void setup(){ ///////////////////////setup////////////////////////////
  Serial.begin(115200);                                         // baud rate serial monitor
  Wire.begin(); 
  
  if(!relay.begin())
    Serial.println("Unable to communicate with Relay. Check Quiic connection.");
  float version = relay.singleRelayVersion();
  Serial.print("Firmware Version: ");
  Serial.println(version);
  pinMode(buttonPin, INPUT);

  relay.turnRelayOn();                                         // turn relay on...
  delay(500);  
  relay.toggleRelay();                                         // toggle on/off the relay...
  delay(500);  
  relay.toggleRelay();                                         // toggle on\off the relay...
  delay(500);  
  relay.turnRelayOff();                                        // turn relay off...
  delay(500);

  Serial.print("The Relay is now ");                           // confirm relay opperation on serial monitor
  int Relay_State = relay.getState();                          // Is the relay on or off?
  if(Relay_State == 1)                                        // if state is HIGH than relay is ON else state is LOW relay is OFF
    Serial.println("On!");
  else if(Relay_State == 0)
    Serial.println("Off!");
}

void loop(){ ///////////////////////loop////////////////////////////
  
}
