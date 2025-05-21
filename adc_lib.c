/* * Description: PIC24FJ64GA002 interfacing with NAU7802 ADC using I2C.
 *              Timer2 interrupt triggers an ADC read at 50 SPS.
 *
 * Hardware assumptions:
 * - PIC24 Fcy = 16 MHz.
 * - ADC I2C slave address: 0x2A.
 * - ADC conversion registers start at 0x12.
 * - ADC power-up is done via register 0x00.
 * - PGA gain is set on register 0x01 bits [2:0]. For gain=1, write 0x00.
 * - I2C1 pins are used: SDA1 on pin 18 and SCL1 on pin 17.
 * - DRDY is connected to PIC24 pin 16.
 */

#include <xc.h>
#include <stdint.h>

// PIC24 configuration bits (example settings ? adjust as needed)
//#pragma config FOSC = PRI     // Primary oscillator mode


// ADC I2C address and register addresses
#define ADC_I2C_ADDR_write    0b01010100      // ADC slave address plus write bit (0)
#define ADC_I2C_ADDR_read    0b01010101      // ADC slave address plus read bit (1))
#define ADC_CONV_REG_1    0x12      // Most sig. register for 24-bit conversion result
#define ADC_CONV_REG_2    0x13      //  register for 24-bit conversion result
#define ADC_CONV_REG_3    0x14      // least sig. register for 24-bit conversion result
#define ADC_PU_CTRL_REG 0x00      // Power-Up Control register
#define ADC_CTRL1_REG   0x01      // Control register 1: PGA gain is set here (bits [2:0])
#define ADC_CTRL2_REG   0x02

// DRDY pin definitions (using PIC24 pin 24)
#define DRDY_TRIS       TRISBbits.TRISB13  
#define DRDY_PORT       PORTBbits.RB13   
#define offset1 0b01111111
#define offset2 0b11111111
#define offset3 0b11111111
//#define offset1 0b01010100
//#define offset2 0b00111100
//#define offset3 0b10100000

// Global variable to hold the latest ADC conversion result.
volatile int32_t adcValue = 0;
uint8_t raw1;
uint8_t raw2;
uint8_t raw3;

void delay_ms(unsigned int ms) {
    while (ms-- > 0) {
        asm("repeat #15998");
        asm("nop");
    }
}

// -------------------- I2C Routines --------------------

// Initialize I2C1 as master at approximately 100 kHz.
void I2C1_Init(void) {
    I2C1CONbits.I2CEN = 0;  
    I2C1BRG = 157;  // Calculated for ~100 kHz with Fcy = 16 MHz
    I2C1STAT = 0;   
    I2C1CONbits.I2CEN = 1;  
}

// Generate an I2C start condition.
void I2C1_Start(void) {
    IFS1bits.MI2C1IF = 0; //clear
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN);
    IFS1bits.MI2C1IF = 0; //clear
}

// Generate an I2C stop condition.
void I2C1_Stop(void) {
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN);
    IFS1bits.MI2C1IF = 0; //clear
}

// Write a byte over I2C.
void I2C1_Write(uint8_t data) {
    I2C1TRN = data;
    while(I2C1STATbits.TRSTAT);
    while(I2C1STATbits.ACKSTAT);
    IFS1bits.MI2C1IF = 0; //clear
}

// Read a byte over I2C; if ack==0, send ACK; if ack==1, send NACK.
uint8_t I2C1_Read() {
    uint8_t received;
    I2C1CONbits.RCEN = 1;          
    while(I2C1CONbits.RCEN == 1);          
    //while(!I2C1STATbits.RBF);
    IFS1bits.MI2C1IF = 0; //clear
    received = I2C1RCV;
//    I2C1CONbits.ACKDT = (ack) ? 1 : 0;  
//    I2C1CONbits.ACKEN = 1;         
//    while(I2C1CONbits.ACKEN);
    IFS1bits.MI2C1IF = 0; //clear
    
    return received;
}

// Write a single ADC register.
void ADC_WriteRegister(uint8_t regAddr, uint8_t data) {
    I2C1_Start();
    //I2C1_Write((ADC_I2C_ADDR << 1) | 0);  //old
    I2C1_Write(ADC_I2C_ADDR_write); 
    I2C1_Write(regAddr);
    I2C1_Write(data);
    I2C1_Stop();
}

// Read multiple bytes from the ADC starting at regAddr.
uint8_t ADC_ReadRegisters(uint8_t regAddr) {
    uint8_t data;
    I2C1_Start();
    I2C1_Write(ADC_I2C_ADDR_write);  
    I2C1_Write(regAddr);
    I2C1_Start();                         
    I2C1_Write(ADC_I2C_ADDR_read);
    data = I2C1_Read();
//    for(uint8_t i = 0; i < length; i++) {
//        raw[i] = I2C1_Read((i == (length - 1)) ? 1 : 0);
//    }
    I2C1_Stop();
    return data;
}

