/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "timer.h"
#include "PacMan/matrixManager.h"
#include "GLCD/GLCD.h"
#include "../music/music.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;
volatile int position=0;
volatile int select=0;
volatile int up=0;
volatile int bottom=0;
volatile int left=0;
volatile int right=0;

//CircularBuffer cb;

extern Player pl;

int currentNote = 0;
int ticks = 0;

void RIT_IRQHandler (void)
{
	
	if(pl.gameStatus!=START) {
		if(!isNotePlaying()) {
			++ticks;
			if(ticks == UPTICKS)
			{
				ticks = 0;
				if(pl.gameStatus==PAUSE)
					playNote(background_melody[currentNote++]);
				else if(pl.gameStatus==WIN)
					playNote(win_melody[currentNote++]);
				else if(pl.gameStatus==LOSE)
					playNote(lose_melody[currentNote++]);
			}
		}
		
		if((currentNote == (sizeof(background_melody) / sizeof(background_melody[0]))) && pl.gameStatus==PAUSE)
			currentNote=0;
		else if((currentNote == (sizeof(win_melody) / sizeof(win_melody[0]))) && pl.gameStatus==WIN)
			currentNote=0;
		else if((currentNote == (sizeof(lose_melody) / sizeof(lose_melody[0]))) && pl.gameStatus==LOSE)
			currentNote=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0) {
		/* Joytick BUTTON pressed */
		select++;
		switch(select) {
			case 1:
				break;
			default:
				break;
		}
	}
	else {
		select=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0) {
		/* Joytick BOTTOM pressed */
		bottom++;
		switch(bottom) {
			case 1:
				updateDirection(&pl, BOTTOM);
				break;
			default:
				break;
		}
	}
	else {
		bottom=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick UP pressed */
		up++;
		switch(up){
			case 1:
				updateDirection(&pl,UP);
				break;
			default:
				break;
		}
	}
	else{
			up=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick LEFT pressed */
		left++;
		switch(left){
			case 1:
				updateDirection(&pl,LEFT);
				break;
			default:
				break;
		}
	}
	else{
			left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick RIGHT pressed */
		right++;
		switch(right){
			case 1:
				updateDirection(&pl,RIGHT);
				break;
			default:
				break;
		}
	}
	else{
			right=0;
	}

	/*************************INT0***************************/
	if(down_0 !=0){
		down_0++;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
			switch(down_0){
				case 2:
					manageGameStatus(&pl);
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	} // end INT0

	/*************************KEY1***************************/
	if(down_1 !=0){
		down_1++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
			switch(down_1){
				case 2:
					/*disable_timer(1);
					startComputation(&cb);
					enable_timer(1);*/
				break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	} // end KEY1

	/*************************KEY2***************************/
	if(down_2 !=0){
		down_2++;
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
			switch(down_2){
				case 2:
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_2=0;		
			NVIC_EnableIRQ(EINT2_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	} // end KEY2
	
	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
