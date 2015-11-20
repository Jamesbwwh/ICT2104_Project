#ifndef TIMER_H_
#define TIMER_H_

// *************************************************************************************************
// Prototypes section
extern void Timer0_Init(void);
extern void Timer0_Start(void);
extern void Timer0_Stop(void);
extern void Timer0_A3_Start(u16 ticks);
extern void Timer0_A3_Stop(void);
extern void Timer0_A4_Delay(u16 ticks);
extern void (*fptr_Timer0_A3_function)(void);

// *************************************************************************************************
// Defines section
struct timer {
    u16 timer0_A3_ticks;    // Timer0_A3 periodic delay
};
extern struct timer sTimer;

// Trigger reset when all buttons are pressed
#define BUTTON_RESET_SEC                (3u)

#endif                          /*TIMER_H_ */
