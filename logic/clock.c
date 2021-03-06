// *************************************************************************************************
// Time functions
// *************************************************************************************************

// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "ports.h"
#include "display.h"
#include "timer.h"

// logic
#include "menu.h"
#include "clock.h"
#include "user.h"

// *************************************************************************************************
// Global Variable section
struct time sTime;

// Display values for time format selection
const u8 selection_Timeformat[][4] = {
    "24H", "12H"
};

// *************************************************************************************************
// @fn          reset_clock
// @brief       Resets clock time to 00:00:00, 24H time format.
// @param       none
// @return      none
// *************************************************************************************************
void reset_clock(void) {
    sTime.system_time = 0; // Set global system time to 0

    // Set main 24H time to start value
    sTime.hour = 4;
    sTime.minute = 30;
    sTime.second = 0;

    sTime.line1ViewStyle = DISPLAY_DEFAULT_VIEW; // Display style of both lines is default (HH:MM)
    sTime.last_activity = 0;                     // Reset timeout detection
}

// *************************************************************************************************
// @fn          clock_tick
// @brief       Add 1 second to system time and to display time
// @param       none
// @return      none
// *************************************************************************************************
void clock_tick(void) {
    // Use sTime.drawFlag to minimize display updates
    // sTime.drawFlag = 1: second
    // sTime.drawFlag = 2: minute, second
    // sTime.drawFlag = 3: hour, minute
    sTime.drawFlag = 1;

    sTime.system_time++;    // Increase global system time

    sTime.second++;         // Add 1 second

    // Add 1 minute
    if (sTime.second == 60) {
        sTime.second = 0;
        sTime.minute++;
        sTime.drawFlag++;

        // Add 1 hour
        if (sTime.minute == 60) {
            sTime.minute = 0;
            sTime.hour++;
            sTime.drawFlag++;

            if (sTime.hour == 24) {
                sTime.hour = 0;
            }
        }
    }
}

// *************************************************************************************************
// @fn          convert_hour_to_12H_format
// @brief       Convert internal 24H time to 12H time.
// @param       u8 hour         Hour in 24H format
// @return      u8              Hour in 12H format
// *************************************************************************************************
u8 convert_hour_to_12H_format(u8 hour) {
    if (hour == 0) return (hour + 12);  // 00:00 .. 11:59 --> AM 12:00 .. 11:59
    else if (hour <= 12) return (hour); // 13:00 .. 23:59 --> PM 01:00 .. 11:59
    else return (hour - 12);
}

// *************************************************************************************************
// @fn          is_hour_am
// @brief       Checks if internal 24H time is AM or PM
// @param       u8 hour         Hour in 24H format
// @return      u8              1 = AM, 0 = PM
// *************************************************************************************************
u8 is_hour_am(u8 hour) {
    if (hour < 12) return (1); // 00:00 .. 11:59 --> AM 12:00 .. 11:59
    else return (0);           // 12:00 .. 23:59 --> PM 12:00 .. 11:59
}

// *************************************************************************************************
// @fn          display_selection_Timeformat
// @brief       Display time format 12H / 24H.
// @param       u8 segments                     Target segments where to display information
//              u32 index                       0 or 1, index for value string
//              u8 digits                       Not used
//              u8 blanks                       Not used
// @return      none
// *************************************************************************************************
void display_selection_Timeformat1(u8 segments, u32 index, u8 digits, u8 blanks) {
    if (index < 2) display_chars(segments, (u8 *) selection_Timeformat[index], SEG_ON_BLINK_ON);
}

