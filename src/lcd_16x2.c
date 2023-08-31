/* ****************************************************************************/
/** 16x2 Liquid Crystal Display Driver

  @File Name
    lcd_16x2.c

  @Summary
    Driver Library for a 16x2 LCD 

  @Description
    Implements functions that allow the user to interact with the 16x2 LCD
******************************************************************************/

#include "lcd_16x2.h"
#include <inttypes.h>
#include <string.h>
#include "nrf_delay.h" // Nordic nRF5 SDK specific library for delays
#include "nrf_gpio.h" // Nordic nRF5 SDK specific library for gpio config

static uint32_t rs_pin = 0; // register select pin
static uint32_t en_pin = 0; // enable pin
static uint32_t dat4_pin = 0; // data pins
static uint32_t dat5_pin = 0;
static uint32_t dat6_pin = 0;
static uint32_t dat7_pin = 0;

uint8_t display_function = 0; // used to change 4bit/8bit, line number, and font size
uint8_t display_control = 0; // used to turns display, cursor, and blink on and off
uint8_t display_mode = 0; // use to turn autoscroll on and off, and change text entry
uint8_t row_offsets[4] = {0x00, 0x40, 0x10, 0x50}; // used for setting the cursor

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
void lcd_init(uint32_t rs, uint32_t en, uint32_t dat4, uint32_t dat5, uint32_t dat6, uint32_t dat7) {
    // set pins
    rs_pin = rs;
    en_pin = en;
    dat4_pin = dat4;
    dat5_pin = dat5;
    dat6_pin = dat6;
    dat7_pin = dat7;
    
    display_function = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    
    // according to data sheet, wait at least 40ms after power before sending commands
    delay_ms(50);

    // set rs_pin low to begin commands
    pin_write(rs_pin, 0);

    // this is according to the datasheet
    // put the lcd into 4 bit mode
    
    // we start in 8 bit mode, try to set 4 bit mode
    lcd_write_data(0x03);
    delay_ms(5); // wait min 4.1ms

    // second try
    lcd_write_data(0x03);
    delay_ms(5);

    // third try
    lcd_write_data(0x03);
    delay_us(150);

    // finally, set to 4 bit interface
    lcd_write_data(0x02);

    // set # lines, font size, etc
    lcd_command(LCD_FUNCTIONSET | display_function);

    // turn the display on with no cursor or blinking default
    display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    lcd_display_on();

    // clear it off
    lcd_clear();

    // initialize to default text direction
    display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    lcd_command(LCD_ENTRYMODESET | display_mode);
}

/*
    @brief Function for turning the display off
*/
void lcd_display_off(void) {
    display_control &= ~LCD_DISPLAYON;
    lcd_command(LCD_DISPLAYCONTROL | display_control);
}

/*
    @brief Function for turning the display on
*/
void lcd_display_on(void) {
    display_control |= LCD_DISPLAYON;
    lcd_command(LCD_DISPLAYCONTROL | display_control);
}   

/*
    @brief Clear the LCD display

    @note sets the cursor position to zero
*/
void lcd_clear(void) {
    lcd_command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
    delay_ms(2); 
}

/*
    @brief Return to the home cursor position

    @note sets the cursor position to zero
*/
void lcd_home(void) {
    lcd_command(LCD_RETURNHOME); // set cursor position to zero
    delay_ms(2);
}

/*
    @brief Shift the entire display to the left
*/
void lcd_shift_left(void) {
    lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

/*
    @brief Shift the entire display to the right
*/
void lcd_shift_right(void) {
    lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

/*
    @brief Function for turning on the cursor
*/
void lcd_cursor_on(void) {
    display_control |= LCD_CURSORON;
    lcd_command(LCD_DISPLAYCONTROL | display_control);
}

/*
    @brief Function for turning off the cursor
*/
void lcd_cursor_off(void) {
    display_control &= ~LCD_CURSORON;
    lcd_command(LCD_DISPLAYCONTROL | display_control);
}

/*
    @brief Function for turning on the cursor blink
*/
void lcd_blink_on(void) {
    display_control |= LCD_BLINKON;
    lcd_command(LCD_DISPLAYCONTROL | display_control);
}

/*
    @brief Function for turning off the cursor blink
*/
void lcd_blink_off(void) {
    display_control &= ~LCD_BLINKON;
    lcd_command(LCD_DISPLAYCONTROL | display_control);
}

/*
    @brief Function for turning on autoscroll

    @note autoscroll starts when writing to a DDRAM address that's off screen
*/
void lcd_autoscroll_on(void) {
    display_mode |= LCD_ENTRYSHIFTINCREMENT;
    lcd_command(LCD_ENTRYMODESET | display_mode);
}

/*
    @brief Function for turning off autoscroll
*/
void lcd_autoscroll_off(void) {
    display_mode &= ~LCD_ENTRYSHIFTINCREMENT;
    lcd_command(LCD_ENTRYMODESET | display_mode);
}

/*
    @brief Function for writing text right to left
*/
void lcd_right_to_left(void) {
    display_mode &= ~LCD_ENTRYLEFT;
    lcd_command(LCD_ENTRYMODESET | display_mode);
}

/*
    @brief Function for writing text left to right
*/
void lcd_left_to_right(void) {
    display_mode |= LCD_ENTRYLEFT;
    lcd_command(LCD_ENTRYMODESET | display_mode);
}

/*
    @brief Set cursor position

    @param[in] row row number

    @param[in] col column number
*/
void lcd_set_cursor(uint16_t col, uint8_t row) {
    const size_t max_lines = sizeof(row_offsets) / sizeof(*row_offsets);

    if(row >= max_lines) {
	row = max_lines - 1;
    }	
    if(row >= NUM_LINES) {
	row = NUM_LINES - 1;
    }

    lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));

}

