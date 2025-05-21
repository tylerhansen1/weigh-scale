/* 
 * File:   main.c
 * Author: alekshurson
 *
 * Created on March 19, 2025, 3:51 PM
 */

#include "xc.h"

#include "lcdLib.h"
#include "adc_lib.h"
//#include "bufferLib.h"
#include <stdint.h>
#include <stdio.h>    // for sprintf
// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)



// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI   // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

#define DRDY_TRIS       TRISBbits.TRISB14



void setup() {
    _RCDIV = 0;             //sets frequency to 16MHz
    AD1PCFG = 0xffff;       //sets all pins digital
}
    
void delay(unsigned int ms) {
    while (ms-- > 0) {
        asm("repeat #15998");
        asm("nop");
    }
}

volatile char adStr[16];

int main(int argc, char** argv) {
    setup();
    lcd_init();
//    initBuffer();
    DRDY_TRIS = 1;
    I2C1_Init();
    ADC_Init();
    while (1) {
        // Read the ADC value (updated in interrupt)
        ADC_WaitForDataReady();
//        putVal(ADC_GetConversion());
//        int32_t currentAdcValue = getAvg();
        int32_t currentAdcValue = ADC_GetConversion();
        float max = 16777216;
        float voltage = (1/max)*currentAdcValue *3.295;
        float mass = (voltage - 0.56)*1000 + 57; // inst amp output range roughly 0.6-1.6 V. Converts to mass in grams
        

        sprintf(adStr, "%0.2f g", mass);  // ?x.xxxx V?
                       // 6.4 in the format string ?%6.4f? means 6 placeholders for the whole
                       // floating-point number, 4 of which are for the fractional part.
        

        // Send the formatted string to your display
       lcd_printStr(adStr);     // Print the ADC value to the display

    }
    return 0;
}

