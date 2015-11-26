#ifndef PROJECT_H_
#define PROJECT_H_

// *************************************************************************************************
// Include section
#include <cc430x613x.h>
#include "bm.h"

// *************************************************************************************************
// Defines section

// Comment this to not use the LCD charge pump
//#define USE_LCD_CHARGE_PUMP

// Comment this define to build the application without watchdog support
//#define USE_WATCHDOG

// Use/not use filter when measuring physical values
#define FILTER_OFF                                              (0u)
#define FILTER_ON                                               (1u)

// *************************************************************************************************
// Macro section

// Conversion from usec to ACLK timer ticks
#define CONV_US_TO_TICKS(usec)                          (((usec) * 32768) / 1000000)

// Conversion from msec to ACLK timer ticks
#define CONV_MS_TO_TICKS(msec)                          (((msec) * 32768) / 1000)

// *************************************************************************************************
// Typedef section

typedef enum {
    MENU_ITEM_NOT_VISIBLE = 0,            // Menu item is not visible
    MENU_ITEM_VISIBLE                     // Menu item is visible
} menu_t;

// Set of system flags
typedef union {
    struct {
        u16 idle_timeout : 1;             // Timeout after inactivity
        u16 idle_timeout_enabled : 1;     // When in set mode, timeout after a given period
        u16 lock_buttons : 1;             // Lock buttons
        u16 mask_buzzer : 1;              // Do not output buzz for next button event
        u16 up_down_repeat_enabled : 1;   // While in set_value(), create virtual UP/DOWN button
        // events
        u16 low_battery : 1;              // 1 = Battery is low
        u16 use_metric_units : 1;         // 1 = Use metric units, 0 = use English units
        u16 delay_over : 1;               // 1 = Timer delay over
    } flag;
    u16 all_flags;                        // Shortcut to all system flags (for reset)
} s_system_flags;
extern volatile s_system_flags sys;

// Set of request flags
typedef union {
    struct {
        u16 temperature_measurement : 1;  // 1 = Measure temperature
        u16 voltage_measurement : 1;      // 1 = Measure voltage
        u16 altitude_measurement : 1;     // 1 = Measure air pressure
        u16 acceleration_measurement : 1; // 1 = Measure acceleration
        u16 buzzer : 1;                   // 1 = Output buzzer
    } flag;
    u16 all_flags;                        // Shortcut to all request flags (for reset)
} s_request_flags;
extern volatile s_request_flags request;

#endif                                    /*PROJECT_H_ */
