#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/stepper.h"

#define STEPS 200

void setup(void) {
    // Set the system clock to 50 MHz (Tiva C LaunchPad)
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable the ADC0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Configure pins for stepper motor control
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinConfigure(GPIO_PE5_PHA1);
    GPIOPinConfigure(GPIO_PE4_PHB1);
    GPIOPinTypeStepper(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Configure the stepper motor driver
    StepperConfigure(stepper, STEPPER_CFG1, STEPPER_MODE_FULL_STEP, 200);

    // Set the speed of the motor to approximately 5 RPM
    StepperSpeedSet(stepper, SysCtlClockGet() / 200); // Adjust the division factor for desired speed

    // Initialize ADC
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
}

int main(void) {
    uint32_t val;
    int32_t step;
    int32_t previous = 0;

    setup();

    while (1) {
        // Trigger ADC conversion
        ADCIntClear(ADC0_BASE, 3);
        ADCProcessorTrigger(ADC0_BASE, 3);
        while (!ADCIntStatus(ADC0_BASE, 3, false)) {
        }
        ADCSequenceDataGet(ADC0_BASE, 3, &val);

        // Map ADC value to steps
        step = (int32_t)map(val, 0, 4095, 0, STEPS);

        // Move the stepper motor
        StepperStep(stepper, step - previous);

        // Remember the previous ADC value
        previous = step;
    }
}

print("Hello World")
    
