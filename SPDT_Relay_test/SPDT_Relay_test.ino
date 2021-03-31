/*

*/

#include "SparkFun_Qwiic_Relay.h"
#include <Wire.h>
#define RELAY_ADDR 0x18 // Alternate address 0x19 for more than one relay

Qwiic_Relay relay(RELAY_ADDR); 
int buttonPin = 0;
int Button_State = 0;
int Button_toggle = 0;

void setup(){ ///////////////////////setup////////////////////////////
  Serial.begin(115200);
  Wire.begin(); 
  
  if(!relay.begin())
    Serial.println("Unable to communicate with Relay. Check Quiic connection.");
  float version = relay.singleRelayVersion();
  Serial.print("Firmware Version: ");
  Serial.println(version);
  pinMode(buttonPin, INPUT);

  relay.turnRelayOn();   // Let's turn on the relay...
  delay(500);  
  relay.toggleRelay(); 
  delay(500);  
  relay.toggleRelay(); 
  delay(500);  
  relay.turnRelayOff();   // Let's turn that relay off...
  delay(500);

  Serial.print("The Relay is now ");  // Is the relay on or off?
  int Relay_State = relay.getState();
  if(Relay_State == 1)
    Serial.print("On!");
  else if(Relay_State == 0)
    Serial.print("Off!");
}

void loop(){ ///////////////////////loop////////////////////////////
  
}
