
#include <eFlexPwm.h>
#include <Arduino.h>
#include <SPI.h>

using namespace eFlex;

// My eFlexPWM submodules (Hardware > PWM2: SM[0], SM[2], SM[3])
SubModule Sm20 (4, 33);
SubModule Sm22 (6, 9);
SubModule Sm23 (36, 37);
//  Tm2 simplifies access to the functions that concern all the sub-modules
Timer &Tm2 = Sm20.timer();

uint8_t dutyCyclePercent = 0; // the duty cycle in %
const uint32_t PwmFreq = 18000; // FPwm 18kHz

void setup() {

  Config myConfig;
  myConfig.setReloadLogic (kPWM_ReloadPwmFullCycle);
  myConfig.setPairOperation (kPWM_ComplementaryPwmA);
  myConfig.setPwmFreqHz (PwmFreq); 

  // Initialize submodule
  Sm20.configure (myConfig);

  // Initialize submodule 2, make it use same counter clock as submodule 0. 
  myConfig.setClockSource (kPWM_Submodule0Clock);
  myConfig.setPrescale (kPWM_Prescale_Divide_1);
  myConfig.setInitializationControl (kPWM_Initialize_MasterSync);

  Sm22.configure (myConfig);
  Sm23.configure (myConfig);
  Tm2.setupDeadtime (500); // deatime 500ns
  // synchronize registers and start all submodules
  Tm2.begin();
}

void loop() {
  dutyCyclePercent += 5;

  // Update duty cycles for all 3 PWM signals
  Sm20.updateDutyCyclePercent (dutyCyclePercent, ChanA);
  Sm22.updateDutyCyclePercent (dutyCyclePercent >> 1, ChanA);
  Sm23.updateDutyCyclePercent (dutyCyclePercent >> 2, ChanA);

  // Set the load okay bit for all submodules to load registers from their buffer
  Tm2.setPwmLdok();

  if (dutyCyclePercent >= 100) {
    
    dutyCyclePercent = 5;
  }

  // Delay at least 100 PWM periods
  delayMicroseconds ( (1000000U / PwmFreq) * 100);
}
