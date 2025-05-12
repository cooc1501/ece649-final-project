#include <msp430fr6989.h>

#include <GrLib/grlib/grlib.h>          // Graphics library (grlib)
#include "LcdDriver/lcd_driver.h"       // LCD driver
#include <stdio.h>

#include "GeorgeLib/george-uart.h"
#include "GeorgeLib/george-joystick.h"
#include "GeorgeLib/george-photos.h"

#define redLED BIT0
#define greenLED BIT7
#define S1 BIT3  // port 3
#define S2 BIT2  // port 3

// Global Definitions
enum State {
    HOME_SCREEN, PIC1, PIC2, PIC3, PIC4, PIC5
};

enum Button_State {
    INACTIVE = 0, ACTIVE = 1
};

// Functions
enum Button_State checkButton(int button);
void displayHome(Graphics_Context *ctx);


/**
 * main.c
 */
int main(void)
{
    volatile unsigned int counter=0;
    char mystring[20];
    unsigned int n;
    unsigned int img_chng = 0;
    unsigned int home_chng = 1;
    
    // import bitmaps
    extern tImage moe;
    extern tImage clancy;
    extern tImage daisy;
    extern tImage norb;
    extern tImage vanish;
    tImage *active;

    enum State state = HOME_SCREEN;
    enum Button_State s_S1, s_S2;
    enum Joystick_Direction s_joystick;

    /* Initialize ADC12_B */

    WDTCTL = WDTPW | WDTHOLD;     // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;         // Disable GPIO power-on default high-impedance mode

    // configure LEDs
    P1DIR |= redLED;    P1OUT &= ~redLED;
    P9DIR |= greenLED;  P9OUT &= ~greenLED;
    
    // configure buttons as input
    P3DIR &= ~S1; P3REN |= S1; P3OUT |= S1;  
    P3DIR &= ~S2; P3REN |= S2; P3OUT |= S2;

    // Configure SMCLK to 8 MHz (used as SPI clock)
    CSCTL0 = CSKEY;                 // Unlock CS registers
    CSCTL3 &= ~(BIT4|BIT5|BIT6);    // DIVS=0
    CSCTL0_H = 0;                   // Relock the CS registers

    // Initialize UART for debug messages
    #ifdef DEBUG
    uart_init();
    #endif

    //////////////////////////////////////////////////////////////////////////////////////////
    Graphics_Context g_sContext;        // Declare a graphic library context
    Crystalfontz128x128_Init();         // Initialize the display

    // Set the screen orientation
    Crystalfontz128x128_SetOrientation(0);

    // Initialize the context
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);

    // Set background and foreground colors
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);

    // Set the default font for strings
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    // Clear the screen
    Graphics_clearDisplay(&g_sContext);
    ////////////////////////////////////////////////////////////////////////////////////////////


    

    // Graphics_drawStringCentered(&g_sContext, "Welcome to", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    // sprintf(mystring, "ECE 649!");
    // Graphics_drawStringCentered(&g_sContext, mystring, AUTO_STRING_LENGTH, 64, 55, OPAQUE_TEXT);

    // n = 1234;
    // sprintf(mystring, "%d", n);
    // Graphics_drawStringCentered(&g_sContext, mystring, AUTO_STRING_LENGTH, 64, 80, OPAQUE_TEXT);

    // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    // Graphics_Rectangle color1 = {20,100,128,103};
    // Graphics_drawRectangle(&g_sContext, &color1);
    // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    // Graphics_fillRectangle(&g_sContext, &color1);

    // Graphics_Rectangle color2 = {40,105,128,108};
    // Graphics_drawRectangle(&g_sContext, &color2);
    // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    // Graphics_fillRectangle(&g_sContext, &color2);

    // Graphics_Rectangle color3 = {60,110,128,113};
    // Graphics_drawRectangle(&g_sContext, &color3);
    // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    // Graphics_fillRectangle(&g_sContext, &color3);

    // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    // Graphics_drawCircle(&g_sContext, 5, 110, 5);
    // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    // Graphics_fillCircle (&g_sContext, 17, 110, 5);

    // Graphics_drawImage(&g_sContext, &unh_logo,0,0);

    // Program Main Loop
    adc_init();
    while(1){
        // Sample the current state of the joystick and pushbuttons
        // s_joystick = checkJoystick();
        check_adc_joystick(&s_joystick);
        s_S1 = checkButton(1);
        s_S2 = checkButton(2);

        // write current state over uart
        #ifdef DEBUG
        uart_write_uint16(state);
        #endif

        // Check state transition conditions, and perform transition actions (managing LCD transitions)
        switch (state) {
            case HOME_SCREEN:
                if (s_S1) {
                    img_chng = 1;
                    state = PIC1;
                }
                break;
            
            case PIC1:
                if (s_S2) {
                    home_chng = 1;
                    state = HOME_SCREEN;
                    break;
                }
                switch (s_joystick) {
                    case RIGHT:
                        img_chng = 1;
                        state++;
                        break;
                    case LEFT:
                        img_chng = 1;
                        state = PIC5;
                        break;
                    default:
                        break;
                }
                break;

            case PIC2:
                if (s_S2) {
                    home_chng = 1;
                    state = HOME_SCREEN;
                    break;
                }
                switch (s_joystick) {
                    case RIGHT:
                        img_chng = 1;
                        state++;
                        break;
                    case LEFT:
                        img_chng = 1;
                        state--;
                        break;
                    default:
                        break;
                }
                break;

            case PIC3:
                if (s_S2) {
                    home_chng = 1;
                    state = HOME_SCREEN;
                    break;
                }
                switch (s_joystick) {
                    case RIGHT:
                        img_chng = 1;
                        state++;
                        break;
                    case LEFT:
                        img_chng = 1;
                        state--;
                        break;
                    default:
                        break;
                }
                break;

            case PIC4:
                if (s_S2) {
                    home_chng = 1;
                    state = HOME_SCREEN;
                    break;
                }
                switch (s_joystick) {
                    case RIGHT:
                        img_chng = 1;
                        state++;
                        break;
                    case LEFT:
                        img_chng = 1;
                        state--;
                        break;
                    default:
                        break;
                }
                break;

            case PIC5:
                if (s_S2) {
                    home_chng = 1;
                    state = HOME_SCREEN;
                    break;
                }
                switch (s_joystick) {
                    case RIGHT:
                        img_chng = 1;
                        state = PIC1;
                        break;
                    case LEFT:
                        img_chng = 1;
                        state--;
                        break;
                    default:
                        break;
                }
                break;

            default:
                home_chng = 1;
                state = HOME_SCREEN;
                break;
        }

        // Perform current state actions
        switch (state) {
            case HOME_SCREEN:
                // state behavior
                break;
            
            case PIC1:
                // state behavior
                active = &moe;
                break;

            case PIC2:
                // state behavior
                active = &clancy;
                break;

            case PIC3:
                // state behavior
                active = &daisy;
                break;

            case PIC4:
                // state behavior
                active = &norb;
                break;

            case PIC5:
                // state behavior
                active = &vanish;
                break;

            default:
                // state behavior
                break;
        }
    }

    if (img_chng) {
        Graphics_drawImage(&g_sContext, active, 0, 0);
        img_chng = 0;
    }

    if (home_chng) {
        Graphics_drawStringCentered(&g_sContext, "ECE649 Photo Frame", AUTO_STRING_LENGTH, 15, 15, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "Developed By:", AUTO_STRING_LENGTH, 15, 40, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "George Crane", AUTO_STRING_LENGTH, 15, 65, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, ">>Press S1 to start", AUTO_STRING_LENGTH, 15, 90, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, ">>Press S2 to reset", AUTO_STRING_LENGTH, 15, 115, OPAQUE_TEXT);
        home_chng = 0;
    }
    
    return 0;
}

enum Button_State checkButton(int button){
    switch (button) {
        case 1:
            if (P3IN & S1){
                return ACTIVE;
            }
            return INACTIVE;
            break;
        case 2:
            if (P3IN & S2){
                return ACTIVE;
            }
            return INACTIVE;
            break;
        default:
            return INACTIVE;
    }
}


void displayHome(Graphics_Context *ctx) {
    Graphics_drawStringCentered(&ctx, "ECE649 Photo Frame", AUTO_STRING_LENGTH, 15, 15, OPAQUE_TEXT);
    Graphics_drawStringCentered(&ctx, "Developed By:", AUTO_STRING_LENGTH, 15, 40, OPAQUE_TEXT);
    Graphics_drawStringCentered(&ctx, "George Crane", AUTO_STRING_LENGTH, 15, 65, OPAQUE_TEXT);
    Graphics_drawStringCentered(&ctx, ">>Press S1 to start", AUTO_STRING_LENGTH, 15, 90, OPAQUE_TEXT);
    Graphics_drawStringCentered(&ctx, ">>Press S2 to reset", AUTO_STRING_LENGTH, 15, 115, OPAQUE_TEXT);
    return;
}
