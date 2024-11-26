
#include <eFlexPwm.h>
#include <Arduino.h>
#include <SPI.h>
#include <MCP320x.h>
#include <pinDrivers.h>

#include <arm_math.h>
#include <math.h>
#include <util/atomic.h>

using namespace eFlex;

// My eFlexPWM submodules (Hardware > PWM2: SM[0], SM[2], SM[3])
SubModule Sm20 (4, 33);
SubModule Sm22 (6, 9);
SubModule Sm23 (36, 37);
//  Tm2 simplifies access to the functions that concern all the sub-modules
Timer &Tm2 = Sm20.timer();

uint8_t dutyCyclePercent = 0; // the duty cycle in %
const uint32_t PwmFreq = 18000; // FPwm 18kHz


// ADC object
MCP320x adc(HW_pins[HW_PIN_CS].pinNum, HW_pins[HW_PIN_MOSI].pinNum, HW_pins[HW_PIN_MISO].pinNum, HW_pins[HW_PIN_SCK].pinNum);


const float32_t MidDutyCycle = 32768; // middle duty cycle value
const float32_t SineFreqMin = 5.0;    // min sine frequency
const float32_t SineFreqMax = 50.0;   // max sine frequency
const float32_t SineFreqStep = 5.0;   // sine frequency step 
const float32_t DeadTimeNs = 50.0;    // deadtime in nanoseconds
const uint32_t StepDelay = 5000;      // Increments sineFreq by SineFreqStep every StepDelay ms.

float32_t pwmFreq = 10000.0; // PWM frequency in hz
float32_t sineFreq = SineFreqMax / 2; // Sine Frequency in Hz

// Interrupt Input Variables
volatile uint32_t vSample;
volatile float32_t vSpeed;

// My eFlexPWM submodules (Hardware > PWM2: SM[0], SM[2])
SubModule Sm42 (2, 3);

Timer &Tm2 = Sm42.timer();


void IsrOverflowSm20() {
  float32_t s;

  // The Teensy's LED is lit during the interrupt routine in order to be able to measure its execution time.
  digitalWriteFast (LED_BUILTIN, HIGH);
  
  s = roundf ( (MidDutyCycle - 1) * arm_sin_f32 (vSpeed * ++vSample));

  Sm20.updateDutyCycle (static_cast<uint16_t> (MidDutyCycle + s));
  Sm22.updateDutyCycle (static_cast<uint16_t> (MidDutyCycle - s));

  Sm20.clearStatusFlags (kPWM_CompareVal1Flag);
  Tm2.setPwmLdok();

  digitalWriteFast (LED_BUILTIN, LOW);
}

void updateSpeed() {

  float32_t s = 2.0 * PI * (sineFreq / pwmFreq);
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    vSpeed = s;
  }
}


void setup() {

  pinMode (LED_BUILTIN, OUTPUT);

  digitalWrite (LED_BUILTIN, HIGH);
  while (!Serial)
    ; // wait for serial port to connect.
  Serial.println ("eFlexPwm Single Phase Inverter Example");    

  Config myConfig;

  myConfig.setReloadLogic (kPWM_ReloadPwmFullCycle);
  /* PWM A & PWM B form a complementary PWM pair */
  myConfig.setPairOperation (kPWM_ComplementaryPwmA);
  myConfig.setPwmFreqHz (pwmFreq);

  /* Initialize submodule 0 */
  if (Sm42.configure (myConfig) != true) {
    Serial.println ("Submodule 0 initialization failed");
    exit (EXIT_FAILURE);
  }

  uint16_t deadTimeVal = ( (uint64_t) Tm2.srcClockHz() * DeadTimeNs) / 1000000000;
  Tm2.setupDeadtime (deadTimeVal);

  // synchronize registers and start all submodules
  if (Tm2.begin() != true) {
    Serial.println ("Failed to start submodules");
    exit (EXIT_FAILURE);
  }
  else {

    Serial.println ("Submodules successfuly started");
    // Sm20.printRegs(); // to see the values of the VALx registers
  }

  updateSpeed();

  // Enable counter overflow interrupt (VAL1)
  Sm42.enableInterrupts (kPWM_CompareVal1InterruptEnable);
  attachInterruptVector (IRQ_FLEXPWM2_0, &IsrOverflowSm20);
  NVIC_ENABLE_IRQ (IRQ_FLEXPWM2_0);

  // end of PWM setup
  digitalWrite (LED_BUILTIN, LOW);

  HW_setupPins();
}

void loop() {



//Measure current and voltage

    uint16_t raw_current = adc.readChannel(0);
    uint16_t raw_voltage = adc.readChannel(1);

    float current = (raw_current * 3.3 / 4095) * 5; // 5A/V
    float voltage = raw_voltage * 3.3 / 4095;


    Serial.printf ("Fs=%.1f\n", sineFreq);
    delay (StepDelay);
    sineFreq += SineFreqStep;

    if (sineFreq > SineFreqMax) {

      sineFreq = SineFreqMin;
    }
    updateSpeed();
}
