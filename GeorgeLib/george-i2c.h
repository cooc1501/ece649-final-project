#ifndef __GEORGE_I2C__
#define __GEORGE_I2C__

#include <msp430fr6989.h>

/**
 * Initialize MSP430 eUSCI module B to I2C master mode at 131KHz
 *
 *
 */
void i2c_init(void) { 
    // Enter reset state before the configuration starts...
    UCB1CTLW0 |= UCSWRST;

    // Divert pins to I2C functionality
    P4SEL1 |= (BIT1|BIT0);
    P4SEL0 &= ~(BIT1|BIT0);

    // Keep all the default values except the fields below...
    // (UCMode 3:I2C) (Master Mode) (UCSSEL 1:ACLK, 2,3:SMCLK)
    UCB1CTLW0 |= UCMODE_3 | UCMST | UCSSEL_3;

    // Clock divider = 8 (SMCLK @ 1.048 MHz / 8 = 131 KHz)
    UCB1BRW = 8;

    // Exit the reset mode
    UCB1CTLW0 &= ~UCSWRST;
}

/**
 * Read 16-bit word from i2c_address[i2c_reg] into data
 *
 *
 */
int i2c_read_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int * data) {
    unsigned char byte1, byte2;
    // Initialize the bytes to make sure data is received every time
    byte1 = 111;
    byte2 = 111;
    //********** Write Frame #1 ***************************
    UCB1I2CSA = i2c_address; // Set I2C address
    UCB1IFG &= ~UCTXIFG0;
    UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
    UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal

    while ((UCB1IFG & UCTXIFG0) ==0) {}

    UCB1TXBUF = i2c_reg; // Byte = register address

    while((UCB1CTLW0 & UCTXSTT)!=0) {}

    if(( UCB1IFG & UCNACKIFG )!=0) return -1;

    UCB1CTLW0 &= ~UCTR; // Master reads (R/W bit = Read)
    UCB1CTLW0 |= UCTXSTT; // Initiate a repeated Start Signal
    //****************************************************

    //********** Read Frame #1 ***************************
    while ( (UCB1IFG & UCRXIFG0) == 0) {}
    byte1 = UCB1RXBUF;
    //****************************************************

    //********** Read Frame #2 ***************************
    while((UCB1CTLW0 & UCTXSTT)!=0) {}
    UCB1CTLW0 |= UCTXSTP; // Setup the Stop Signal
    while ( (UCB1IFG & UCRXIFG0) == 0) {}
    byte2 = UCB1RXBUF;
    while ( (UCB1CTLW0 & UCTXSTP) != 0) {}
    //****************************************************

    // Merge the two received bytes
    *data = ( (byte1 << 8) | (byte2 & 0xFF) );
    return 0;
}

/**
 * Write 16-bit word data to i2c_address[i2c_reg]
 *
 *
 */
int i2c_write_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int data) {
    unsigned char byte1, byte2;

    byte1 = (data >> 8) & 0xFF; // MSByte
    byte2 = data & 0xFF; // LSByte

    UCB1I2CSA = i2c_address; // Set I2C address

    UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
    UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal

    while ((UCB1IFG & UCTXIFG0) ==0) {}

    UCB1TXBUF = i2c_reg; // Byte = register address

    while((UCB1CTLW0 & UCTXSTT)!=0) {}

    //********** Write Byte #1 ***************************
    UCB1TXBUF = byte1;
    while ( (UCB1IFG & UCTXIFG0) == 0) {}
    //****************************************************

    //********** Write Byte #2 ***************************
    UCB1TXBUF = byte2;
    while ( (UCB1IFG & UCTXIFG0) == 0) {}
    UCB1CTLW0 |= UCTXSTP;
    while ( (UCB1CTLW0 & UCTXSTP) != 0) {}
    //****************************************************
    return 0;
}




#endif