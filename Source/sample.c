/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "PacMan/matrixManager.h"
#include "adc/adc.h"
#include "CAN/CAN.h"


#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	LCD_Initialization();                
	LCD_Clear(Black);
  BUTTON_init();												// BUTTON Initialization       
	ADC_init();
	CAN_Init();
	
	LPC_SC -> PCONP |= (1 << 22);  // TURN ON TIMER 2
	LPC_SC -> PCONP |= (1 << 23);  // TURN ON TIMER 3	
	
	joystick_init();
	init_RIT(0x004C4B40);
	init_timer(2, 0, 0, 3, 0x2625A0); //MR0 1 secondo : 0x17D7840, 25ms : 5F5E10, 10ms : 2625A0
	//enable_timer(3);	//TIMER PER IL SEED RANDOM E SPEAKER
	//enable_timer(0);	//TIMER PER I SECONDI RANDOM DELLE POWER PILLS
	
	GUI_Text(0, 0, (uint8_t *) "GAME OVER IN", White, Black);
	GUI_Text(150, 0, (uint8_t *) "SCORE", White, Black);
	
	GUI_Text(40, 13, (uint8_t *) "60s", White, Black);
	GUI_Text(160, 13, (uint8_t *) "00", White, Black);
	
	createMap();
	
	//enable_timer(1);	//TIMER PER SPEAKER
	//enable_timer(2);	//TIMER PER IL DIPLAY DEI SECONDI RIMANENTI E MOVIMENTO PLAYER + GHOST
	
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);			

	enable_RIT();
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
