/*
  ESP32 Test hall sensor, push button PIN_0 and LED PIN_13
  setup complete with flashing stobe
  push button_0 and PIN_13 LED gose off
  
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
*/

int ESP_BUTTON_PIN = 0;                                                               // pin number of the push button is 0 on ESP32
int BUTTON_State = 0;                                                                 // variable for reading the pushbutton status

void setup() {///////////////////////setup////////////////////////////
  Serial.begin(115200);
  Serial.println(hallRead());                                                         // Magnetic field sensed by Hall sensor

  pinMode(LED_BUILTIN, OUTPUT);                                                       // initialize the LED pin as an output, LED pin 13 on ESP32:
  pinMode(ESP_BUTTON_PIN, INPUT);                                                     // initialize the pushbutton pin as an input:
  
  for (byte count = 0; count < 5; count++) {                                          // short strobe confirm i2c communication
    digitalWrite(LED_BUILTIN, HIGH);                                                  // turn the LED on (HIGH is the voltage level)
    delay(50);                                                                        // wait for a few msec
    digitalWrite(LED_BUILTIN, LOW);                                                   // turn the LED off by making the voltage LOW
    delay(50); }                                                                      // wait for a few msec
  Serial.println("ESP32 Thing Plus ready");                                           // confirm opperation to serial monitor
}

void loop() {////////////////////////loop///////////////////////////
  BUTTON_State = digitalRead(ESP_BUTTON_PIN);                                         // read the state of the pushbutton value:

  if (BUTTON_State == HIGH) {                                                         // if pushbutton is pressed buttonState = HIGH, than LED is on, else LED off:
    digitalWrite(LED_BUILTIN, HIGH);                                                  // turn LED on:
  } else {
    digitalWrite(LED_BUILTIN, LOW);                                                   // turn LED off:
  }
}
