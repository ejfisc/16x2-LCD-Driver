# 16x2 Matrix LCD Driver

## Getting Started
The current implementation of the `delay_us`, `delay_ms` and `pin_write` functions are specific to the Nordic nRF5 SDK. You'll have to modify these functions and the includes to fit your micro and environment.

Define your register select, enable, data 4, data 5, data 6, and data 7 pins in main. Call `lcd_init()`.

## Communicating With The Display
This uses a custom data bus implemented in `lcd_write_data()`, as long as the gpio pins are defined correctly in main, and provided to the `lcd_init()` function, you should have no problem communicating with the display. Functions for printing to the display are defined in the lcd_16x2.h file.