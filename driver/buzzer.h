#ifndef BUZZER_H_
#define BUZZER_H_

// *************************************************************************************************
// Prototypes section
extern void reset_buzzer(void);
extern void start_buzzer(u8 cycles, u16 on_time, u16 off_time);
extern void stop_buzzer(void);
extern void toggle_buzzer(void);
extern u8 is_buzzer(void);
extern void countdown_buzzer(void);

// *************************************************************************************************
// Defines section

// Buzzer states
#define BUZZER_OFF                                      (0u)
#define BUZZER_ON_OUTPUT_DISABLED                       (1u)
#define BUZZER_ON_OUTPUT_ENABLED                        (2u)

// Buzzer output signal frequency = 32,768kHz/(BUZZER_TIMER_STEPS+1)/2 = 2.7kHz
#define BUZZER_TIMER_STEPS                              (5u)

// Buzzer on time
#define BUZZER_ON_TICKS                                 (CONV_MS_TO_TICKS(20))

// Buzzer off time
#define BUZZER_OFF_TICKS                                (CONV_MS_TO_TICKS(200))

// *************************************************************************************************
// Global Variable section
struct buzzer {
    u8 time;        // Keep output for "time" seconds
    u16 on_time;    // On/off duty
    u16 off_time;
    u8 state;       // Current buzzer output state
};
extern struct buzzer sBuzzer;

// *************************************************************************************************
// Extern section

#endif                          /*BUZZER_H_ */
