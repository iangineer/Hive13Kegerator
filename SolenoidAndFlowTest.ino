// Hive13 RFID Kegerator Project
// written by Ian Mathews
// This test is to count enough pulses from the flow meter to reach 1 pint (16oz), then close the solenoid.

#include <AFMotor.h>

AF_DCMotor motor(4);  //Solenoid connected to M4 port with red wire left and black wire right

// Variable to count pulses from flow meter.  volatile so that the Interrupt sSrvice Routine (ISR) and setup and loop functions can share data.
volatile unsigned int pulses = 0;      // gets reset to zero after every ounce in main loop
volatile unsigned int totalPulses = 0; // total pulses recorded to print to Serial 

// count the number of ounces (13 pulses = 1 ounces)
int ounces = 0;

// count the number of pints (16 ounces = 1 pint)
int pints = 0;

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
  
  
  // turn on solenoid control and open solenoid
  motor.setSpeed(255);
  motor.run(BACKWARD);  //Solenoid opens
  delay(50);            //Solenoid requires 50ms pulse
  motor.run(RELEASE);
}

void loop() {
  
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
  }
  
  // print current pulse, ounce, and pint counts
  Serial.print("Pulses: "); Serial.println(totalPulses);
  //Serial.print("Ounces: "); Serial.println(ounces);
  Serial.print("Pints: "); Serial.println(pints);
}
