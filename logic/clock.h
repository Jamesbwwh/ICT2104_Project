#ifndef CLOCK_H_
#define CLOCK_H_

// *************************************************************************************************
// Defines section

// Definitions for time format
#define TIMEFORMAT_24H          (0u)
#define TIMEFORMAT_12H          (1u)

// *************************************************************************************************
// Prototypes section
extern void reset_clock(void);
extern void sx_time(u8 line);
extern void mx_time(u8 line);
extern void clock_tick(void);
extern void display_selection_Timeformat1(u8 segments, u32 index, u8 digits, u8 blanks);
extern void display_time(u8 line, u8 update);

// English units support
extern void calc_24H_to_12H(u8 * hours, u8 * timeAM);
extern u8 convert_hour_to_12H_format(u8 hour);
extern u8 is_hour_am(u8 hour);

// *************************************************************************************************
// Global Variable section
struct time {
    u32 system_time;            // Global system time. Used to calculate last activity
    u32 last_activity;          // Inactivity detection (exits set_value() function)
    u8 drawFlag;                // Flag to minimize display updates
    u8 line1ViewStyle;          // Viewing style
    u8 hour;                    // Time data
    u8 minute;                  // Time data
    u8 second;                  // Time data
};
extern struct time sTime;

#endif                          /*CLOCK_H_ */
