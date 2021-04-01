/********************************************************************************************************
  SparkFun BME680         Outdoor AQI sensor
  Order product:          https://www.sparkfun.com/products/16466 
  Hookup Guide:           https://learn.sparkfun.com/tutorials/sparkfun-environmental-sensor-breakout---bme680-qwiic-hookup-guide?_ga=2.200546990.1835296421.1617144529-249622734.1608670751
  BME680 Datasheet:       https://cdn.sparkfun.com/assets/8/a/1/c/f/BME680-Datasheet.pdf
  Library:                
  Program:                Test BME680 AQI Sensor prints Temp, relative humidity, air pressure
********************************************************************************************************/
///////////////////////initialize////////////////////////////
#include <Wire.h>
#include <Adafruit_Sensor.h>                               //Click here to get the library: http://librarymanager/All#Adafruit_Sensor
#include "Adafruit_BME680.h"                               //Click here to get the library: http://librarymanager/All#Adafruit_BME680

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME680 BME680_Sensor; // I2C

void setup(){  ////////////////////////////////setup///////////////////////////////
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("BME680 async test"));

  if (!BME680_Sensor.begin()) {
    Serial.println("Unable to communicate with BME680. Check Quiic connection.");
    while (1);
  }

  BME680_Sensor.setTemperatureOversampling(BME680_OS_8X);                             // Set up oversampling for temp
  BME680_Sensor.setHumidityOversampling(BME680_OS_2X);                                // Set up oversampling for humidity
  BME680_Sensor.setPressureOversampling(BME680_OS_4X);                                // Set up oversampling for pressuser
  BME680_Sensor.setIIRFilterSize(BME680_FILTER_SIZE_3);                               // Set up filter size
  BME680_Sensor.setGasHeater(320, 150);                                               // 320*C for 150 ms
  
  BME680_Sensor.performReading();
  Serial.println(BME680_Sensor.temperature);
  Serial.println(BME680_Sensor.humidity);
  Serial.println(BME680_Sensor.pressure);
  Serial.println(BME680_Sensor.gas_resistance);
  
  pinMode(LED_BUILTIN, OUTPUT);                                                       // initialize the LED pin as an output, LED pin 13 on ESP32:
  for (byte count = 0; count < 5; count++) {                                          // short strobe confirm i2c communication
    digitalWrite(LED_BUILTIN, HIGH);                                                  // turn the LED on (HIGH is the voltage level)
    delay(50);                                                                        // wait for a few msec
    digitalWrite(LED_BUILTIN, LOW);                                                   // turn the LED off by making the voltage LOW
    delay(50); }                                                                      // wait for a few msec
}

void loop(){  ////////////////////////////////loop///////////////////////////////
  Serial.println(F("\nTemp\xC2\xB0\x43\t%RH\tkPa\tAlt-m\tkOhms\tDew Point"));            // Show header    
  for (byte count = 0; count < 25; count++) {                                            // Loop
    Serial.print(BME680_Sensor.temperature);                                             // Temperature degrees C
    Serial.print("\t");
    Serial.print(BME680_Sensor.humidity);                                                // relative humidity
    Serial.print("\t");
    Serial.print(BME680_Sensor.pressure / 1000.0);                                       // Air pressure
    Serial.print("\t");
    Serial.print(BME680_Sensor.readAltitude(SEALEVELPRESSURE_HPA));                      // Altitude calulated from pressure     
    Serial.print("\t");
    Serial.print(BME680_Sensor.gas_resistance / 1000.0);                                 // Gas resistance                  
    Serial.print("\t");
    Serial.print((BME680_Sensor.temperature(9/5)+32) - (9/25)*(100 - BME680_Sensor.humidity)); //Aproximet Dew point
    Serial.println();
    delay(2000);
  }
}
