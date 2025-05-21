/* 
 * File:   adc_lib.h
 * Author: alekshurson
 *
 * Created on March 19, 2025, 5:28 PM
 */

#ifndef ADC_LIB_H
#define	ADC_LIB_H

#ifdef	__cplusplus
extern "C" {
#endif

void I2C1_Init(void);
void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_Write(uint8_t data);
uint8_t I2C1_Read(uint8_t ack);
void ADC_WriteRegister(uint8_t regAddr, uint8_t data);
void ADC_ReadRegisters(uint8_t regAddr, uint8_t *data, uint8_t length);
void ADC_Init(void);
void ADC_WaitForDataReady(void);
int32_t ADC_GetConversion(void);
void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt(void);
// Initialize Timer2 to trigger interrupts at 50 Hz.
void Timer2_Init(void);
void delay_ms(unsigned int ms);
int32_t ADC_GetLatestValue(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_LIB_H */