// *************************************************************************************************
// @fn          mx_time
// @brief       Clock set routine.
// @param       u8 line         LINE1, LINE2
// @return      none
// *************************************************************************************************
void mx_time(u8 line) {
    u8 select;
    s32 timeformat;
    s16 timeformat1;
    s32 hours;
    s32 minutes;
    s32 seconds;
    u8 *str;

    clear_display_all();    // Clear display

    // Convert global time to local variables
    // Global time keeps on ticking in background until it is overwritten
    if (sys.flag.use_metric_units) timeformat = TIMEFORMAT_24H;
    else timeformat = TIMEFORMAT_12H;
    timeformat1 = timeformat;
    hours = sTime.hour;
    minutes = sTime.minute;
    seconds = sTime.second;

    select = 0;    // Init value index

    // Loop values until all are set or user breaks set
    while (1) {
        // Idle timeout: exit without saving
        if (sys.flag.idle_timeout) {
            // Roll back time format
            if (timeformat1 == TIMEFORMAT_24H) sys.flag.use_metric_units = 1;
            else sys.flag.use_metric_units = 0;
            display_symbol(LCD_SYMB_AM, SEG_OFF);
            break;
        }

        // Button STAR (short): save, then exit
        if (button.flag.star) {
            Timer0_Stop();            // Stop clock timer

            // Store local variables in global clock time
            sTime.hour = hours;
            sTime.minute = minutes;
            sTime.second = seconds;

            Timer0_Start();           // Start clock timer

            display_symbol(LCD_SYMB_AM, SEG_OFF); // Full display update is done when returning from function
            break;
        }

        switch (select) {
        case 0:
        	// Clear LINE1 and LINE2 and AM icon - required when coming back from set_value(seconds)
            clear_display();
            display_symbol(LCD_SYMB_AM, SEG_OFF);

            // Set 24H / 12H time format
            set_value(&timeformat, 1, 0, 0, 1, SETVALUE_ROLLOVER_VALUE + SETVALUE_DISPLAY_SELECTION +
                      SETVALUE_NEXT_VALUE, LCD_SEG_L1_3_1, display_selection_Timeformat1);

            // Modify global time format variable immediately to update AM/PM icon correctly
            if (timeformat == TIMEFORMAT_24H) sys.flag.use_metric_units = 1;
            else sys.flag.use_metric_units = 0;
            select = 1;
            break;

        case 1:
        	// Display HH:MM (LINE1) and .SS (LINE2)
            str = int_to_array(hours, 2, 0);
            display_chars(LCD_SEG_L1_3_2, str, SEG_ON);
            display_symbol(LCD_SEG_L1_COL, SEG_ON);

            str = int_to_array(minutes, 2, 0);
            display_chars(LCD_SEG_L1_1_0, str, SEG_ON);

            str = int_to_array(seconds, 2, 0);
            display_chars(LCD_SEG_L2_1_0, str, SEG_ON);
            display_symbol(LCD_SEG_L2_DP, SEG_ON);

            // Set hours
            set_value(&hours, 2, 0, 0, 23, SETVALUE_ROLLOVER_VALUE + SETVALUE_DISPLAY_VALUE +
                      SETVALUE_NEXT_VALUE, LCD_SEG_L1_3_2, display_hours);
            select = 2;
            break;

        case 2:
        	// Set minutes
            set_value(&minutes, 2, 0, 0, 59, SETVALUE_ROLLOVER_VALUE + SETVALUE_DISPLAY_VALUE +
                      SETVALUE_NEXT_VALUE, LCD_SEG_L1_1_0, display_value);
            select = 3;
            break;

        case 3:
        	// Set seconds
            set_value(&seconds, 2, 0, 0, 59, SETVALUE_ROLLOVER_VALUE + SETVALUE_DISPLAY_VALUE +
                      SETVALUE_NEXT_VALUE, LCD_SEG_L2_1_0, display_value);
            select = 0;
            break;
        }
    }

    button.all_flags = 0;    // Clear button flags
}

// *************************************************************************************************
// @fn          sx_time
// @brief       Time user routine. Toggles view style between HH:MM and SS.
// @param       line            LINE1
// @return      none
// *************************************************************************************************
void sx_time(u8 line) {
    if (sTime.line1ViewStyle == DISPLAY_DEFAULT_VIEW) sTime.line1ViewStyle = DISPLAY_ALTERNATIVE_VIEW;
    else sTime.line1ViewStyle = DISPLAY_DEFAULT_VIEW;
}

