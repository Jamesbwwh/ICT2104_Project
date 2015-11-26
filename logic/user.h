#ifndef USER_H_
#define USER_H_

// *************************************************************************************************
// Defines section
#define SETVALUE_ROLLOVER_VALUE                         BIT0
#define SETVALUE_DISPLAY_VALUE                          BIT1
#define SETVALUE_DISPLAY_ARROWS                         BIT2
#define SETVALUE_DISPLAY_SELECTION                      BIT3
#define SETVALUE_FAST_MODE                              BIT4
#define SETVALUE_NEXT_VALUE                             BIT5

// *************************************************************************************************
// Prototypes section
extern u8 *select_view_style(u8 line, u8 * view1, u8 * view2);
extern void (*fptr_setValue_display_function1)(u8 segments, u32 value, u8 digits, u8 blanks);
extern void set_value(s32 * value, u8 digits, u8 blanks, s32 limitLow, s32 limitHigh, u16 mode, u8 segments,
                      void (*fptr_setValue_display_function1)(u8 segments, u32 value, u8 digits, u8 blanks));
extern void dummy(u8 line);
extern void dummy2(u8 line, u8 update);
extern u8 dummy3(void);

#endif                          /*USER_H_ */
