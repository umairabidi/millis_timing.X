#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/proc/pic16f616.h"

#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config IOSCFS = 8MHZ    // Internal Oscillator Frequency Select bit (8 MHz)
#pragma config BOREN = OFF      // Brown-out Reset Selection bits (BOR Disabled)

#define _XTAL_FREQ 8000000
#define initialValue 21

volatile uint32_t millisCount = 0;

void millisInit() {
	OPTION_REGbits.T0CS = 0;    // Use internal clock (Fosc/4)
    OPTION_REGbits.PSA = 0;     // Assign prescaler to Timer0
    OPTION_REGbits.PS = 0b000;  // Prescaler 1:64
    TMR0 = initialValue;
    INTCONbits.T0IE = 1;        // Enable Timer0 interrupt
    INTCONbits.GIE = 1;         // Enable global interrupts
}

void __interrupt() isr() {
    if (INTCONbits.T0IF) {
        INTCONbits.T0IF = 0;
        millisCount++;
        TMR0 = initialValue;
    }
}

uint32_t millis() {
    INTCONbits.GIE = 0;
    uint32_t result = millisCount;    
    INTCONbits.GIE = 1;
    return result;
}

void delay_ms(uint32_t ms) {
    uint32_t startTime = millis();
    while ((millis() - startTime) < ms);
}

void main() {
    ANSEL = 0;
    TRISCbits.TRISC3 = 0;	// LED output
    millisInit();

    while (1) {
        PORTCbits.RC3 = 1;
        delay_ms(4);
        PORTCbits.RC3 = 0;
        delay_ms(4);
    }
}
