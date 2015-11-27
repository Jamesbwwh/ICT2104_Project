#include "cc430x613x.h"
#include <string.h>
#include "lcd.h"
#include <stdlib.h>



// L1  char display declarations
const unsigned char L1_lcd_charlist[] = {

    (L1_SEG_A | L1_SEG_B | L1_SEG_C | L1_SEG_D | L1_SEG_E | L1_SEG_F),         		// Displays "0"
    (L1_SEG_B | L1_SEG_C),                                         					// Displays "1"
    (L1_SEG_A | L1_SEG_B | L1_SEG_D | L1_SEG_E | L1_SEG_G),                 		// Displays "2"
    (L1_SEG_A | L1_SEG_B | L1_SEG_C | L1_SEG_D | L1_SEG_G),                 		// Displays "3"
    (L1_SEG_B | L1_SEG_C | L1_SEG_F | L1_SEG_G),                         			// Displays "4"
    (L1_SEG_A | L1_SEG_C | L1_SEG_D | L1_SEG_F | L1_SEG_G),                 		// Displays "5"
    (L1_SEG_A | L1_SEG_C | L1_SEG_D | L1_SEG_E | L1_SEG_F | L1_SEG_G),         		// Displays "6"
    (L1_SEG_A | L1_SEG_B | L1_SEG_C),                                 				// Displays "7"
    (L1_SEG_A | L1_SEG_B | L1_SEG_C | L1_SEG_D | L1_SEG_E | L1_SEG_F | L1_SEG_G),	// Displays "8"
    (L1_SEG_A | L1_SEG_B | L1_SEG_C | L1_SEG_D | L1_SEG_F | L1_SEG_G),         		// Displays "9"

};

// L2  char display declarations
const unsigned char L2_lcd_charlist[] = {

    (L2_SEG_A | L2_SEG_B | L2_SEG_C | L2_SEG_D | L2_SEG_E | L2_SEG_F),         		// Displays "0"
    (L2_SEG_B | L2_SEG_C),                                         					// Displays "1"
    (L2_SEG_A | L2_SEG_B | L2_SEG_D | L2_SEG_E | L2_SEG_G),                 		// Displays "2"
    (L2_SEG_A | L2_SEG_B | L2_SEG_C | L2_SEG_D | L2_SEG_G),                 		// Displays "3"
    (L2_SEG_B | L2_SEG_C | L2_SEG_F | L2_SEG_G),                         			// Displays "4"
    (L2_SEG_A | L2_SEG_C | L2_SEG_D | L2_SEG_F | L2_SEG_G),                 		// Displays "5"
    (L2_SEG_A | L2_SEG_C | L2_SEG_D | L2_SEG_E | L2_SEG_F | L2_SEG_G),         		// Displays "6"
    (L2_SEG_A | L2_SEG_B | L2_SEG_C),                                 				// Displays "7"
    (L2_SEG_A | L2_SEG_B | L2_SEG_C | L2_SEG_D | L2_SEG_E | L2_SEG_F | L2_SEG_G), 	// Displays "8"
    (L2_SEG_A | L2_SEG_B | L2_SEG_C | L2_SEG_D | L2_SEG_F | L2_SEG_G),      		// Displays "9"

};

unsigned char* symbols_memLocs [] = {

	LCD_SYMB_AM_MEM,
	LCD_SYMB_PM_MEM,
	LCD_SYMB_ARROW_UP_MEM,
	LCD_SYMB_ARROW_DOWN_MEM,
	LCD_ICON_ALARM_MEM,
	LCD_ICON_BEEPER1_MEM,
	LCD_ICON_BEEPER2_MEM,
	LCD_ICON_BEEPER3_MEM,
};

