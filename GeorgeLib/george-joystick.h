#ifndef __GEORGE_JOYSTICK__
#define __GEORGE_JOYSTICK__

#include <stdint.h>
#include <stdio.h>
#include <msp430fr6989.h>

enum Joystick_Direction {
    NONE = 0, UP, DOWN, LEFT, RIGHT
};

/**
 * Initialize and Configure ADC12_B
 */
void adc_init() {
    // set P9.2 to mode 3 (ADC input A10, Joystick X)
    P9SEL0 |= BIT2; P9SEL1 |= BIT2;

    // set P8.7 to mode 2 (ADC input A4, Joystick Y)
    P9SEL0 &= ~BIT2; P9SEL1 |= BIT2;

    ADC12CTL0 = ADC12SHT0_4 | ADC12SHT1_4 | ADC12ON;  // set sample and hold time, turn on ADC
    ADC12CTL1 |= ADC12SHP | ADC12CONSEQ0;  // use sample timer as source of sample signal rather than the 
    // sample-input signal (ADC12SC software signal), and change conversion mode to sequence of channels mode
    ADC12CTL2 |= ADC12RES_2;  // use 12-bit sampling (right aligned, so bits 15-12 are 0)
    ADC12CTL3 &= ~(ADC12CSTARTADD0 | ADC12CSTARTADD1 | ADC12CSTARTADD2 | ADC12CSTARTADD3 | ADC12CSTARTADD4);
    // ^ set conversion start address to MEM0
    
    // configure memory 0 and 1 for channels 4 and 10 respectively
    ADC12MCTL0 |= ADC12INCH_4;
    ADC12MCTL1 |= ADC12INCH_10 | ADC12EOS;  // end of sequence bit resets sequence position
}

void check_adc_joystick(enum Joystick_Direction *direction){
    // trigger reading
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    
    // wait for result to get stored in mem0
    while (~ADC12IFGR0 & ADC12IFG0) {}
    ADC12CTL0 |= ADC12SC;  // trigger next reading into mem1
    while (~ADC12IFGR0 & ADC12IFG1) {}

    // read position samples
    uint16_t pos_x = ADC12MEM1;  // [left] 0 --- 2048+-200 --- 4095 [right]
    uint16_t pos_y = ADC12MEM0;  // [down] 0 --- 2048+-200 --- 4095 [up]

    if (pos_x >= 1848 && pos_x <= 2248) {
        *direction = NONE;   
    }
    else if (pos_x <= 1848) {
        *direction = LEFT;
    }
    else {
        *direction = RIGHT;
    }

    ADC12CTL0 &= ~ADC12ENC;
    return;
}



#endif
