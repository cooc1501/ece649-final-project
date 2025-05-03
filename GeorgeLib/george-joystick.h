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
void adc_init() {
    // set P9.2 to mode 3 (ADC input A10, Joystick X)
    P9SEL0 |= BIT2; P9SEL1 |= BIT2;

    // set P8.7 to mode 2 (ADC input A4, Joystick Y)
    P9SEL0 &= ~BIT2; P9SEL1 |= BIT2;

    // I'm not quite sure what the proper sampling and hold time is, but for now
    ADC12CTL0 = ADC12SHT0_4 | ADC12SHT1_4 | ADC12ON;  // set sample and hold time, turn on ADC
    ADC12CTL1 |= ADC12SHP | ADC12CONSEQ0;  // use sample timer as source of sample signal rather than the 
    // sample-input signal (ADC12SC software signal), and change conversion mode to sequence of channels mode
    ADC12CTL2 |= ADC12RES_2;  // use 12-bit sampling (right aligned, so bits 15-12 are 0)
    ADC12CTL3 &= ~(ADC12CSTARTADD0 | ADC12CSTARTADD1 | ADC12CSTARTADD2 | ADC12CSTARTADD3 | ADC12CSTARTADD4);


    // default clock is MODOSC, default divider is 1. These are fine.
    
    // configure memory 0 and 1 for channels 4 and 10 respectively
    ADC12MCTL0 |= ADC12INCH_4;
    ADC12MCTL1 |= ADC12INCH_10 | ADC12EOS;

    // enable sampling (this doesn't begin a sample, thats ADC12CTL0 |= ADC12SC)
    // ADC12CTL0 |= ADC12ENC;
}

void check_adc_joystick(enum Joystick_Direction *x){
    // trigger reading
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    
    // wait for result to get stored in mem0
    while (~ADC12IFGR0 & ADC12IFG0) {}
    ADC12CTL0 |= ADC12SC;  // trigger next reading into mem1
    while (~ADC12IFGR0 & ADC12IFG1) {}

    // load positions to pointer args
    uint16_t pos_x = ADC12MEM1;  // [left] 0 --- 2000+-200 --- 4095 [right]
    uint16_t pos_y = ADC12MEM0;  // [down] 0 --- 2000+-200 --- 4095 [up]

    // char buffer[100];
    // sprintf(buffer, "X pos: %d\nY pos: %d\0", pos_x, pos_y);
    // sprintf(buffer, "X pos: \nY pos: \0");
    // uart_write_ascii(buffer);
    uart_write_uint16(pos_x);
    uart_write_char('\t');
    uart_write_uint16(pos_y);
    uart_write_char('\n');

    ADC12CTL0 &= ~ADC12ENC;
    return;
}



#endif
