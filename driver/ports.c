// *************************************************************************************************
// Button entry functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "ports.h"
#include "buzzer.h"
#include "timer.h"
#include "display.h"

// logic
#include "clock.h"
#include "alarm.h"

// *************************************************************************************************
// Defines section

// Macro for button IRQ
#define IRQ_TRIGGERED(flags, bit)               ((flags & bit) == bit)

// *************************************************************************************************
// Global Variable section
volatile s_button_flags button;
volatile s_button sButton;

// *************************************************************************************************
// Extern section
extern void (*fptr_Timer0_A3_function)(void);

// *************************************************************************************************
// @fn          init_buttons
// @brief       Init and enable button interrupts.
// @param       none
// @return      none
// *************************************************************************************************
void init_buttons(void) {
    // Set button ports to input
    BUTTONS_DIR &= ~ALL_BUTTONS;

    // Enable internal pull-downs
    BUTTONS_OUT &= ~ALL_BUTTONS;
    BUTTONS_REN |= ALL_BUTTONS;

    // IRQ triggers on rising edge
    BUTTONS_IES &= ~ALL_BUTTONS;

    // Reset IRQ flags
    BUTTONS_IFG &= ~ALL_BUTTONS;

    // Enable button interrupts
    BUTTONS_IE |= ALL_BUTTONS;
}

// *************************************************************************************************
// @fn          PORT2_ISR
// @brief       Interrupt service routine for
//                                      - buttons
//                                      - acceleration sensor INT1
//                                      - pressure sensor EOC
// @param       none
// @return      none
// *************************************************************************************************
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void) {
    // Clear flags
    u8 int_flag, int_enable;
    u8 buzzer = 0;

    // Remember interrupt enable bits
    int_enable = BUTTONS_IE;

    if ((!button.flag.star_long) && (!button.flag.num_long)) {
        // Clear button flags
        button.all_flags = 0;

        // Store valid button interrupt flag
        int_flag = BUTTONS_IFG & int_enable;

        // Debounce buttons
        if ((int_flag & ALL_BUTTONS) != 0) {
            // Disable PORT2 IRQ
            __disable_interrupt();
            BUTTONS_IE = 0x00;
            __enable_interrupt();

            // Debounce delay 1
            Timer0_A4_Delay(CONV_MS_TO_TICKS(BUTTONS_DEBOUNCE_TIME_IN));

            // Reset inactivity detection
            sTime.last_activity = sTime.system_time;
        }

        // ---------------------------------------------------
        // STAR button IRQ
        if (IRQ_TRIGGERED(int_flag, BUTTON_STAR_PIN)) {
            // Filter bouncing noise
            if (BUTTON_STAR_IS_PRESSED) {
                button.flag.star = 1;
                button.flag.star_not_long = 0;
                // Generate button click
                buzzer = 1;
            } else if ((BUTTONS_IES & BUTTON_STAR_PIN) == BUTTON_STAR_PIN) {
                button.flag.star = 1;
                button.flag.star_not_long = 0;
                BUTTONS_IES &= ~BUTTON_STAR_PIN;
            }
        }
        // ---------------------------------------------------
        // NUM button IRQ
        else if (IRQ_TRIGGERED(int_flag, BUTTON_NUM_PIN)) {
            // Filter bouncing noise
            if (BUTTON_NUM_IS_PRESSED) {
                button.flag.num = 1;
                button.flag.num_not_long = 0;
                // Generate button click
                buzzer = 1;
            } else if ((BUTTONS_IES & BUTTON_NUM_PIN) == BUTTON_NUM_PIN) {
                button.flag.num = 1;
                button.flag.num_not_long = 0;
                BUTTONS_IES &= ~BUTTON_NUM_PIN;
            }
        }
        // ---------------------------------------------------
        // UP button IRQ
        else if (IRQ_TRIGGERED(int_flag, BUTTON_UP_PIN)) {
            // Filter bouncing noise
            if (BUTTON_UP_IS_PRESSED) {
                button.flag.up = 1;

                // Generate button click
                buzzer = 1;
            }
        }
        // ---------------------------------------------------
        // DOWN button IRQ
        else if (IRQ_TRIGGERED(int_flag, BUTTON_DOWN_PIN)) {
            // Filter bouncing noise
            if (BUTTON_DOWN_IS_PRESSED) {
                button.flag.down = 1;

                // Generate button click
                buzzer = 1;

                // Faster reaction for stopwatch stop button press
                //if (is_stopwatch() && !sys.flag.lock_buttons) {
                //    stop_stopwatch();
                //    button.flag.down = 0;
                //}
            }
        }
        // ---------------------------------------------------
        // B/L button IRQ
        else if (IRQ_TRIGGERED(int_flag, BUTTON_BACKLIGHT_PIN)) {
            // Filter bouncing noise
            if (BUTTON_BACKLIGHT_IS_PRESSED) {
                sButton.backlight_status = 1;
                sButton.backlight_timeout = 0;
                P2OUT |= BUTTON_BACKLIGHT_PIN;
                P2DIR |= BUTTON_BACKLIGHT_PIN;
            }
        }
    }

    // Trying to lock/unlock buttons?
    if (button.flag.num && button.flag.down) {
        // No buzzer output
        buzzer = 0;
        button.all_flags = 0;
    }

    // Generate button click when button was activated
    if (buzzer) {
        // Any button event stops active alarm
        if (sAlarm.state == ALARM_ON) {
            stop_alarm();
            button.all_flags = 0;
        } else if (!sys.flag.up_down_repeat_enabled) {
            start_buzzer(1, CONV_MS_TO_TICKS(20), CONV_MS_TO_TICKS(150));
        }

        // Debounce delay 2
        Timer0_A4_Delay(CONV_MS_TO_TICKS(BUTTONS_DEBOUNCE_TIME_OUT));
    }

    // ---------------------------------------------------
    // Safe long button event detection
    if (button.flag.star || button.flag.num) {
        // Additional debounce delay to enable safe high detection - 50ms
        Timer0_A4_Delay(CONV_MS_TO_TICKS(BUTTONS_DEBOUNCE_TIME_LEFT));

        // Check if this button event is short enough
        if (BUTTON_STAR_IS_PRESSED) {
            // Change interrupt edge to detect button release
            BUTTONS_IES |= BUTTON_STAR_PIN;
            button.flag.star = 0;
            // This flag is used to detect if the user released the button before the
            // time for a long button press (3s)
            button.flag.star_not_long = 1;
        }
        if (BUTTON_NUM_IS_PRESSED) {
            // Change interrupt edge to detect button release
            BUTTONS_IES |= BUTTON_NUM_PIN;
            button.flag.num = 0;
            // This flag is used to detect if the user released the button before the
            // time for a long button press (3s)
            button.flag.num_not_long = 1;
        }
    }

    //}
    // Reenable PORT2 IRQ
    __disable_interrupt();
    BUTTONS_IFG = 0x00;
    BUTTONS_IE = int_enable;
    __enable_interrupt();

    // Exit from LPM3/LPM4 on RETI
    __bic_SR_register_on_exit(LPM4_bits);
}

