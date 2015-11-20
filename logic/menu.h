#ifndef MENU_H_
#define MENU_H_

// *************************************************************************************************
// Defines section

struct menu {
    void (*sx_function)(u8 line);                  // Pointer to direct function (start, stop etc)
    void (*mx_function)(u8 line);                  // Pointer to sub menu function (change settings, reset counter etc)
    void (*display_function)(u8 line, u8 mode);    // Pointer to display function
    u8 (*display_update)(void);                    // Display update trigger
    const struct menu *next;                       // Pointer to next menu item
};

// *************************************************************************************************
// Extern section

// Line1 navigation
extern const struct menu menu_L1_Time;
extern const struct menu menu_L1_Alarm;

// Line2 navigation
//extern const struct menu menu_L2_Date;
//extern const struct menu menu_L2_Stopwatch;
//extern const struct menu menu_L2_Battery;

// Pointers to current menu item
extern const struct menu *ptrMenu_L1;
extern const struct menu *ptrMenu_L2;

#endif                          /*MENU_H_ */
