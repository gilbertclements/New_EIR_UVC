/********************************************************************************************************
  SparkFun VEML6075       UV sensor
  Order product:          https://www.sparkfun.com/products/15089
  Hookup Guide:           https://learn.sparkfun.com/tutorials/qwiic-uv-sensor-veml6075-hookup-guide
  Datasheet:              https://cdn.sparkfun.com/assets/3/c/3/2/f/veml6075.pdf
  Library:                https://github.com/sparkfun/SparkFun_VEML6075_Arduino_Library/archive/master.zip
  Program:                Test read UVA, UVB, calculate UVindex
********************************************************************************************************/
///////////////////////initialize////////////////////////////
#include <SparkFun_VEML6075_Arduino_Library.h>                          // Click here to get the library: http://librarymanager/All#SparkFun_VEML6075 
#include <Wire.h>

VEML6075 UV_Sensor;                                                     // Create a VEML6075 object

void setup(){ ///////////////////////setup////////////////////////////
  Serial.begin(115200);                                                 //Communicate with serial montor
  Wire.begin();
  if (!UV_Sensor.begin()) {                                             // confirm communication on serial monitor
    Serial.println("Unable to communicate with VEML6075. Check Quiic connection.");
    while (1);
  }
  Serial.print("Raw UVA: "); Serial.println(UV_Sensor.rawUva());
  Serial.print("Raw UVB: "); Serial.println(UV_Sensor.rawUvb());
  Serial.print("Raw IR: ");  Serial.println(UV_Sensor.irCompensation());
  Serial.print("UVA:\t");    Serial.println(UV_Sensor.a());
  Serial.print("UVB:\t");    Serial.println(UV_Sensor.b());
  Serial.print("UVIndex:");  Serial.println(UV_Sensor.index());
  
  pinMode(LED_BUILTIN, OUTPUT);                                                       // initialize the LED pin as an output, LED pin 13 on ESP32:
  for (byte count = 0; count < 5; count++) {                                          // short strobe confirm i2c communication
    digitalWrite(LED_BUILTIN, HIGH);                                                  // turn the LED on (HIGH is the voltage level)
    delay(50);                                                                        // wait for a few msec
    digitalWrite(LED_BUILTIN, LOW);                                                   // turn the LED off by making the voltage LOW
    delay(50); }                                                                      // wait for a few msec
}

void loop(){ ///////////////////////loop////////////////////////////
  // Use the uva, uvb, and index functions to read calibrated UVA and UVB values and a calculated UV index value between 0-11.
  Serial.println(F("\nUVA\tUVB\tUVIndex"));            // Show header    
  for (byte count = 0; count < 15; count++) {                                            // Loop
    Serial.print(UV_Sensor.uva());
    Serial.print("\t");
    Serial.print(UV_Sensor.uvb());
    Serial.print("\t");
    Serial.println(UV_Sensor.index());
    delay(1000*5);  //Wait 5 second
  }
}
