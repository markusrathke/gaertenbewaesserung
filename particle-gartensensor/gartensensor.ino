/******************************************************************************
 * Gartensensor.ino
 * 
 * Garten-Sensor Programm
 * zur Temperatur- und Feuchtigskeitsmesseung
 * inklusive Ausgabe auf OLED-Display
 * 
 * Autor: Markus Rathke
 * Datum: 29.12.2017
 * 
 * Hardware Connections:
    
    MicroOLED ------------- Photon
      GND ------------------- GND
      VDD ------------------- 3.3V (VCC)
    D1/MOSI ----------------- A5 (don't change)
    D0/SCK ------------------ A3 (don't change)
      D2
      D/C ------------------- D6 (can be any digital pin)
      RST ------------------- D7 (can be any digital pin)
      CS  ------------------- A2 (can be any digital pin)
 * 
*******************************************************************************/

#include <SparkFunMicroOLED.h>
#include <SHT1x.h>

/////////////////////////
// Startup Definitions //
/////////////////////////
// STARTUP(WiFi.selectAntenna(ANT_EXTERNAL)); // use the external antenna (via u.FL)

//////////////////////////
// MicroOLED Definition //
//////////////////////////
#define PIN_RESET D7  // Connect RST to pin 9 (req. for SPI and I2C)
#define PIN_DC    D7  // Connect DC to pin 8 (required for SPI)
#define PIN_CS    A2 // Connect CS to pin 10 (required for SPI)
// #define DC_JUMPER 0
// Also connect pin 13 to SCK and pin 11 to MOSI

//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////
// Declare a MicroOLED object. The parameters include:
// 1 - Mode: Should be either MODE_SPI or MODE_I2C
// 2 - Reset pin: Any digital pin
// 3 - D/C pin: Any digital pin (SPI mode only)
// 4 - CS pin: Any digital pin (SPI mode only, 10 recommended)
// MicroOLED oled(MODE_SPI, PIN_RESET, PIN_DC, PIN_CS);
MicroOLED oled;

// I2C is great, but will result in a much slower update rate. The
// slower framerate may be a worthwhile tradeoff, if you need more
// pins, though.

////////////////////////////////
// Hum+Temp-Sensor Definition //
////////////////////////////////
#define DATA_PIN  D3
#define CLOCK_PIN D4
SHT1x htsensor1(DATA_PIN, CLOCK_PIN);
String S1_tempC;
String S1_humi;

//////////////////////
// Debug Definition //
//////////////////////
int led1 = D2;
int sleepJmp = D0;

void setup() {
    // Initialize Debug PINs
    pinMode(sleepJmp, INPUT_PULLDOWN);
    pinMode(led1, OUTPUT);
    digitalWrite(led1, HIGH);
    
    // Before you can start using the OLED, call begin() to init
    // all of the pins and configure the OLED.
    oled.begin();
    
    // clear(ALL) will clear out the OLED's graphic memory.
    // clear(PAGE) will clear the Arduino's display buffer.
    oled.clear(ALL);  // Clear the display's memory (gets rid of artifacts)
    
    // To actually draw anything on the display, you must call the
    // display() function. 
    oled.display();
    delay(150);
    digitalWrite(led1, LOW);
}

void loop() {
    digitalWrite(led1, HIGH);
    
    // real temperature and humidity
    S1_humi = String(htsensor1.readHumidity(), 1);
    S1_tempC = String(htsensor1.readTemperatureC(), 1);
    
    // publish temperature and humidity to particle cloud
    Particle.publish("S1_HumiTempC", S1_humi + ";" + S1_tempC);
    
    oled.clear(PAGE);
    oled.setFontType(0);  // Set the text to small (10 columns, 6 rows worth of characters).
    // oled.setFontType(1);  // Set the text to medium (6 columns, 3 rows worth of characters).
    // oled.setFontType(2);  // Set the text to medium/7-segment (5 columns, 3 rows worth of characters).
    // oled.setFontType(3);  // Set the text to large (5 columns, 1 row worth of characters).
    
    oled.setCursor(0, 0);  // Set the text cursor to the upper-left of the screen.
    oled.print("Garten-"); // Print a const string
    oled.setCursor(0, 8);
    oled.print("sensor");
    
    oled.setCursor(0, 16);
    oled.print("----------");
    
    oled.setCursor(0, 24);
    oled.print("Humi: ");
    oled.print(S1_humi);
    
    oled.setCursor(0, 32);
    oled.print("Temp: ");
    oled.print(S1_tempC);
    
    oled.display(); // Draw to the screen
    delay(1000);
    
    digitalWrite(led1, LOW);
    delay(1000);
    
    // Enter Sleep Mode if Sleep Jumper is set to HIGH
    // otherwise wait for 60 seconds
    if(digitalRead(sleepJmp) == HIGH) {
        digitalWrite(led1, HIGH);
        delay(100);
        digitalWrite(led1, LOW);
        delay(100);
        digitalWrite(led1, HIGH);
        delay(100);
        digitalWrite(led1, LOW);
        
        // Sleep system for 5 minutes or interupt via A0
        System.sleep(A0, RISING, 300);
    } else {
        delay(30000);
    }
}
