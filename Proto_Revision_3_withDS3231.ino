/*
   date: 4/2/19
   Final Revision.
   Conditional Loop
   Interfaced with PT100 Temperature sensor, DS3231 RTC
   Pending work = to complete the serial port logic
 * */
#include <SD.h>

int timeInterval = 0;
int minutes = 1000;
int getSensorRate;
unsigned long sl = 1;

const int chipSelect = 53;
const int statusled = 6;
const int writefail = 7;
const int writeok = 8;

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);


#include <DS3231.h> //Library for RTC module (Download from Link in article)
DS3231  rtc(SDA, SCL);

#include <Adafruit_MAX31865.h>
// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 max = Adafruit_MAX31865(26, 24, 25, 23);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 max = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

byte inByte;

const int buttonPin = 2;    // the number of the pushbutton pin
const int ledPin = 8;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


void setup() {

  Serial.begin(9600);
  rtc.begin();
  max.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary


  pinMode(statusled, OUTPUT);
  pinMode(writefail, OUTPUT);
  pinMode(writeok, OUTPUT);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);


  //Initialize_SDcard();
  Serial.println("Enter time interval in Minutes");
  Serial.println("");
  Serial.println("            Office Name");
  Serial.println("             Address");
  Serial.println("");

  Serial.println("Instrument. No               " + String(rtc.getDateStr()));  //default
  //   Serial.println("Instrument. No               ");

  //Serial.println(__DATE__);
  Serial.println("");
  Serial.println("");
  Serial.println("Sl.No   Date          Time        Temperature      Humidity");

  u8g2.begin();
  u8g2.setDisplayRotation(U8G2_R2);


  u8g2.clearBuffer();         // clear the internal memory
  u8g2.setFont(u8g2_font_logisoso28_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  //setCursor(h,v)

  u8g2.drawStr(32, 28, "DLCS"); // write something to the internal memory
  u8g2.sendBuffer();         // transfer internal memory to the display
  delay(2000);

  u8g2.clearBuffer();         // clear the internal memory
  u8g2.setFont(u8g2_font_helvB12_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.setCursor(22, 14);
  u8g2.print("Ready For");  // write something to the internal memory

  u8g2.setCursor(45, 31);
  u8g2.print("Test");

  u8g2.sendBuffer();

    // The following lines can be uncommented to set the date and time
  rtc.setDOW(THURSDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(10 , 55, 10);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(21, 2, 2019);
}

void motor() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}


void getSensorVal()
{
  uint16_t rtd = max.readRTD();

  //Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  /*
    Serial.print("rtd = "); Serial.println(rtd);
    Serial.print("Ratio = "); Serial.println(ratio,8);
    Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
    Serial.print("Temperature = "); Serial.println(max.temperature(RNOMINAL, RREF),1);
  */

  //  float h = dht.readHumidity();
  Serial.println(String(sl) + "    " + String(rtc.getDateStr()) + "    " + String(rtc.getTimeStr()) + "       " + String(( max.temperature(RNOMINAL, RREF)-.04), 1) + " ºC");

  u8g2.clearBuffer();         // clear the internal memory
  u8g2.setFont(u8g2_font_helvB12_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  //setCursor(h,v)
  u8g2.setCursor(22, 14);

  u8g2.print("Temp :" + String(max.temperature(RNOMINAL, RREF), 1)); // write something to the internal memory
  u8g2.setCursor(22, 14);
  u8g2.sendBuffer();
  //u8g2.print("Humd :"+String(h,1));
  //u8g2.sendBuffer();         // transfer internal memory to the display

  if (max.temperature(RNOMINAL, RREF) > 40) {
    digitalWrite(9, HIGH);
  }
  else
    digitalWrite(9, LOW);
    //delay(1000);

}

void stopReading() {
  
    Serial.println("Test Completed -- Ready for Test");
    
    u8g2.clearBuffer();         // clear the internal memory
    u8g2.setFont(u8g2_font_helvB12_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall

    //setCursor(h,v)
      u8g2.setCursor(22,14);
    u8g2.print("Completed");  // write something to the internal memory

    u8g2.setCursor(45,31);
    u8g2.print("Test");
    u8g2.sendBuffer();

    delay(1500);

    u8g2.clearBuffer();         // clear the internal memory
    u8g2.setCursor(22,14);
    u8g2.print("Ready For");  // write something to the internal memory

    u8g2.setCursor(45,31);
    u8g2.print("Test");

    u8g2.sendBuffer();
  inByte = 0;
}


void loop() {
  // read the sensor:
  motor();
 if (Serial.available() > 0)
  {
    inByte = Serial.read();
  }
    switch (inByte) {
      case '1':
        getSensorVal();
        sl++;
        break;

      case '0':
        stopReading();
        break;


    
  }
}