const unsigned char symbol_mask[] = {

		LCD_SYMB_AM_MASK,
		LCD_SYMB_PM_MASK,
		LCD_SYMB_ARROW_UP_MASK,
		LCD_SYMB_ARROW_DOWN_MASK,
		LCD_ICON_ALARM_MASK,
		LCD_ICON_BEEPER1_MASK,
		LCD_ICON_BEEPER2_MASK,
		LCD_ICON_BEEPER3_MASK,

};


unsigned char*  L1_memLocs [] ={
		LCD_MEM_2,
		LCD_MEM_3,
		LCD_MEM_4,
		LCD_MEM_6,
};

unsigned char*  L2_memLocs [] ={
		LCD_MEM_12,
		LCD_MEM_11,
		LCD_MEM_10,
		LCD_MEM_9,
		LCD_MEM_8,
};

unsigned char * lcdmem;
//
int mainT(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer


    init_lcd();

    //to display char
   unsigned char mystr[6] = "159";
   display_char(mystr, 2);

    //to display icons
    display_symbol(1);




	return 0;
}

// *************************************************************************************************
// @fn          init_lcd
// @brief       init peripheral.
// @param       none
// @return      none
// *************************************************************************************************
void init_lcd(){

	// Clear entire display memory
	    LCDBMEMCTL |= LCDCLRBM + LCDCLRM;

	    // LCD_FREQ = ACLK/16/8 = 256Hz
	    // Frame frequency = 256Hz/4 = 64Hz, LCD mux 4, LCD on
	    LCDBCTL0 = (LCDDIV0 + LCDDIV1 + LCDDIV2 + LCDDIV3) | (LCDPRE0 + LCDPRE1) | LCD4MUX | LCDON;

	    // LCB_BLK_FREQ = ACLK/8/4096 = 1Hz
	    LCDBBLKCTL = LCDBLKPRE0 | LCDBLKPRE1 | LCDBLKDIV0 | LCDBLKDIV1 | LCDBLKDIV2 | LCDBLKMOD0;

	    // I/O to COM outputs
	    P5SEL |= (BIT5 | BIT6 | BIT7);
	    P5DIR |= (BIT5 | BIT6 | BIT7);

	    // Activate LCD output
	    LCDBPCTL0 = 0xFFFF;  // Select LCD segments S0-S15
	    LCDBPCTL1 = 0x00FF;  // Select LCD segments S16-S22


}
// *************************************************************************************************
// @fn          display_char
// @brief       display string on lcd by looping through each char.
// @param       str: string to be displayed, int for which level
// @return      none
// *************************************************************************************************
void display_char(unsigned  char *mystr, int line){

   int i=0;

   int len = strlen(mystr);

   if (line == 1){ //print items on the first line (top)
	   for (i=0; i<len; i++  )
		{
		   char y  = (char)mystr[i];
		   y = y & 0x0F;
		   lcdmem  = L1_memLocs[i];
		   *lcdmem = (unsigned char) (*lcdmem & 0x00); //clear mem
		   *lcdmem = (unsigned char)(*lcdmem | L1_lcd_charlist[y]);
		}

   }
   else{ //print on line 2 (bottom)
	   for (i=0; i<len; i++  )
	   	{
	   	   char y  = (char)mystr[i];
	   	   y = y & 0x0F;
	   	   lcdmem  = L2_memLocs[i];
	   	   *lcdmem = (unsigned char) (*lcdmem & 0x00); //clear mem
	   	   *lcdmem = (unsigned char)(*lcdmem | L2_lcd_charlist[y]);
	   	}
   }

}



// *************************************************************************************************
// @fn          display symbol
// @brief       sets the mem loc to clear, and bits to show symbol AM, PM, ALARM ICON & beepers
// @param       int: index of icon
// @return      none
// *************************************************************************************************
void display_symbol(int symbol){

	lcdmem  = symbols_memLocs[symbol];
   *lcdmem = (unsigned char) (*lcdmem & ~(0x00)); //clear mem
   *lcdmem = (unsigned char)(*lcdmem | symbol_mask[symbol]);

}