// *************************************************************************************************
// @fn          button_repeat_on
// @brief       Start button auto repeat timer.
// @param       none
// @return      none
// *************************************************************************************************
void button_repeat_on(u16 msec) {
    // Set button repeat flag
    sys.flag.up_down_repeat_enabled = 1;

    // Set Timer0_A3 function pointer to button repeat function
    fptr_Timer0_A3_function = button_repeat_function;

    // Timer0_A3 IRQ triggers every 200ms
    Timer0_A3_Start(CONV_MS_TO_TICKS(msec));
}

// *************************************************************************************************
// @fn          button_repeat_off
// @brief       Stop button auto repeat timer.
// @param       none
// @return      none
// *************************************************************************************************
void button_repeat_off(void) {
    // Clear button repeat flag
    sys.flag.up_down_repeat_enabled = 0;

    // Timer0_A3 IRQ repeats with 4Hz
    Timer0_A3_Stop();
}

// *************************************************************************************************
// @fn          button_repeat_function
// @brief       Check at regular intervals if button is pushed continuously
//                              and trigger virtual button event.
// @param       none
// @return      none
// *************************************************************************************************
void button_repeat_function(void) {
    static u8 start_delay = 10; // Wait for 2 seconds before starting auto up/down
    u8 repeat = 0;

    // If buttons UP or DOWN are continuously high, repeatedly set button flag
    if (BUTTON_UP_IS_PRESSED) {
        if (start_delay == 0) {
            // Generate a virtual button event
            button.flag.up = 1;
            repeat = 1;
        } else {
            start_delay--;
        }
    } else if (BUTTON_DOWN_IS_PRESSED) {
        if (start_delay == 0) {
            // Generate a virtual button event
            button.flag.down = 1;
            repeat = 1;
        } else {
            start_delay--;
        }
    } else {
        // Reset repeat counter
        sButton.repeats = 0;
        start_delay = 10;

        // Enable blinking
        start_blink();
    }

    // If virtual button event is generated, stop blinking and reset timeout counter
    if (repeat) {
        // Increase repeat counter
        sButton.repeats++;

        // Reset inactivity detection counter
        sTime.last_activity = sTime.system_time;

        // Disable blinking
        stop_blink();
    }
}
