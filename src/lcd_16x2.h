/* ****************************************************************************/
/** 16x2 Liquid Crystal Display Driver

  @File Name
    lcd_16x2.h

  @Summary
    Driver Library for a 16x2 LCD 

  @Description
    Defines functions that allow the user to interact with the 16x2 LCD
******************************************************************************/

#include <inttypes.h>

#ifndef LCD_16X2_H
#define LCD_16X2_H

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define NUM_LINES 2

/*******************************[ High-Level Sensor Functions For General Use ]****************************************/

/*
    @brief Initialize the LCD for 4-bit interface

    @note implements initialization sequence according to datasheet

    @param[in] rs Register Select pin number

    @param[in] en Enable pin number
    
    @param[in] dat4 Data4 pin number

    @param[in] dat5 Data5 pin number

    @param[in] dat6 Data6 pin number

    @param[in] dat7 Data7 pin number
*/
void lcd_init(uint32_t rs, uint32_t en, uint32_t dat4, uint32_t dat5, uint32_t dat6, uint32_t dat7);

/*
    @brief Function for turning the display off
*/
void lcd_display_off(void);

/*
    @brief Function for turning the display on
*/
void lcd_display_on(void);

/*
    @brief Clear the LCD display

    @note sets the cursor position to zero
*/
void lcd_clear(void);

/*
    @brief Return to the home cursor position

    @note sets the cursor position to zero
*/
void lcd_home(void);

/*
    @brief Shift the entire display to the left
*/
void lcd_shift_left(void);

/*
    @brief Shift the entire display to the right
*/
void lcd_shift_right(void);

/*
    @brief Function for turning on the cursor
*/
void lcd_cursor_on(void);

/*
    @brief Function for turning off the cursor
*/
void lcd_cursor_off(void);

/*
    @brief Function for turning on the cursor blink
*/
void lcd_blink_on(void);

/*
    @brief Function for turning off the cursor blink
*/
void lcd_blink_off(void);

/*
    @brief Function for turning on autoscroll

    @note autoscroll starts when writing to a DDRAM address that's off screen
*/
void lcd_autoscroll_on(void);

/*
    @brief Function for turning off autoscroll
*/
void lcd_autoscroll_off(void);

/*
    @brief Function for writing text right to left
*/
void lcd_right_to_left(void);

/*
    @brief Function for writing text left to right
*/
void lcd_left_to_right(void);

/*
    @brief Set cursor position

    @param[in] row row number

    @param[in] col column number
*/
void lcd_set_cursor(uint16_t col, uint8_t row);

/*
    @brief Function for printing a character to LCD at current position

    @param[in] data Character to be written to the screen
*/
void lcd_write_char(char data);

/*
    @brief Write a string to LCD

    @note calls lcd_write_char() for each character in the string

    @param[in] str String to be written to the screen
*/
void lcd_write_string(char * str);

/*
    @brief Function for printing an integer to the LCD

    @note copies the integer into a string and calls lcd_write_string()

    @param[in] num 32-bit integer to write to the LCD
*/
void lcd_write_int(uint32_t num);

/*
    @brief Function for printing a float to the LCD

    @note copies the float into a string and calls lcd_write_string()

    @param[in] num float to write to the LCD
*/
void lcd_write_float(float num);

/*
    @brief Function for sending a command to LCD

    @note middle man function, tells lcd_send() to set the register select to 0 (instruction register)

    @param[in] cmd Command to send to LCD
*/
void lcd_command(uint8_t cmd);

/*
    @brief Function for printing to the LCD

    @note middle man function, tells lcd_send() to set the register select to 1 (data register)

    @param[in] value ASCII value to print to the LCD
*/
void lcd_write(uint8_t value);

/*
    @brief Function for sending something to the LCD

    @note sets the register select pin to instruction register or data register based on mode input

    @note since the LCD is in 4 bit mode, we write the upper 4 bits and then the lower 4 bits of the command/value

    @param[in] value Command or ASCII character to write to LCD

    @param[in] mode Instruction or Data (0 or 1)
*/
void lcd_send(uint8_t value, uint8_t mode);

/*
    @brief Function for transmitting 4-bit data to LCD

    @note sets the data pins to low or high based on the data to be sent

    @param[in] data 4-bit Data to send to LCD
*/
void lcd_write_data(uint8_t data);

/*
    @brief Function for pulsing the enable pin 

    @note sends a 'clock' signal when sending data to the LCD
*/
void enable_pulse(void);

/*******************************[ Low-Level Functions Specific to Hardware/SDK ]****************************************/

/*
    @brief Function for waiting a desired amount of time

    @note This is written to work with the Nordic nRF52 SDK, if you're using a different chip 
	  architecture you'll have to rewrite this to work with your own micro

    @param[in] ms_time The desired wait time in miliseconds
*/
void delay_ms(uint32_t ms_time);

/*
    @brief Function for waiting a desired amount of microseconds

    @note This is written to work with the Nordic nRF52 SDK, if you're using a different chip 
	  architecture you'll have to rewrite this to work with your own micro

    @param[in] us_time The desired wait time in microseconds
*/
void delay_us(uint32_t us_time);

/*
    @brief Function for writing a value to a pin

    @note This is written to work with the Nordic nRF52 SDK, if you're using a different chip 
	  architecture you'll have to rewrite this to work with your own micro

    @param[in] pin_no Pin number to write to

    @param[in] value Value to write to the pin (0 or 1, but I used 32 bit because that's what the nrf function takes)
*/
void pin_write(uint32_t pin_no, uint32_t value);

#endif 