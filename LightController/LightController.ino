/*
 *  RemoteControlBox.ino
 *
 * Based on: SimpleReceiver.cpp
 *
 *  SimpleReceiver.cpp is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2023 Armin Joachimsmeyer and Java Jokers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */

/*
 * Specify which protocol(s) should be used for decoding.
 * If no protocol is defined, all#define DECODE_DENON        // Includes Sharp
 */

#define DECODE_SAMSUNG

// #define DECODE_WHYNTER protocols (except Bang&Olufsen) are active.
//  * This must be done before the #include <IRremote.hpp>

#include <Arduino.h>

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.
#include <IRremote.hpp>

#include <Adafruit_NeoPixel.h> // Add Neopixel Library

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LF_LED_PIN 6
#define RF_LED_PIN 5
#define LB_LED_PIN 7
#define RB_LED_PIN 8

String r;
String g;
String b;

int colour_offset = 0;
int colour_increment = 5000;

int effect = 1;
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 3

Adafruit_NeoPixel stripLF(LED_COUNT, LF_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripRF(LED_COUNT, RF_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripLB(LED_COUNT, LB_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripRB(LED_COUNT, RB_LED_PIN, NEO_GRB + NEO_KHZ800);

// Global Vars
String command;

void setup()
{
  Serial.begin(115200); // Begin debugging Serial port
  Serial.println("Serial enabled");
  pinMode(13, OUTPUT);

// These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
// Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  stripLF.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  stripLF.show();            // Turn OFF all pixels ASAP
  stripLF.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  stripRF.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  stripRF.show();            // Turn OFF all pixels ASAP
  stripRF.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  stripLB.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  stripLB.show();            // Turn OFF all pixels ASAP
  stripLB.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  stripRB.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  stripRB.show();            // Turn OFF all pixels ASAP
  stripRB.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
}

void loop()
{
  if (effect == 1)
  {
    rainbow(25);
  }
}

void colorWipe(int r, int g, int b, int wait, int endWait)
{
  for (int i = 0; i < stripLF.numPixels(); i++)
  {                                    // For each pixel in strip...
    stripRF.setPixelColor(i, r, g, b); //  Set pixel's color (in RAM)
    stripLF.setPixelColor(i, r, g, b); //  Set pixel's color (in RAM)
    stripRB.setPixelColor(i, r, g, b); //  Set pixel's color (in RAM)
    stripLB.setPixelColor(i, r, g, b); //  Set pixel's color (in RAM)
    stripRF.show();                    //  Update strip to match
    stripLF.show();                    //  Update strip to match
    stripRB.show();                    //  Update strip to match
    stripLB.show();                    //  Update strip to match
    delay(wait);                       //  Pause for a moment
  }
  delay(endWait);
}

void checkForIR()
{

  if (IrReceiver.decode())
  {
    IrReceiver.resume(); // Enable receiving of the next value
    /*
     * Print a short summary of received data
     */
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRSendUsage(&Serial);
    //        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
    //            Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
    //            // We have an unknown protocol here, print more info
    //            IrReceiver.printIRResultRawFormatted(&Serial, true);
    //        }
    //        Serial.println();

    /*
     * !!!Important!!! Enable receiving of the next value,
     * since receiving has stopped after the end of the current received data packet.
     */
    /*
     * Finally, check the received data and perform actions according to the received command
     */

    Serial.println("----------------------------------------");
    Serial.println(String(IrReceiver.decodedIRData.command));

    if (IrReceiver.decodedIRData.command == 0x60)
    {
      Serial.println("Up");
    }
    else if (IrReceiver.decodedIRData.command == 0x61)
    {
      Serial.println("Down");
    }
    else if (IrReceiver.decodedIRData.command == 0x04)
    {
      Serial.println("1");
      colorWipe(255, 255, 255, 50, 2000);
    }
    else if (IrReceiver.decodedIRData.command == 0x05)
    {
      Serial.println("2");
      colorWipe(255, 0, 255, 50, 2000);
    }
    else if (IrReceiver.decodedIRData.command == 0x06)
    {
      Serial.println("3");
      colorWipe(0, 255, 255, 50, 2000);
    }
    else if (IrReceiver.decodedIRData.command == 0x08)
    {
      Serial.println("4");
      colorWipe(255, 255, 0, 50, 2000);
    }
    else if (IrReceiver.decodedIRData.command == 0x09)
    {
      Serial.println("5");
      meteorRain(0xff,0xff,0xff,1, 64, true, 50);
    }
    else if (IrReceiver.decodedIRData.command == 0x0A)
    {
      Serial.println("6");
      colorWipe(170, 54, 254, 50, 2000);
    }
    else if (IrReceiver.decodedIRData.command == 0x0C)
    {
      Serial.println("7");
      colorWipe(53, 257, 2, 50, 2000);
    }
    else if (IrReceiver.decodedIRData.command == 0x0D)
    {
      Serial.println("8");
    }
    else if (IrReceiver.decodedIRData.command == 0x0E)
    {
      Serial.println("9");
    }
    else if (IrReceiver.decodedIRData.command == 0x11)
    {
      Serial.println("0");
    }
  }
}

void stopAll()
{
  for (int i = 0; i < stripLF.numPixels(); i++)
  {                                    // For each pixel in strip...
    stripRF.setPixelColor(i, 0, 0, 0); //  Set pixel's color (in RAM)
    stripLF.setPixelColor(i, 0, 0, 0); //  Set pixel's color (in RAM)
    stripRB.setPixelColor(i, 0, 0, 0); //  Set pixel's color (in RAM)
    stripLB.setPixelColor(i, 0, 0, 0); //  Set pixel's color (in RAM)
    stripRF.show();                    //  Update strip to match
    stripLF.show();                    //  Update strip to match
    stripRB.show();                    //  Update strip to match
    stripLB.show();                    //  Update strip to match
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
{
  // Set all pixels on all strips to 0, 0, 0
  stopAll();
  for(int i = 0; i < LED_COUNT+LED_COUNT; i++) {
    // fade brightness all LEDs one step
    for (int j = 0; j < LED_COUNT; j++)
    {
      if ((!meteorRandomDecay) || (random(10) > 5))
      {
        fadeToBlack(j, meteorTrailDecay);
      }
    }
    // draw meteor
    for (int j = 0; j < meteorSize; j++)
    {
      if ((i - j < LED_COUNT) && (i - j >= 0))
      {
        setPixel(i - j, red, green, blue);
      }
    }
    showStrip();
    delay(SpeedDelay);
  }
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[LED_COUNT];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < LED_COUNT; i++) {
    cooldown = random(0, ((Cooling * 10) / LED_COUNT) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= LED_COUNT - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < LED_COUNT; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void setPixel(int num, byte red, byte green, byte blue)
{
  stripRF.setPixelColor(num, red, green, blue); //  Set pixel's color (in RAM)
  stripLF.setPixelColor(num, red, green, blue); //  Set pixel's color (in RAM)
  stripRB.setPixelColor(num, red, green, blue); //  Set pixel's color (in RAM)
  stripLB.setPixelColor(num, red, green, blue); //  Set pixel's color (in RAM)
}

void showStrip() {
  stripRF.show(); //  Update strip to match
  stripLF.show(); //  Update strip to match
  stripRB.show(); //  Update strip to match
  stripLB.show(); //  Update strip to match
}

void fadeToBlack(int ledNo, byte fadeValue)
{
  // Fade all strips
  for (int i = 0; i < stripLF.numPixels(); i++)
  { // For each pixel in strip...
    // Get current color values
    uint32_t oldColor = stripLF.getPixelColor(i);
    // Break down color values
    uint8_t r = (oldColor >> 16) & 0xFF;
    uint8_t g = (oldColor >> 8) & 0xFF;
    uint8_t b = oldColor & 0xFF;
    // Fade color values
    r = (r <= 10) ? 0 : (r - fadeValue);
    g = (g <= 10) ? 0 : (g - fadeValue);
    b = (b <= 10) ? 0 : (b - fadeValue);
    // Set pixel color
    stripLF.setPixelColor(i, r, g, b);
    stripRF.setPixelColor(i, r, g, b);
    stripRB.setPixelColor(i, r, g, b);
    stripLB.setPixelColor(i, r, g, b);
  }
  stripLF.show();
  stripRF.show();
  stripRB.show();
  stripLB.show();
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait)
{
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
  {
    for (int i = 0; i < stripLF.numPixels(); i++)
    { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / stripLF.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      stripLF.setPixelColor(i, stripLF.gamma32(stripLF.ColorHSV(pixelHue)));
      stripRF.setPixelColor(i, stripRF.gamma32(stripRF.ColorHSV(pixelHue)));
      stripRB.setPixelColor(i, stripRB.gamma32(stripRB.ColorHSV(pixelHue)));
      stripLB.setPixelColor(i, stripLB.gamma32(stripLB.ColorHSV(pixelHue)));
    }
    stripLF.show();
    stripRF.show();
    stripRB.show();
    stripLB.show();          // Update strip with new contents
    delayWithChecking(wait); // Pause for a moment
  }
}

void delayWithChecking(unsigned int delayAmount)
{ // Non-blocking version of delay() so that we can use it to wait during light functions without blocking the event loop of checking for message availability

  unsigned long millisToStop = millis() + delayAmount;

  while (millisToStop >= millis())
  {
    checkForIR();
  }
}