// *************************************************************************************************
// @fn          display_time
// @brief       Clock display routine. Supports 24H and 12H time format.
// @param       u8 line                 LINE1
//              u8 update               DISPLAY_LINE_UPDATE_FULL, DISPLAY_LINE_UPDATE_PARTIAL
// @return      none
// *************************************************************************************************
void display_time(u8 line, u8 update) {
    u8 hour12;

    if (update == DISPLAY_LINE_UPDATE_PARTIAL) {
        if (sTime.drawFlag != 0) {
            if (sTime.line1ViewStyle == DISPLAY_DEFAULT_VIEW) {
                switch (sTime.drawFlag) {
                case 3:
                    if (sys.flag.use_metric_units) {
                        // Display 24H time "HH"
                        display_chars(switch_seg(line, LCD_SEG_L1_3_2, LCD_SEG_L2_3_2), int_to_array(sTime.hour, 2, 0), SEG_ON);
                    } else {
                        // Display 12H time "HH" + AM/PM
                        hour12 = convert_hour_to_12H_format(sTime.hour);
                        display_chars(switch_seg(line, LCD_SEG_L1_3_2, LCD_SEG_L2_3_2), int_to_array(hour12, 2, 0), SEG_ON);
                        display_am_pm_symbol(sTime.hour);
                    }
                case 2:
                    display_chars(switch_seg(line, LCD_SEG_L1_1_0, LCD_SEG_L2_1_0), int_to_array(sTime.minute, 2, 0), SEG_ON);
                }
            } else {
                display_chars(switch_seg(line, LCD_SEG_L1_1_0, LCD_SEG_L2_1_0), int_to_array(sTime.second, 2, 0), SEG_ON);
            }
        }
    } else if (update == DISPLAY_LINE_UPDATE_FULL) {
        if (sTime.line1ViewStyle == DISPLAY_DEFAULT_VIEW) {
            if (sys.flag.use_metric_units) {
                // Display 24H time "HH"
                display_chars(switch_seg(line, LCD_SEG_L1_3_2, LCD_SEG_L2_3_2), int_to_array(sTime.hour, 2, 0), SEG_ON);
            } else {
                // Display 12H time "HH" + AM/PM information
                hour12 = convert_hour_to_12H_format(sTime.hour);
                display_chars(switch_seg(line, LCD_SEG_L1_3_2, LCD_SEG_L2_3_2), int_to_array(hour12, 2, 0), SEG_ON);
                // Display AM/PM information
                if (line == LINE1) display_am_pm_symbol(sTime.hour);
            }
            display_chars(switch_seg(line, LCD_SEG_L1_1_0, LCD_SEG_L2_1_0), int_to_array(sTime.minute, 2, 0), SEG_ON);
            display_symbol(switch_seg(line, LCD_SEG_L1_COL, LCD_SEG_L2_COL0), SEG_ON_BLINK_ON);
        } else {
            display_chars(switch_seg(line, LCD_SEG_L1_1_0, LCD_SEG_L2_1_0), int_to_array(sTime.second, 2, 0), SEG_ON);
            display_symbol(switch_seg(line, LCD_SEG_L1_DP1, LCD_SEG_L2_DP), SEG_ON);
        }
    } else if (update == DISPLAY_LINE_CLEAR) {
        display_symbol(switch_seg(line, LCD_SEG_L1_COL, LCD_SEG_L2_COL0), SEG_OFF_BLINK_OFF); // Change display style to default (HH:MM)
        sTime.line1ViewStyle = DISPLAY_DEFAULT_VIEW;        // Clean up AM/PM icon
        display_symbol(LCD_SYMB_AM, SEG_OFF);
    }
}
