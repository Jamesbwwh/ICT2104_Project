// *************************************************************************************************
// Include section

// *************************************************************************************************
// Prototypes section

// internal functions
extern void reset_alarm(void);
extern void check_alarm(void);
extern void stop_alarm(void);

// menu functions
extern void sx_alarm(u8 line);
extern void mx_alarm(u8 line);
extern void display_alarm(u8 line, u8 update);

// *************************************************************************************************
// Defines section

// Alarm states
#define ALARM_DISABLED          (0u)
#define ALARM_ENABLED           (1u)
#define ALARM_ON                        (2u)

// Keep alarm for 10 on-off cycles
#define ALARM_ON_DURATION       (10u)

// *************************************************************************************************
// Global Variable section
struct alarm {
    u8 state;                   // ALARM_DISABLED, ALARM_ENABLED, ALARM_ON
    u8 duration;                // Alarm duration
    u8 hour;                    // Alarm hour
    u8 minute;                  // Alarm minute
};
extern struct alarm sAlarm;

// *************************************************************************************************
// Extern section
