// *************************************************************************************************
// Menu management functions
// *************************************************************************************************

// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "display.h"

// logic
#include "menu.h"
#include "user.h"
#include "clock.h"
#include "alarm.h"

// *************************************************************************************************
// Defines section
#define FUNCTION(function)  function

// *************************************************************************************************
// Global Variable section
const struct menu *ptrMenu_L1 = NULL;
const struct menu *ptrMenu_L2 = NULL;

// *************************************************************************************************
// Global Variable section

void display_nothing(u8 line, u8 update) {
}

u8 update_time(void) {
    return (display.flag.update_time);
}

u8 update_stopwatch(void) {
    return (display.flag.update_stopwatch);
}

u8 update_date(void) {
    return (display.flag.update_date);
}

u8 update_alarm(void) {
    return (display.flag.update_alarm);
}

u8 update_battery_voltage(void) {
    return (display.flag.update_battery_voltage);
}

// *************************************************************************************************
// User navigation ( [____] = default menu item after reset )
//
//      LINE1:  [Time] -> Alarm
//
//      LINE2:  [Date] -> Stopwatch -> Battery
// *************************************************************************************************

// Line1 - Time
const struct menu menu_L1_Time = {
    FUNCTION(sx_time),                // direct function
    FUNCTION(mx_time),                // sub menu function
    FUNCTION(display_time),           // display function
    FUNCTION(update_time),            // new display data
    &menu_L1_Alarm,
};

// Line1 - Alarm
const struct menu menu_L1_Alarm = {
    FUNCTION(sx_alarm),               // direct function
    FUNCTION(mx_alarm),               // sub menu function
    FUNCTION(display_alarm),          // display function
    FUNCTION(update_alarm),           // new display data
    &menu_L1_Time,
};

// Line2 - Date
const struct menu menu_L2_Date = {
    FUNCTION(dummy),                // direct function
    FUNCTION(dummy),                // sub menu function
    FUNCTION(dummy2),               // display function
    FUNCTION(dummy3),               // new display data
    &menu_L2_Date,
};

//// Line2 - Stopwatch
//const struct menu menu_L2_Stopwatch = {
//    FUNCTION(sx_stopwatch),           // direct function
//    FUNCTION(mx_stopwatch),           // sub menu function
//    FUNCTION(display_stopwatch),      // display function
//    FUNCTION(update_stopwatch),       // new display data
//    &menu_L2_Battery,
//};
//
//// Line2 - Battery
//const struct menu menu_L2_Battery = {
//    FUNCTION(dummy),                  // direct function
//    FUNCTION(dummy),                  // sub menu function
//    FUNCTION(display_battery_V),      // display function
//    FUNCTION(update_battery_voltage), // new display data
//    &menu_L2_Date,
//};
