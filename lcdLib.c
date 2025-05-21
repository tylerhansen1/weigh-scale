#include "xc.h"
#include "lcdLib.h"

void lcd_printChar(char Package) {
    
    I2C2CONbits.SEN = 1;	//Initiate Start condition
    while (I2C2CONbits.SEN == 1); // SEN will clear when Start Bit is complete
    IFS3bits.MI2C2IF = 0; //clear
    
    I2C2TRN = 0b01111100; // 8-bits consisting of the slave address and the R/nW bit
    while (IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0; //clear
    
    I2C2TRN = 0b01000000; // 8-bits consisting of control byte /w RS=1
    while (IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0; // clear
    
    I2C2TRN = Package; // 8-bits consisting of the data byte
    while (IFS3bits.MI2C2IF == 0);
    I2C2CONbits.PEN = 1;
    while (I2C2CONbits.PEN == 1); // PEN will clear when Stop bit is complete
}


void lcd_setCursor(char x, char y)
{
    // Calculate DDRAM address based on row and column
    // Note: The exact calculation might vary depending on the LCD module
    char ddrAddress = 0x40 * y + x; // Assuming each row starts at address 0x40
    
    // Construct command by combining control byte and DDRAM address
    char command = 0b10000000 | ddrAddress;
    
    // Send command to set cursor position
    lcd_cmd(command);
}

void lcd_init() {
    I2C2BRG = 157; //set this to clock freq of 100kHz    157 works
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.I2CEN = 1;
    
    //delay(40);
    
    lcd_cmd(0b00111000);    // function set
    lcd_cmd(0b00111001);    // function set, advance instruction mode
    lcd_cmd(0b00010100);    // interval csc
    lcd_cmd(0b01110000);    // contrast low
    lcd_cmd(0b01010110);    //
    lcd_cmd(0b01101100);    // follower control
    
    delay(200);
    
    lcd_cmd(0b00111000);    // function set
    lcd_cmd(0b00001100);    // Display on
    lcd_cmd(0b00000001);    // clear display
    
    delay(1);
}

void lcd_cmd(char Package) {    
    I2C2CONbits.SEN = 1;	//Initiate Start condition
    while (I2C2CONbits.SEN == 1); // SEN will clear when Start Bit is complete
    IFS3bits.MI2C2IF = 0; //clear
    
    I2C2TRN = 0b01111100; // 8-bits consisting of the slave address and the R/nW bit
    while (IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0; //clear
    
    I2C2TRN = 0b00000000; // 8-bits consisting of control byte /w RS=1
    while (IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0; // clear
    
    I2C2TRN = Package; // 8-bits consisting of the data byte
    while (IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0; // clear
    
    I2C2CONbits.PEN = 1;
    while (I2C2CONbits.PEN == 1); // PEN will clear when Stop bit is complete
}

void lcd_printStr(const char s[]) {
    // Prints a string character-by-character moving the cursor accordingly, across top row only
    for (int i = 0; i < 8; i++) {
        lcd_setCursor(i,0);
        lcd_printChar(s[i]);
    }
}
