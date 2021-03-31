/*

  SGP30 Datasheet: https://cdn.sparkfun.com/assets/c/0/a/2/e/Sensirion_Gas_Sensors_SGP30_Datasheet.pdf
  https://www.sparkfun.com/products/14813
  SGP30 IAQ Sensor   https://learn.sparkfun.com/tutorials/sparkfun-air-quality-sensor---sgp30-qwiic-hookup-guide
  https://github.com/sparkfun/SparkFun_SGP30_Arduino_Library/archive/master.zip

*/

#include "SparkFun_SGP30_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_SGP30
#include <Wire.h>

SGP30 SGP30_Sensor; //create an object of the SGP30 class

void setup() {  ///////////////////////setup////////////////////////////
  Serial.begin(115200);
  Wire.begin();
  if (!SGP30_Sensor.begin()) {
    Serial.println("Unable to communicate with SGP30. Check Quiic connection.");
    while (1);
  }
  SGP30_Sensor.initAirQuality();
  SGP30_Sensor.measureRawSignals();  //get raw values for H2 and Ethanol
  Serial.print("Raw H2: ");        Serial.print(SGP30_Sensor.H2);
  Serial.print("\tRaw Ethanol: "); Serial.println(SGP30_Sensor.ethanol);
  SGP30_Sensor.measureAirQuality();  //measure CO2 and TVOC levels
  Serial.print("CO2: ");           Serial.print(SGP30_Sensor.CO2);
  Serial.print(" ppm\tTVOC: ");    Serial.print(SGP30_Sensor.TVOC);Serial.println(" ppb");
 
  pinMode(LED_BUILTIN, OUTPUT);                                                       // initialize the LED pin as an output, LED pin 13 on ESP32:
  for (byte count = 0; count < 5; count++) {                                          // short strobe confirm i2c communication
    digitalWrite(LED_BUILTIN, HIGH);                                                  // turn the LED on (HIGH is the voltage level)
    delay(50);                                                                        // wait for a few msec
    digitalWrite(LED_BUILTIN, LOW);                                                   // turn the LED off by making the voltage LOW
    delay(50); }                                                                      // wait for a few msec
}

void loop() { ///////////////////////loop////////////////////////////
  //First fifteen readings will be baseline CO2: 400 ppm  TVOC: 0 ppb
  //measure CO2 and TVOC levels
  SGP30_Sensor.measureAirQuality();
  Serial.print("CO2: ");           Serial.print(SGP30_Sensor.CO2);
  Serial.print(" ppm\tTVOC: ");    Serial.print(SGP30_Sensor.TVOC); Serial.println(" ppb");
  delay(1000*5); //Wait 5 second
}
