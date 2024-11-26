#include "pinDrivers.h"

HW_pin_S HW_pins[PIN_COUNT] = {
    
    [HW_PIN_GATE_DRIVER_LOW] = { //output
        .pinNum = 2,
        .isOutput = true,
        .activeLow = false,
        .physicalState = true,
    },

    [HW_PIN_GATE_DRIVER_HIGH] = { //output
        .pinNum = 3,
        .isOutput = true,
        .activeLow = false,
        .physicalState = false,
    },

    [HW_PIN_MOSI] = { //output
        .pinNum = 26,
        .isOutput = true,
        .activeLow = false,
        .physicalState = false,
    },

    [HW_PIN_SCK] = { //output
        .pinNum = 27,
        .isOutput = true,
        .activeLow = false,
        .physicalState = false,
    },

    [HW_PIN_CS] = { //output 
        .pinNum = 38,
        .isOutput = true,
        .activeLow = false,
        .physicalState = false,
    },

    [HW_PIN_MISO] = { //input 
        .pinNum = 39, 
        .isOutput = false,
        .activeLow = false,
        .physicalState = false,
    }
};

void HW_setupPins(void)
{
    for (int i = 0; i < PIN_COUNT; i++)
    {
        if (HW_pins[i].isOutput)
        {
            pinMode(HW_pins[i].pinNum, OUTPUT);
            digitalWrite(HW_pins[i].pinNum, HW_pins[i].physicalState);
        }
        else
        {
            pinMode(HW_pins[i].pinNum, INPUT);
        }
    }
}

bool HW_digitalRead(HW_pin_E i)
{
    bool state = false;
    if (i < PIN_COUNT)
    {
        state = digitalRead(HW_pins[i].pinNum);
        HW_pins[i].physicalState = state;
    }

    return state;
}

void HW_digitalWrite(HW_pin_E i, bool physState)
{
    if (i < PIN_COUNT)
    {
        digitalWrite(HW_pins[i].pinNum, physState);
        HW_pins[i].physicalState = physState;
    }
}

int HW_analogRead(HW_pin_E i)
{
    if (i < PIN_COUNT)
    {
        return analogRead(HW_pins[i].pinNum);
    }

    return 0;
}

void HW_analogWrite(HW_pin_E i, int value)
{
    if (i < PIN_COUNT)
    {
        analogWrite(HW_pins[i].pinNum, value);
    }
}
