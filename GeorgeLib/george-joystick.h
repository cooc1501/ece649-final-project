#ifndef __GEORGE_JOYSTICK__
#define __GEORGE_JOYSTICK__

#include <stdint.h>
#include <stdio.h>
#include <msp430fr6989.h>
#include "GeorgeLib/george-uart.h"

enum Joystick_Direction {
    NONE = 0, UP, DOWN, LEFT, RIGHT
};

/**
 * Initialize and Configure ADC12_B
 */
void init_adc() {
    // set P9.2 to mode 3 (ADC input A10, Joystick X)
    P9SEL0 |= BIT2; P9SEL1 |= BIT2;

    // set P8.7 to mode 2 (ADC input A4, Joystick Y)
    P9SEL0 &= ~BIT2; P9SEL1 |= BIT2;

    // I'm not quite sure what the proper sampling and hold time is, but for now
    ADC12CTL0 = ADC12SHT0_4 | ADC12SHT1_4 | ADC12ON;  // set sample and hold time, turn on ADC
    ADC12CTL1 |= ADC12SHP;  // use sample timer as source of sample signal rather than the sample-input signal (ADC12SC software signal)
    ADC12CTL2 |= ADC12RES_2;  // use 12-bit sampling (right aligned, so bits 15-12 are 0)


    // configure clock source and divider
    // default clock is MODOSC, default divider is 1. These are fine.
    
    // configure memory 0 and 1 for channels 4 and 10 respectively
    ADC12MCTL0 |= ADC12INCH_4;
    ADC12MCTL1 |= ADC12INCH_10;

    // enable sampling (this doesn't begin a sample, thats ADC12CTL0 |= ADC12SC)
    // ADC12CTL0 |= ADC12ENC;
}

void check_adc_joystick(enum Joystick_Direction *x){
    // trigger reading
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    
    // wait for results to get stored in memory
    while (~(ADC12IFGR0 & ADC12IFG0)) {}
    while (~(ADC12IFGR0 & ADC12IFG1)) {}

    // load positions to pointer args
    uint16_t pos_x = ADC12MEM0;
    uint16_t pos_y = ADC12MEM0;

    char buffer[100];
    sprintf(buffer, "X pos: %d\tY pos: %d", pos_x, pos_y);
    uart_write_ascii(buffer);
    return;
}



#endif