/*
    @brief Function for printing a character to LCD at current position

    @param[in] data Character to be written to the screen
*/
void lcd_write_char(char data) {
    lcd_write(data);
}

/*
    @brief Write a string to LCD

    @note calls lcd_write_char() for each character in the string

    @param[in] str String to be written to the screen
*/
void lcd_write_string(char * str) {
    uint32_t i;
    for(i = 0; str[i] != '\0'; i++)
	lcd_write_char(str[i]);
}

/*
    @brief Function for printing an integer to the LCD

    @note copies the integer into a string and calls lcd_write_string()

    @param[in] num 32-bit integer to write to the LCD
*/
void lcd_write_int(uint32_t num) {
    char str[10];
    sprintf(str, "%d", num);
    lcd_write_string(str);
}

/*
    @brief Function for printing a float to the LCD

    @note copies the float into a string and calls lcd_write_string()

    @param[in] num float to write to the LCD
*/
void lcd_write_float(float num) {
    char str[10];
    sprintf(str, "%.4f", num);
    lcd_write_string(str);
}

/*
    @brief Function for sending a command to LCD

    @note middle man function, tells lcd_send() to set the register select to 0 (instruction register)

    @param[in] cmd Command to send to LCD
*/
inline void lcd_command(uint8_t cmd) {
    lcd_send(cmd, 0);
}

/*
    @brief Function for printing to the LCD

    @note middle man function, tells lcd_send() to set the register select to 1 (data register)

    @param[in] value ASCII value to print to the LCD
*/
void lcd_write(uint8_t value) {
    lcd_send(value, 1);
} 

/*
    @brief Function for sending something to the LCD

    @note sets the register select pin to instruction register or data register based on mode input

    @note since the LCD is in 4 bit mode, we write the upper 4 bits and then the lower 4 bits of the command/value

    @param[in] value Command or ASCII character to write to LCD

    @param[in] mode Instruction or Data (0 or 1)
*/
void lcd_send(uint8_t value, uint8_t mode) {
    pin_write(rs_pin, mode);

    lcd_write_data(value >> 4);
    lcd_write_data(value);
}  

/*
    @brief Function for transmitting 4-bit data to LCD

    @note sets the data pins to low or high based on the data to be sent

    @param[in] data 4-bit Data to send to LCD
*/
void lcd_write_data(uint8_t data) {
    if(data & 1)
	pin_write(dat4_pin, 1);
    else
	pin_write(dat4_pin, 0);
    if(data & 2)
	pin_write(dat5_pin, 1);
    else
	pin_write(dat5_pin, 0);
    if(data & 4)
	pin_write(dat6_pin, 1);
    else
	pin_write(dat6_pin, 0);
    if(data & 8)
	pin_write(dat7_pin, 1);
    else
	pin_write(dat7_pin, 0);

    enable_pulse();
}

/*
    @brief Function for pulsing the enable pin 

    @note sends a 'clock' signal when sending data to the LCD
*/
void enable_pulse(void) {
    pin_write(en_pin, 0);
    delay_us(1);
    pin_write(en_pin, 1);
    delay_us(1);
    pin_write(en_pin, 0);
    delay_us(100);
}

/*******************************[ Low-Level Functions Specific to Hardware/SDK ]****************************************/

/*
    @brief Function for waiting a desired amount of microseconds

    @note This is written to work with the Nordic nRF52 SDK, if you're using a different chip 
	  architecture you'll have to rewrite this to work with your own micro

    @param[in] us_time The desired wait time in microseconds
*/
void delay_us(uint32_t us_time) {
    nrf_delay_us(us_time);
}

/*
    @brief Function for waiting a desired amount of miliseconds

    @note This is written to work with the Nordic nRF52 SDK, if you're using a different chip 
	  architecture you'll have to rewrite this to work with your own micro

    @param[in] ms_time The desired wait time in miliseconds
*/
void delay_ms(uint32_t ms_time) {
    nrf_delay_ms(ms_time);
}

/*
    @brief Function for writing a value to a pin

    @note This is written to work with the Nordic nRF52 SDK, if you're using a different chip 
	  architecture you'll have to rewrite this to work with your own micro

    @param[in] pin_no Pin number to write to

    @param[in] value Value to write to the pin (0 or 1, but I used 32 bit because that's what the nrf function takes)
*/
void pin_write(uint32_t pin_no, uint32_t value) {
    nrf_gpio_pin_write(pin_no, value);
}