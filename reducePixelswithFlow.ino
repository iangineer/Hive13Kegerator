// Hive13 RFID Kegerator Project
// written by Ian Mathews
// This test is to count enough pulses from the flow meter to reach 1 pint (16oz), then close the solenoid.

#include <AFMotor.h>

const int ringPin = 6;

const int ringPixels = 16; // 16 pixels in the NeoPixel ring.

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(ringPixels, ringPin, NEO_GRB + NEO_KHZ800);

const int pixelBrightness = 64; // pick value from 0-255
const uint32_t red = (pixelBrightness, 0, 0);   // RGB values for red
const uint32_t green = (0, pixelBrightness, 0); // RGB values for green
const uint32_t blank = (0, 0, 0); // RGB values to turn off LED

AF_DCMotor motor(4);  //Solenoid connected to M4 port with red wire left and black wire right

// Variable to count pulses from flow meter.  volatile so that the Interrupt sSrvice Routine (ISR) and setup and loop functions can share data.
volatile unsigned int pulses = 0;      // gets reset to zero after every ounce in main loop
volatile unsigned int totalPulses = 0; // total pulses recorded to print to Serial 

// count the number of ounces (13 pulses = 1 ounces)
int ounces = 0;
int lastOunces = 0;

// count the number of pints (16 ounces = 1 pint)
int pints = 0;

boolean pouring = TRUE;

// ISR for flow meter #1
// Every time the flow meter signal changes from LOW to HIGH, incriment the pulse count by 1
void flowMeter1()
{
  pulses++;
  totalPulses++;
}

void setup() {
  
  // set up Serial library at 9600 bps
  Serial.begin(9600);
  Serial.println("Flow control test!");
  
  // attach interrupt handler to detect when Pin D2 (flow meter #1 signal) changes state from LOW to HIGH.
  attachInterrupt (0, flowMeter1, RISING);  
  
  colorWipe(strip.Color(red), 10); // Red
  delay (1000);
  colorWipe(strip.Color(green), 50); // Green
  
  // turn on solenoid control and open solenoid
  motor.setSpeed(255);
  motor.run(BACKWARD);  //Solenoid opens
  delay(50);            //Solenoid requires 50ms pulse
  motor.run(RELEASE);
}

void loop() {
  
  if (ounces > lastOunces) {
    strip.setPixelColor(strip.numPixels() - ounces, blank);
    strip.show();
    lastOunces = ounces;
  }
  
  //Every 13 pulses of the flow meter is an ounce
  if(pulses >= 13)
  {
    ounces++;  // increment ounce count by 1
    pulses = 0;  // reset pulse count to zero
  }
  
  // close solenoid when we've poured a pint (16 ounces)
  if(ounces >= 16)
  {
    pints++;     // incriment pint count by 1
    ounces = 0;  // reset ounce count to zero
    
    motor.run(FORWARD);   // solenoid closes
    delay(50);            // solenoid requires 50ms pulse
    motor.run(RELEASE);
    pouring = FALSE;
  }
  
  // print current pulse, ounce, and pint counts
  Serial.print("Pulses: "); Serial.println(totalPulses);
  //Serial.print("Ounces: "); Serial.println(ounces);
  Serial.print("Pints: "); Serial.println(pints);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
