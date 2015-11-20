#ifndef PORTS_H_
#define PORTS_H_

// *************************************************************************************************
// Defines section

// Port, pins and interrupt resources for buttons
#define BUTTONS_IN              (P2IN)
#define BUTTONS_OUT             (P2OUT)
#define BUTTONS_DIR             (P2DIR)
#define BUTTONS_REN             (P2REN)
#define BUTTONS_IE              (P2IE)
#define BUTTONS_IES             (P2IES)
#define BUTTONS_IFG             (P2IFG)
#define BUTTONS_IRQ_VECT2       (PORT2_VECTOR)

// Button ports
#define BUTTON_STAR_PIN         (BIT2)
#define BUTTON_NUM_PIN          (BIT1)
#define BUTTON_UP_PIN           (BIT4)
#define BUTTON_DOWN_PIN         (BIT0)
#define BUTTON_BACKLIGHT_PIN    (BIT3)
#define ALL_BUTTONS             (BUTTON_STAR_PIN + BUTTON_NUM_PIN + BUTTON_UP_PIN + \
                                 BUTTON_DOWN_PIN + BUTTON_BACKLIGHT_PIN)

// Macros for button press detection
#define BUTTON_STAR_IS_PRESSED          ((BUTTONS_IN & BUTTON_STAR_PIN) == 1)
#define BUTTON_NUM_IS_PRESSED           ((BUTTONS_IN & BUTTON_NUM_PIN) == 1)
#define BUTTON_UP_IS_PRESSED            ((BUTTONS_IN & BUTTON_UP_PIN) == 1)
#define BUTTON_DOWN_IS_PRESSED          ((BUTTONS_IN & BUTTON_DOWN_PIN) == 1)
#define BUTTON_BACKLIGHT_IS_PRESSED     ((BUTTONS_IN & BUTTON_BACKLIGHT_PIN) == 1)
#define NO_BUTTON_IS_PRESSED            ((BUTTONS_IN & ALL_BUTTONS) == 0)

// Macros for button release detection
#define BUTTON_STAR_IS_RELEASED         ((BUTTONS_IN & BUTTON_STAR_PIN) == 0)
#define BUTTON_NUM_IS_RELEASED          ((BUTTONS_IN & BUTTON_NUM_PIN) == 0)
#define BUTTON_UP_IS_RELEASED           ((BUTTONS_IN & BUTTON_UP_PIN) == 0)
#define BUTTON_DOWN_IS_RELEASED         ((BUTTONS_IN & BUTTON_DOWN_PIN) == 0)
#define BUTTON_BACKLIGHT_IS_RELEASED    ((BUTTONS_IN & BUTTON_BACKLIGHT_PIN) == 0)

// Button debounce time (msec)
#define BUTTONS_DEBOUNCE_TIME_IN        (5u)
#define BUTTONS_DEBOUNCE_TIME_OUT       (250u)
#define BUTTONS_DEBOUNCE_TIME_LEFT      (50u)

// Detect if STAR / NUM button is held low continuously
#define LEFT_BUTTON_LONG_TIME           (2u)

// Backlight time  (sec)
#define BACKLIGHT_TIME_ON               (3u)

// Leave set_value() function after some seconds of user inactivity
#define INACTIVITY_TIME                 (30u)

// Set of button flags
typedef union {
    struct {
        // Manual button events
        u16 star : 1;           // Short STAR button press
        u16 num : 1;            // Short NUM button press
        u16 up : 1;             // Short UP button press
        u16 down : 1;           // Short DOWN button press
        u16 backlight : 1;      // Short BACKLIGHT button press
        u16 star_long : 1;      // Long STAR button press
        u16 num_long : 1;       // Long NUM button press
        u16 star_not_long : 1;  // Between short and long STAR button press
        u16 num_not_long : 1;   // Between short and long NUM button press
    } flag;
    u16 all_flags;              // Shortcut to all display flags (for reset)
} s_button_flags;
extern volatile s_button_flags button;

typedef struct {
    u8 star_timeout;            // this variable is incremented each second if STAR button is still pressed
    u8 num_timeout;             // this variable is incremented each second if NUM button is still pressed
    u8 backlight_timeout;       // controls the timeout for the backlight
    u8 backlight_status;        // 1 case backlight is on
    s16 repeats;
} s_button;
extern volatile s_button sButton;

// *************************************************************************************************
// Extern section
extern void button_repeat_on(u16 msec);
extern void button_repeat_off(void);
extern void button_repeat_function(void);
extern void init_buttons(void);

#endif                          /*PORTS_H_ */
