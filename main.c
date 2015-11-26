// *************************************************************************************************
// Initialization and control of application.
// *************************************************************************************************

// *************************************************************************************************
// Include section

// system
#include "project.h"
#include <string.h>

// driver
#include "clock.h"
#include "display.h"
#include "buzzer.h"
#include "ports.h"
#include "timer.h"
#include "pmm.h"

// logic
#include "menu.h"
#include "alarm.h"

// *************************************************************************************************
// Prototypes section
void init_application(void);
void init_global_variables(void);
void wakeup_event(void);
void process_requests(void);
void display_update(void);
void to_lpm(void);
void configure_ports(void);

// *************************************************************************************************
// Global Variable section

// Variable holding system internal flags
volatile s_system_flags sys;

// Variable holding flags set by logic modules
volatile s_request_flags request;

// Function pointers for LINE1 and LINE2 display function
void (*fptr_lcd_function_line1)(u8 line, u8 update);
void (*fptr_lcd_function_line2)(u8 line, u8 update);

// *************************************************************************************************
// @fn          main
// @brief       Main routine
// @param       none
// @return      none
// *************************************************************************************************
int main(void) {
    // Init MCU
    init_application();

    // Assign initial value to global variables
    init_global_variables();

    // Main control loop: wait in low power mode until some event needs to be processed
    while (1) {
        // When idle go to LPM3
        to_lpm();

        // Process wake-up events
        if (button.all_flags || sys.all_flags)
            wakeup_event();

        // Process actions requested by logic modules
        if (request.all_flags)
            process_requests();

        // Before going to LPM3, update display
        if (display.all_flags)
            display_update();
    }
}

// *************************************************************************************************
// @fn          init_application
// @brief       Initialize the microcontroller.
// @param       none
// @return      none
// *************************************************************************************************
void init_application(void) {
    volatile unsigned char *ptr;

    WDTCTL = WDTPW + WDTHOLD;

    // ---------------------------------------------------------------------
    // Configure PMM
    SetVCore(3);

    // Set global high power request enable
    PMMCTL0_H = 0xA5;
    PMMCTL0_L |= PMMHPMRE;
    PMMCTL0_H = 0x00;

    // ---------------------------------------------------------------------
    // Enable 32kHz ACLK
    P5SEL |= 0x03;              // Select XIN, XOUT on P5.0 and P5.1
    UCSCTL6 &= ~XT1OFF;         // XT1 On, Highest drive strength
    UCSCTL6 |= XCAP_3;          // Internal load cap

    UCSCTL3 = SELA__XT1CLK;     // Select XT1 as FLL reference
    UCSCTL4 = SELA__XT1CLK | SELS__DCOCLKDIV | SELM__DCOCLKDIV;

    // ---------------------------------------------------------------------
    // Configure CPU clock for 12MHz
    _BIS_SR(SCG0);              // Disable the FLL control loop
    UCSCTL0 = 0x0000;           // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_5;        // Select suitable range
    UCSCTL2 = FLLD_1 + 0x16E;   // Set DCO Multiplier
    _BIC_SR(SCG0);              // Enable the FLL control loop

    // Worst-case settling time for the DCO when the DCO range bits have been
    // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
    // UG for optimization.
    // 32 x 32 x 12 MHz / 32,768 Hz = 375000 = MCLK cycles for DCO to settle
    __delay_cycles(375000);

    // Loop until XT1 & DCO stabilizes, use do-while to insure that
    // body is executed at least once
    do {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
        SFRIFG1 &= ~OFIFG;      // Clear fault flags
    } while ((SFRIFG1 & OFIFG));

    // ---------------------------------------------------------------------
    // Configure port mapping

    __disable_interrupt();    // Disable all interrupts
    PMAPPWD = 0x02D52;        // Get write-access to port mapping registers
    PMAPCTL = PMAPRECFG;      // Allow reconfiguration during runtime

    ptr = &P2MAP0;            // P2.7 = TA0CCR1A or TA1CCR0A output (buzzer output)
    *(ptr + 7) = PM_TA1CCR0A;
    P2OUT &= ~BIT7;
    P2DIR |= BIT7;

    ptr = &P1MAP0;
    *(ptr + 5) = PM_UCA0SOMI;    // P1.5 = SPI MISO input
    *(ptr + 6) = PM_UCA0SIMO;    // P1.6 = SPI MOSI output
    *(ptr + 7) = PM_UCA0CLK;     // P1.7 = SPI CLK output

    PMAPPWD = 0;             // Disable write-access to port mapping registers
    __enable_interrupt();    // Re-enable all interrupts

    lcd_init();
    init_buttons();
    Timer0_Init();
}

