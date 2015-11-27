/*
 * lcd.h
 *
 *  Created on: Nov 23, 2015
 *      Author: Hp
 */

#ifndef LCD_H_
#define LCD_H_

#define INITSTATE    0
#define BACKLIGHT   BIT3
#define BL_BUTTON   BIT0
//

// 7-segment character bit assignments
#define L1_SEG_A                   (BIT4)
#define L1_SEG_B                   (BIT5)
#define L1_SEG_C                   (BIT6)
#define L1_SEG_D                   (BIT7)
#define L1_SEG_E                   (BIT2)
#define L1_SEG_F                   (BIT0)
#define L1_SEG_G                   (BIT1)

#define L2_SEG_A                   (BIT0)
#define L2_SEG_B                   (BIT1)
#define L2_SEG_C                   (BIT2)
#define L2_SEG_D                   (BIT3)
#define L2_SEG_E                   (BIT6)
#define L2_SEG_F                   (BIT4)
#define L2_SEG_G                   (BIT5)



// LCD segments/positions
#define LCD_MEM_1                               ((unsigned char*)0x0A20)
#define LCD_MEM_2                               ((unsigned char*)0x0A21) // row 1, 1st digit
#define LCD_MEM_3                               ((unsigned char*)0x0A22) // row 1, 2nd digit
#define LCD_MEM_4                               ((unsigned char*)0x0A23) // row 1, 3r digit
#define LCD_MEM_5                               ((unsigned char*)0x0A24)
#define LCD_MEM_6                               ((unsigned char*)0x0A25) // row 1, 4th digit
#define LCD_MEM_7                               ((unsigned char*)0x0A26) //small notations at the bothr row right most
#define LCD_MEM_8                               ((unsigned char*)0x0A27) //
#define LCD_MEM_9                               ((unsigned char*)0x0A28)
#define LCD_MEM_10                              ((unsigned char*)0x0A29)
#define LCD_MEM_11                              ((unsigned char*)0x0A2A)
#define LCD_MEM_12                              ((unsigned char*)0x0A2B) // ro2 2 1st digit

// Memory assignment
#define LCD_SEG_L1_0_MEM                        (LCD_MEM_6)
#define LCD_SEG_L1_1_MEM                        (LCD_MEM_4)
#define LCD_SEG_L1_2_MEM                        (LCD_MEM_3)
#define LCD_SEG_L1_3_MEM                        (LCD_MEM_2)
#define LCD_SEG_L1_COL_MEM                      (LCD_MEM_1)
#define LCD_SEG_L2_0_MEM                        (LCD_MEM_8)
#define LCD_SEG_L2_1_MEM                        (LCD_MEM_9)
#define LCD_SEG_L2_2_MEM                        (LCD_MEM_10)
#define LCD_SEG_L2_3_MEM                        (LCD_MEM_11)
#define LCD_SEG_L2_4_MEM                        (LCD_MEM_12)
#define LCD_SEG_L2_5_MEM                        (LCD_MEM_12)
#define LCD_SEG_L2_COL1_MEM                     (LCD_MEM_1)
#define LCD_SEG_L2_COL0_MEM                     (LCD_MEM_5)
#define LCD_SYMB_AM_MEM                         (LCD_MEM_1)
#define LCD_SYMB_PM_MEM                         (LCD_MEM_1)
#define LCD_SYMB_ARROW_UP_MEM           		(LCD_MEM_1)
#define LCD_SYMB_ARROW_DOWN_MEM         		(LCD_MEM_1)
#define LCD_ICON_ALARM_MEM                      (LCD_MEM_4)
#define LCD_ICON_BEEPER1_MEM            		(LCD_MEM_5)
#define LCD_ICON_BEEPER2_MEM            		(LCD_MEM_6)
#define LCD_ICON_BEEPER3_MEM            		(LCD_MEM_7)

// Bit masks for write access

#define LCD_SYMB_AM_MASK                        (BIT1 | BIT0)
#define LCD_SYMB_PM_MASK                        (BIT0)
#define LCD_SYMB_ARROW_UP_MASK          		(BIT2)
#define LCD_SYMB_ARROW_DOWN_MASK        		(BIT3)
#define LCD_ICON_ALARM_MASK                     (BIT3)
#define LCD_ICON_BEEPER1_MASK           		(BIT3)
#define LCD_ICON_BEEPER2_MASK           		(BIT3)
#define LCD_ICON_BEEPER3_MASK           		(BIT3)




// Definitions for line access
#define LINE1                                   (0x00)
#define LINE2                                   (0x01)

// LCD display modes
#define SEG_OFF                                 (0x00)
#define SEG_ON                                  (0x01)

void init_lcd(void);
void display_char(unsigned char *, int);
void display_symbol(int);
#endif /* LCD_H_ */
