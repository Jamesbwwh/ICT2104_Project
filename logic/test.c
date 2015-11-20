// *************************************************************************************************
// Test functions
// *************************************************************************************************

// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "display.h"
#include "ports.h"
#include "timer.h"

// logic
#include "test.h"

// *************************************************************************************************
// @fn          test_mode
// @brief       Manual test mode. Activated by holding buttons STAR and UP simultaneously.
//                              Cancelled by any other button press.
// @param       none
// @return      none
// *************************************************************************************************
void test_mode(void) {
    u8 test_step, start_next_test;

    // Disable timer - no need for a clock tick
    Timer0_Stop();

    // Disable LCD charge pump while in standby mode
    // This reduces current consumption by ca. 5µA to ca. 10µA
    LCDBVCTL = 0;

    // Show welcome screen
    display_chars(LCD_SEG_L1_3_0, (u8 *) "0430", SEG_ON);
    display_chars(LCD_SEG_L2_4_0, (u8 *) "CC430", SEG_ON);
    display_symbol(LCD_SEG_L1_COL, SEG_ON);
    display_symbol(LCD_ICON_HEART, SEG_ON);
    display_symbol(LCD_ICON_STOPWATCH, SEG_ON);
    display_symbol(LCD_ICON_RECORD, SEG_ON);
    display_symbol(LCD_ICON_ALARM, SEG_ON);
    display_symbol(LCD_ICON_BEEPER1, SEG_ON);
    display_symbol(LCD_ICON_BEEPER2, SEG_ON);
    display_symbol(LCD_ICON_BEEPER3, SEG_ON);
    display_symbol(LCD_SYMB_ARROW_UP, SEG_ON);
    display_symbol(LCD_SYMB_ARROW_DOWN, SEG_ON);
    display_symbol(LCD_SYMB_AM, SEG_ON);

    // Hold watchdog
    WDTCTL = WDTPW + WDTHOLD;

    // Wait for button press
    _BIS_SR(LPM3_bits + GIE);
    __no_operation();

    // Clear display
    display_all_off();

#ifdef USE_LCD_CHARGE_PUMP
    // Charge pump voltage generated internally, internal bias (V2-V4) generation
    // This ensures that the contrast and LCD control is constant for the whole battery lifetime
    LCDBVCTL = LCDCPEN | VLCD_2_72;
#endif

    // Renenable timer
    Timer0_Start();

    // Debounce button press
    Timer0_A4_Delay(CONV_MS_TO_TICKS(100));

    while (1) {
        // Check button event
        if (BUTTON_STAR_IS_PRESSED && BUTTON_UP_IS_PRESSED) {
            // Start with test #0
            test_step = 0;
            start_next_test = 1;
            while (1) {
                if (start_next_test) {
                    // Clean up previous test display
                    display_all_off();

                    start_next_test = 0;

                    switch (test_step) {
                    case 0: // All LCD segments on
                        display_all_on();
                        // Wait until buttons are off
                        while (BUTTON_STAR_IS_PRESSED && BUTTON_UP_IS_PRESSED) ;
                        break;
                    }

                    // Debounce button
                    Timer0_A4_Delay(CONV_MS_TO_TICKS(200));
                }

                // Check button event
                if (BUTTON_STAR_IS_PRESSED) {
                    test_step = 1;
                    start_next_test = 1;
                } else if (BUTTON_NUM_IS_PRESSED) {
                    test_step = 2;
                    start_next_test = 1;
                } else if (BUTTON_UP_IS_PRESSED) {
                    test_step = 3;
                    start_next_test = 1;
                } else if (BUTTON_DOWN_IS_PRESSED) {
                    test_step = 4;
                    start_next_test = 1;
                } else if (BUTTON_BACKLIGHT_IS_PRESSED) {
                    // Wait until button has been released (avoid restart)
                    while (BUTTON_BACKLIGHT_IS_PRESSED) ;

                    // Disable LCD and LCD charge pump
                    LCDBCTL0 &= ~BIT0;
                    LCDBVCTL = 0;

                    // Debounce button press
                    Timer0_A4_Delay(CONV_MS_TO_TICKS(500));

                    // Disable timer - no need for a clock tick
                    Timer0_Stop();

                    // Hold watchdog
                    WDTCTL = WDTPW + WDTHOLD;

                    // Sleep until button is pressed (ca. 4µA current consumption)
                    _BIS_SR(LPM4_bits + GIE);
                    __no_operation();

                    // Force watchdog reset for a clean restart
                    WDTCTL = 1;
                }

#ifdef USE_WATCHDOG
                // Service watchdog
                WDTCTL = WDTPW + WDTIS__512K + WDTSSEL__ACLK + WDTCNTCL;
#endif
                // To LPM3
                _BIS_SR(LPM3_bits + GIE);
                __no_operation();
            }
        } else {
            // Debounce button
            Timer0_A4_Delay(CONV_MS_TO_TICKS(100));
            button.all_flags = 0;

            // Turn off backlight
            P2OUT &= ~BUTTON_BACKLIGHT_PIN;
            P2DIR &= ~BUTTON_BACKLIGHT_PIN;
            break;
        }
    }
}

// *************************************************************************************************
// @fn          display_all_on
// @brief       Turns on all LCD segments
// @param       none
// @return      none
// *************************************************************************************************
void display_all_on(void) {
    u8 *lcdptr = (u8 *) 0x0A20;
    u8 i;

    for (i = 1; i <= 12; i++) {
        *lcdptr = 0xFF;
        lcdptr++;
    }
}

// *************************************************************************************************
// @fn          display_all_off
// @brief       Turns off all LCD segments
// @param       none
// @return      none
// *************************************************************************************************
void display_all_off(void) {
    u8 *lcdptr = (u8 *) 0x0A20;
    u8 i;

    for (i = 1; i <= 12; i++) {
        *lcdptr = 0x00;
        lcdptr++;
    }
}
