/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "PacMan/matrixManager.h"
#include "GLCD/GLCD.h"
#include "music/music.h"
#include "CAN/CAN.h"

extern Ghost ghRed;

uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

//extern CircularBuffer cb;
//int tim0Val=0;

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
//extern int cont;
void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{
		initPowerPill();
		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1			
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern Player pl;

void TIMER1_IRQHandler (void)
{
	if(LPC_TIM1->IR & 1) // MR0
	{ 
		static int sineticks=0;
		/* DAC management */	
		static int currentValue; 
		currentValue = SinTable[sineticks];
		currentValue -= 410;
		currentValue /= 1;
		currentValue += 410;
		LPC_DAC->DACR = currentValue <<6;
		sineticks++;
		if(sineticks==45) sineticks=0;
		
		LPC_TIM1->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM1->IR & 2){ // MR1
		
		LPC_TIM1->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 4){ // MR2
		// your code	
		LPC_TIM1->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 8){ // MR3
		// your code	
		LPC_TIM1->IR = 8;			// clear interrupt flag 
	} 

	return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
int manageGhostStatus = -1;
int waitRespawnGhost = -1;
int check1sec = 0;
int testMusic=0;
int ghostSecCounter=0; //4 step ogni 660 punti. ad ogni aumento il ghost è più veloce

void TIMER2_IRQHandler (void)
{
	if(LPC_TIM2->IR & 1) // MR0
	{		
		movePlayer(&pl);
		
		CAN_TxMsg.data[0] = (pl.score & 0xFFDD) >> 8;
		CAN_TxMsg.data[1] = pl.score & 0xFF;
		CAN_TxMsg.data[2] = pl.lives & 0xFF;
		CAN_TxMsg.data[3] = pl.timerValue & 0xFF;
		CAN_TxMsg.len=4;
		CAN_TxMsg.id = 2;
		CAN_TxMsg.format = STANDARD_FORMAT;
		CAN_TxMsg.type = DATA_FRAME;
		CAN_wrMsg(1,&CAN_TxMsg);
		
		switch(ghRed.ghostDifficult) {
			case 0:
				//500ms
				if(ghostSecCounter>=5) {
					if (waitRespawnGhost == -1) {
						testAstar(&pl,&ghRed);
					}
					ghostSecCounter=0;
				} else {
					ghostSecCounter++;
				}
				break;
			case 1:
				//400ms
					if(ghostSecCounter>=4) {
					if (waitRespawnGhost == -1) {
						testAstar(&pl,&ghRed);
					}
					ghostSecCounter=0;
				} else {
					ghostSecCounter++;
				}
				break;
			case 2:
				//300ms
				if(ghostSecCounter>=3) {
					if (waitRespawnGhost == -1) {
						testAstar(&pl,&ghRed);
					}
					ghostSecCounter=0;
				} else {
					ghostSecCounter++;
				}
				break;
			case 3:
				//200ms
				if(ghostSecCounter>=2) {
					if (waitRespawnGhost == -1) {
						testAstar(&pl,&ghRed);
					}
					ghostSecCounter=0;
				} else {
					ghostSecCounter++;
				}
				break;
		}
		
		if (check1sec==10) {
			displayTimer(&pl);
			
			if (manageGhostStatus>0 && manageGhostStatus<10) {
				manageGhostStatus++;
			} else if (manageGhostStatus == 10) {
				ghRed.enemy=1;
				manageGhostStatus=-1;
			}
			
			if (waitRespawnGhost>0 && waitRespawnGhost<3) {
				waitRespawnGhost++;
			} else if (waitRespawnGhost == 3) {
				resetGhostPos(10,12,REDGHOST,&ghRed);
				waitRespawnGhost=-1;
			}
			check1sec=0;
		} else {
			check1sec++;
		}
		LPC_TIM2->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM2->IR & 2){ // MR1
		
		LPC_TIM2->IR = 2;			// clear interrupt flag 
	}
  return;
}


/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{
  if(LPC_TIM3->IR & 1) // MR0
	{ 
		disable_timer(1);
		LPC_TIM3->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM3->IR & 2){ // MR1
		LPC_TIM3->IR = 2;			// clear interrupt flag 
	}
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/