// -------------------- ADC Routines --------------------

// Initialize the ADC: power it up and set PGA gain = 1.
void ADC_Init(void) {
    delay(10);
    ADC_WriteRegister(ADC_PU_CTRL_REG, 0x01);  // Power up the ADC
    ADC_WriteRegister(ADC_PU_CTRL_REG, 0x02);  // Power up the ADC
    delay(1); // for 200 us power sequencing delay
//    uint8_t data = 0b11111111;
//    uint8_t masked_data;
//    data = ADC_ReadRegisters(0x00);
//    masked_data = data & 0b00001000;
//    while (masked_data != 0b00001000) {
//        ADC_WriteRegister(ADC_PU_CTRL_REG, 0x01);  // Power up the ADC
//        ADC_WriteRegister(ADC_PU_CTRL_REG, 0x02);  // Power up the ADC
//        data = ADC_ReadRegisters(0x00);
//        masked_data = data & 0b00001000;
//        delay(1);
//    }     
    ADC_WriteRegister(ADC_PU_CTRL_REG, 0x06);  // Power up the analog front end
    ADC_WriteRegister(ADC_CTRL1_REG, 0x00);    // Set PGA gain to 1 (bits 2:0 = 000)
    ADC_WriteRegister(ADC_CTRL2_REG, 0x04);
//    ADC_WriteRegister(0x03, offset1);
//    ADC_WriteRegister(0x04, offset2);
//    ADC_WriteRegister(0x05, offset3);
    
    delay(10);
    
//    uint8_t data2 = 0b11111111;
//    uint8_t masked_data2;
//    data2 = ADC_ReadRegisters(0x00);
//    masked_data2 = data2 & 0b00001000;
//    while (masked_data2 == 0b00001000) {
//        ADC_WriteRegister(ADC_PU_CTRL_REG, 0x01);  // Power up the ADC
//        ADC_WriteRegister(ADC_PU_CTRL_REG, 0x02);  // Power up the ADC
//        data2 = ADC_ReadRegisters(0x00);
//        masked_data2 = data2 & 0b00001000;
//        delay(1);
//    }   

    ADC_WriteRegister(ADC_PU_CTRL_REG, 0x16);  // set cycle start
//    ADC_WriteRegister(ADC_PU_CTRL_REG, 0x10);  // standby
//    delay(1);
//    ADC_WriteRegister(ADC_PU_CTRL_REG, 0x14);  // turn on
}

// Wait for DRDY (Data Ready) to go low.
void ADC_WaitForDataReady(void) {
    uint8_t data;
    data = (ADC_ReadRegisters(0x00)& 0b00100000);
    while (data != 0b00100000) {
        data = (ADC_ReadRegisters(0x00)& 0b00100000);
    }
//    
//    while(DRDY_PORT == 0);  
}
    


// Read a 24-bit conversion result from the ADC.
int32_t ADC_GetConversion(void) {

    int32_t result;
    
   // ADC_WaitForDataReady();
    raw1 = ADC_ReadRegisters(ADC_CONV_REG_1);
    raw2 = ADC_ReadRegisters(ADC_CONV_REG_2);
    raw3 = ADC_ReadRegisters(ADC_CONV_REG_3);
    
    // Combine three bytes (MSB first) into a 24-bit signed integer.
    result = ((int32_t)raw1 << 16) | ((int32_t)raw2 << 8) | raw3;
    
    // Sign extension for 24-bit 2's complement.
    //if(result & 0x800000)
     //   result |= 0xFF000000;
    
    return result;
}

// -------------------- Timer2 Interrupt --------------------

// Timer2 ISR: triggered at 50 Hz (every 20 ms), reads ADC conversion.
//void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt(void) {
//    adcValue = ADC_GetConversion();
//    IFS0bits.T2IF = 0;  // Clear Timer2 interrupt flag.
//}

// Initialize Timer2 to trigger interrupts at 50 Hz.
//void Timer2_Init(void) {
//    T2CONbits.TCKPS = 2;    // Prescaler 1:64 
//    PR2 = 4999;             // PR2 = (16e6 / 64 / 50) - 1 = 4999
//    TMR2 = 0;
//    
//    IPC1bits.T2IP = 4;      // Set Timer2 interrupt priority
//    IFS0bits.T2IF = 0;      // Clear Timer2 interrupt flag
//    IEC0bits.T2IE = 1;      // Enable Timer2 interrupt
//    
//    T2CONbits.TON = 1;      // Start Timer2
//}