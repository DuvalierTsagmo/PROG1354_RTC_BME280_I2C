

/*
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp32-i2c-communication-arduino-ide/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "RTClib.h"

/***************************************************************************
  This is a library for the BMP280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Adafruit_BMP280.h>

#define BMP_SCK (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS (10)

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_BMP280 bmp; // I2C
// Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
// Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup()
{
    Serial.begin(9600);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    Serial.println(F("BMP280 Forced Mode Test."));

    // if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
    if (!bmp.begin(0x76))
    {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                         "try a different address!"));
        while (1)
            delay(10);
    }

    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (1)
            delay(10);
    }

    if (rtc.lostPower())
    {
        Serial.println("RTC lost power, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

    // When time needs to be re-set on a previously configured device, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    display.clearDisplay();
    display.setTextColor(WHITE);
}

void loop()
{
    // must call this to wake sensor up and get new measurement data
    // it blocks until measurement is complete
    if (bmp.takeForcedMeasurement())
    {
        // can now print out the new measurements
        Serial.print(F("Temperature = "));
        Serial.print(bmp.readTemperature());
        Serial.println(" *C");

        Serial.print(F("Pressure = "));
        Serial.print(bmp.readPressure());
        Serial.println(" Pa");

        Serial.print(F("Approx altitude = "));
        Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
        Serial.println(" m");

        Serial.println();
        delay(2000);

        display.clearDisplay();
        // display temperature
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Temperature: ");
        display.setTextSize(2);
        display.setCursor(0, 10);
        display.print(String(bmp.readTemperature()));
        display.print(" ");
        display.setTextSize(1);
        display.cp437(true);
        display.write(167);
        display.setTextSize(2);
        display.print("C");

        // display humidity
        display.setTextSize(1);
        display.setCursor(0, 35);
        display.print("Pressure: ");
        display.setTextSize(2);
        display.setCursor(0, 45);
        display.print(String(bmp.readPressure()));
        display.print(" %");

        display.display();
        delay(5000);

        display.clearDisplay();

        DateTime now = rtc.now();

        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Heure: ");
        display.setTextSize(2);
        display.setCursor(0, 10);
        display.print(now.hour(), DEC);
        display.print(':');
        display.print(now.minute(), DEC);
        display.print(':');
        display.print(now.second(), DEC);
        display.println();

        display.setTextSize(1);
        display.setCursor(0, 35);
        display.print("Date: ");
        display.setTextSize(2);
        display.setCursor(0, 45);
        display.print(now.year(), DEC);
        display.print('/');
        display.print(now.month(), DEC);
        display.print('/');
        display.print(now.day(), DEC);
        display.print(" (");
        display.print(daysOfTheWeek[now.dayOfTheWeek()]);
        display.print(") ");

        display.display();

        delay(5000);
    }
    else
    {
        Serial.println("Forced measurement failed!");
    }
}
