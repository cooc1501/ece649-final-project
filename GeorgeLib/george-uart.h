#ifndef __GEORGE_UART__
#define __GEORGE_UART__

#include <msp430fr6989.h>

#define FLAGS UCA1IFG
#define RXFG UCRXIFG
#define TXFG UCTXIFG
#define TXBUF UCA1TXBUF
#define RXBUF UCA1RXBUF

void init_UART(void){
    // Divert pins to UART functionality
    P3SEL1 &= ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);

    UCA1CTLW0 = UCSWRST;
    UCA1CTL1 |= UCSSEL__SMCLK;
    UCA1BR0 = 8;
    UCA1MCTLW = 0xD600;
    UCA1BR1 = 0;
    UCA1CTL1 &= ~UCSWRST;

    // 9600 baud rate doesn't work for some reason 
}

void uart_write_char(unsigned char ch){
    // Wait for any ongoing transmission to complete
    while ( (FLAGS & TXFG)==0 ) {}

    // Write the byte to the transmit buffer
    TXBUF = ch;
    return;
}

unsigned char uart_read_char(void){
    unsigned char temp;
    
    // Return NULL if no byte received
    if( (FLAGS & RXFG) == 0)
    return NULL;
    
    // Otherwise, copy the received byte (clears the flag) and return it
    temp = RXBUF;
    return temp;
}

void uart_write_uint16(uint16_t n) {
    int pos = 0;
    uint16_t tmp = n;
    int8_t digits[6] = { 0 };

    for (pos=0; pos<6; pos++){
        if (tmp) {
            digits[pos] = tmp%10;
            tmp /= 10;

        }
        else {
            digits[pos] = -1;
        }
    }

    for (pos=5; pos>=0; pos--){
        if (digits[pos] != -1){
            uart_write_char(digits[pos] + '0');
        }
    }

    return;
}

void uart_write_ascii(char *string) {
    while (*string != NULL){
        uart_write_char(*string++);
    }
    uart_write_char('\n');
    uart_write_char('\r');
}



#endif