// *************************************************************************************************
// @fn          init_global_variables
// @brief       Initialize global variables.
// @param       none
// @return      none
// *************************************************************************************************
void init_global_variables(void) {
    // set menu pointers to default menu items
    ptrMenu_L1 = &menu_L1_Time;
    ptrMenu_L2 = &menu_L2_Date;

    // Assign LINE1 and LINE2 display functions
    fptr_lcd_function_line1 = ptrMenu_L1->display_function;
    fptr_lcd_function_line2 = ptrMenu_L2->display_function;

    // Init system flags
    button.all_flags = 0;
    sys.all_flags = 0;
    request.all_flags = 0;
    display.all_flags = 0;

    // Force full display update when starting up
    display.flag.full_update = 1;

#ifndef ISM_US
    // Use metric units when displaying values
    sys.flag.use_metric_units = 1;
#endif

    reset_clock();    // Set system time to default value
    reset_alarm();    // Set alarm time to default value
    reset_buzzer();   // Set buzzer to default value
}

// *************************************************************************************************
// @fn          wakeup_event
// @brief       Process external / internal wakeup events.
// @param       none
// @return      none
// *************************************************************************************************
void wakeup_event(void) {
    // Enable idle timeout
    sys.flag.idle_timeout_enabled = 1;

    if (button.flag.star_long) {
        button.flag.star_long = 0;        // Clear button event
        ptrMenu_L1->mx_function(LINE1);   // Call sub menu function
        display.flag.full_update = 1;     // Set display update flag
    } else if (button.flag.num_long) {
        button.flag.num_long = 0;         // Clear button event
        ptrMenu_L2->mx_function(LINE2);   // Call sub menu function
        display.flag.full_update = 1;     // Set display update flag
    } else if (button.flag.star) {
		fptr_lcd_function_line1(LINE1, DISPLAY_LINE_CLEAR);     // Clean up display before activating next menu item
		ptrMenu_L1 = ptrMenu_L1->next;                          // Go to next menu entry
		fptr_lcd_function_line1 = ptrMenu_L1->display_function; // Assign new display function
		display.flag.line1_full_update = 1;                     // Set Line1 display update flag
		button.flag.star = 0;                                   // Clear button flag
	} else if (button.flag.num) {
		fptr_lcd_function_line2(LINE2, DISPLAY_LINE_CLEAR);     // Clean up display before activating next menu item
		ptrMenu_L2 = ptrMenu_L2->next;                          // Go to next menu entry
		fptr_lcd_function_line2 = ptrMenu_L2->display_function; // Assign new display function
		display.flag.line2_full_update = 1;                     // Set Line2 display update flag
		button.flag.num = 0;                                    // Clear button flag
	} else if (button.flag.up) {
		ptrMenu_L1->sx_function(LINE1);     // Call direct function
		display.flag.line1_full_update = 1; // Set Line1 display update flag
		button.flag.up = 0;                 // Clear button flag
	} else if (button.flag.down) {
		ptrMenu_L2->sx_function(LINE2);     // Call direct function
		display.flag.line2_full_update = 1; // Set Line1 display update flag
		button.flag.down = 0;               // Clear button flag
	}
    // Process internal events
    if (sys.flag.idle_timeout) {
        sys.flag.idle_timeout = 0;            // Clear timeout flag
        clear_display();                      // Clear display
        display.flag.full_update = 1;         // Set display update flags
    }
    sys.flag.idle_timeout_enabled = 0;            // Disable idle timeout
}

// *************************************************************************************************
// @fn          process_requests
// @brief       Process requested actions outside ISR context.
// @param       none
// @return      none
// *************************************************************************************************
void process_requests(void) {
    // Generate alarm (two signals every second)
    if (request.flag.buzzer)
        start_buzzer(2, BUZZER_ON_TICKS, BUZZER_OFF_TICKS);

    // Reset request flag
    request.all_flags = 0;
}

// *************************************************************************************************
// @fn          display_update
// @brief       Process display flags and call LCD update routines.
// @param       none
// @return      none
// *************************************************************************************************
void display_update(void) {
    if (display.flag.full_update || display.flag.line1_full_update) {
        clear_line(LINE1);
        fptr_lcd_function_line1(LINE1, DISPLAY_LINE_UPDATE_FULL);
    } else if (ptrMenu_L1->display_update()) fptr_lcd_function_line1(LINE1, DISPLAY_LINE_UPDATE_PARTIAL);

    if (display.flag.full_update || display.flag.line2_full_update) {
        clear_line(LINE2);
        fptr_lcd_function_line2(LINE2, DISPLAY_LINE_UPDATE_FULL);
    } else if (ptrMenu_L2->display_update()) fptr_lcd_function_line2(LINE2, DISPLAY_LINE_UPDATE_PARTIAL);
    display.all_flags = 0;    // Clear display flag
}

// *************************************************************************************************
// @fn          to_lpm
// @brief       Go to LPM0/3.
// @param       none
// @return      none
// *************************************************************************************************
void to_lpm(void) {
    // Go to LPM3
    _BIS_SR(LPM3_bits + GIE);
    __no_operation();
}
