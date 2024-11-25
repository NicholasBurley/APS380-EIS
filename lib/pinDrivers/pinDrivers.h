#include "Arduino.h"

#ifndef UTFR_PIN_DRIVER_ACM
#define UTFR_PIN_DRIVER_ACM

enum HW_pin_E
{              
    HW_PIN_GATE_DRIVER_LOW,                 
    HW_PIN_GATE_DRIVER_HIGH,
    HW_PIN_MOSI,
    HW_PIN_SCK,
    HW_PIN_CS,
    HW_PIN_MISO,
    PIN_COUNT
};

struct HW_pin_S{
    int pinNum;
    bool isOutput;
    bool activeLow;
    bool physicalState;
};

extern HW_pin_S HW_pins[PIN_COUNT];

void HW_setupPins(void);
bool HW_digitalRead(HW_pin_E x);
void HW_digitalWrite(HW_pin_E x, bool physState);
int HW_analogRead(HW_pin_E x);
void HW_analogWrite(HW_pin_E x, int value);

#endif
