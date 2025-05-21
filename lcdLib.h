/* 
 * File:   lcdLib.h
 * Author: alekshurson
 *
 * Created on April 2, 2024, 11:52 AM
 */

#ifndef LCDLIB_H
#define	LCDLIB_H

#ifdef	__cplusplus
extern "C" {
#endif

void lcd_printChar(char Package);
void lcd_setCursor(char x, char y);
void lcd_init();
void lcd_cmd(char Package);
void lcd_printStr(const char s[]);
void lcd_printVoltage(float V);



#ifdef	__cplusplus
}
#endif

#endif	/* LCDLIB_H */

