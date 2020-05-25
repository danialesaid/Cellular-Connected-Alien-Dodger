#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "lcd.h"
#include "timer.h"
#include "bit.h"
#include "usart_ATmega1284.h"

#define CUSTOM_PLAYER_CHAR 1
#define BLOCK_CUSTOM_CHAR 2

void ADC_init()
{
	ADCSRA |= (1 << ADEN) | (1 << ADSC);
}

unsigned char LCD[33];

//syntax sugar
char BLOCK = 1;
char PLAYER = 2;
char DIAGONAL_BLOCK = 3;
char SPACE = 32;
char OFFLCD = 34;

//globals
unsigned char playerHitBlock = 0;
short playerScore;
unsigned char JS_TIME = 2;
unsigned char JSTimer = 0;
	
unsigned char BLOCK_UPDATE_TIME = 5;
unsigned char blockUpdateTimer = 0;

unsigned char BLOCK_GENERATE_TIME = 5;
unsigned char blockGenerateTimer = 0;
	
unsigned char BLOCK_UPSPEED_TIME = 100;
unsigned char blockUpspeedTimer = 0;
	

unsigned char playerScoreTimer = 0;
unsigned char PLAYER_SCORE_UPDATE_TIME = 10;



enum JSStates {JSInit, checkJS} JSState;
unsigned char ADC_MUX_LEFT_RIGHT = 0;
unsigned char ADC_MUX_UP_DOWN = 1;
unsigned short ADCUpDown;
unsigned short ADCLeftRight;
unsigned char cursorPosition = 1;
void JSTick()
{
	switch(JSState)
	{
		case JSInit:
			JSState = checkJS;
			break;

		case checkJS:
			JSState = checkJS;
			break;

		default:
			break;
	}
	switch(JSState)
	{
		case JSInit:
			cursorPosition = 1;
			break;

		case checkJS:
			LCD[cursorPosition] = 0;
				
			ADMUX = ADC_MUX_LEFT_RIGHT;
			ADCSRA |= (1 << ADSC);
			while( ADCSRA & (1 << ADSC) );
			ADCLeftRight = ADC;
		
			ADMUX = ADC_MUX_UP_DOWN;
			ADCSRA |= (1 << ADSC);
			while( ADCSRA & (1 << ADSC) );
			ADCUpDown = ADC;
		
			if(ADCUpDown < 200)
			{
				if(cursorPosition >= 17 && cursorPosition < 29)
				{
					cursorPosition -= 16;
				}
			}
			else if(ADCUpDown > 500)
			{
				if(cursorPosition <= 16)
				{
					cursorPosition += 16;
				}
			}
			if(ADCLeftRight < 200)
			{
				if(cursorPosition != 1 && cursorPosition != 17)
				{
					cursorPosition -= 1;
				}
			}
		
			else if(ADCLeftRight > 500)
			{
				if(cursorPosition != 16 && cursorPosition != 32 && cursorPosition != 13 && cursorPosition != 14 && cursorPosition != 15 && cursorPosition != 16)
				{
					cursorPosition += 1;
				}
			}
			
			if(LCD[cursorPosition] == BLOCK)
			{
				playerHitBlock = 1;
			}
			
			
			LCD[cursorPosition] = PLAYER;
			break;

		default:
			break;
	}
}


enum generateBlocksStates {generateBlocksInit, generateBlocks} generateBlocksState;
unsigned char regularGenerationThreshold = 70;
unsigned char newBlockType;
unsigned char newBlockStartPos;
int randomlyGeneratedInt;
void generateBlocksTick()
{
	switch(generateBlocksState)
	{
		case generateBlocksInit:
		generateBlocksState = generateBlocks;
		break;

		case generateBlocks:
		generateBlocksState = generateBlocks;
		break;

		default:
		break;
	}

	switch(generateBlocksState)
	{
		case generateBlocksInit:
		break;

		case generateBlocks:
		randomlyGeneratedInt = rand();
		randomlyGeneratedInt = randomlyGeneratedInt % 2;
		if(randomlyGeneratedInt == 0)
		{
			if(LCD[29] != BLOCK)
			{
				newBlockStartPos = 13;
			}
			else
			{
				break;
			}
		}
		else
		{
			//if(LCD[16] != BLOCK)
			//{
				newBlockStartPos = 32;
			//}
			//else
			//{
				//break;
			//}
		}
		
		randomlyGeneratedInt = rand();
		randomlyGeneratedInt = randomlyGeneratedInt % 100;
		
		if(randomlyGeneratedInt > regularGenerationThreshold)
		{
			LCD[newBlockStartPos] = BLOCK;
		}
		break;

		default:
		break;
	}
}


enum updateBlocksStates {updateBlocksInit, updateBlocks} updateBlocksState;

void updateBlocksTick()
{
	switch(updateBlocksState)
	{
		case updateBlocksInit:
			updateBlocksState = updateBlocks;
			break;

		case updateBlocks:
			updateBlocksState = updateBlocks;
			break;

		default:
			break;
	}
	switch(updateBlocksState)
	{
		case updateBlocksInit:
			break;

		case updateBlocks:
		

		
			for(int cell = 1; cell <= 32; ++cell)
			{
				
				if(LCD[cell] == BLOCK)
				{
					LCD[cell] = 0;
					if(cell != 1 && cell != 17)
					{	
						if(LCD[cell - 1] == PLAYER)
						{
							playerHitBlock= 1;
						}
						//else
						//{
							//playerHitBlock = 0;
						//}
						LCD[cell - 1] = BLOCK;	
					}
				}
			}
			break;

		default:
			break;
	}
}


enum LCDUpdateStates {LCDUpdateInit, updateLCD} LCDUpdateState;
char playerScoreStr[4];
void updateLCDTick()
{
	switch(LCDUpdateState)
	{
		case LCDUpdateInit:
		LCDUpdateState = updateLCD;
		break;

		case updateLCD:
		LCDUpdateState = updateLCD;
		break;

		default:
		break;
	}
	switch(LCDUpdateState)
	{
		case LCDUpdateInit:
		break;

		case updateLCD:
		

		for(int cell = 1; cell <= 12; ++cell)
		{
			LCD_Cursor(cell);
			if(LCD[cell] == PLAYER)
			{
				LCD_WriteData(CUSTOM_PLAYER_CHAR);
			}
			else if(LCD[cell] == BLOCK)
			{
				LCD_WriteData(BLOCK_CUSTOM_CHAR);
			}
			else
			{
				LCD_WriteData(SPACE);
			}
		}

		for(int cell = 17; cell <= 32; ++cell)
		{
			LCD_Cursor(cell);
			if(LCD[cell] == PLAYER)
			{
				LCD_WriteData(CUSTOM_PLAYER_CHAR);
			}
			else if(LCD[cell] == BLOCK)
			{
				LCD_WriteData(BLOCK_CUSTOM_CHAR);
			}
			else
			{
				LCD_WriteData(SPACE);
			}
		}

		LCD_Cursor(OFFLCD);
		
		if(playerHitBlock)
		{
			JSState = JSInit;
			generateBlocksState = generateBlocksInit;
			updateBlocksState = updateBlocksInit;
			LCDUpdateState = LCDUpdateInit;

			JS_TIME = 2;
			JSTimer = 0;
	
			BLOCK_UPDATE_TIME = 5;
			blockUpdateTimer = 0;

			BLOCK_GENERATE_TIME = 5;
			blockGenerateTimer = 0;
			
			BLOCK_UPSPEED_TIME = 100;
			blockUpspeedTimer = 0;
			
			playerScoreTimer = 0;
			PLAYER_SCORE_UPDATE_TIME = 10;
			cursorPosition = 1;
		
			itoa(playerScore, playerScoreStr, 10);
			LCD_DisplayString(1, "Game Over");
			LCD_DisplayString_M(17, "Score: ");
			LCD_DisplayString_M( 24, playerScoreStr);
			
			
			
			
			playerHitBlock = 0;
			
			//https://www.electronicwings.com/avr-atmega/hc-05-bluetooth-module-interfacing-with-atmega1632

					int j;
					//if(playerScore < 10)
					//{
						//j = 1;
					//}
					//else if(playerScore < 100)
					//{
						//j =2;
					//}
					//else if(playerScore<1000)
					//{
						//
						//j=3;
					//}
					//else
					//{
						//
						//j=4;
					//}
					j=4;
					for(int i =0;i<j;++i)
					{
						//if (USART_IsSendReady(0))
						//{
							USART_Send(playerScoreStr[i], 0);
						//}
					}
					
				//if (USART_IsSendReady(0))
										//{
					//USART_Send('X', 0);
										//}
			//	}
			delay_ms(5000);
			playerScore = 0;
			
					for(int cell = 1; cell <= 32; ++cell)
					{
						LCD[cell] = 0;
					}
			break;
		}
		
		break;
		
		default:
		break;
	}
}
 
 
//custom characters reference:
//https://www.electronicwings.com/avr-atmega/lcd-custom-character-display-using-atmega-16-32-
 void LCDBuildChar(unsigned char loc, unsigned char *p)
 {
	 unsigned char i;
	 if(loc<8) //If valid address
	 {
		 LCD_WriteCommand(0x40+(loc*8)); //Write to CGRAM
		 for(i=0;i<8;i++)
		 {
			 
		 
		 LCD_WriteData(p[i]); //Write the character pattern to CGRAM
		 }
	 }

	 LCD_WriteCommand(0x80); //shift back to DDRAM location 0
 }
 
 
 
int main(void)
{	
	//custom characters reference: 
	//https://www.electronicwings.com/avr-atmega/lcd-custom-character-display-using-atmega-16-32-
	 
	unsigned char customPlayer[8] = {0b00001110,
									 0b00001110,
									 0b00000100,
									 0b00011111,
									 0b00000100,
									 0b00000100,
									 0b00001010,
									 0b00000000};	

unsigned char customBlock[8] = {0b00000000,
								0b00001111,
								0b00011000,
								0b00111000,
								0b00011000,
								0b00001111,
								0b00000000,
								0b00000000};


	DDRD = 0xF0; PORTD = 0x0F;
	
	//DDRD = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	
	LCD_init();
	LCDBuildChar(1, customPlayer);
	LCDBuildChar(2, customBlock);

	initUSART(0);
	ADC_init();	
	TimerSet(100);
	TimerOn();
	srand(42);
	
	while (1) 
    {				
		

		
		
		
		if(JSTimer == JS_TIME)
		{
			JSTick();
			JSTimer = 0;	
		}
		else
		{
			++JSTimer;
		}
		
		if(blockGenerateTimer == BLOCK_GENERATE_TIME)
		{
			generateBlocksTick();
			blockGenerateTimer = 0;
		}
		else
		{
			++blockGenerateTimer;
		}

		if(blockUpdateTimer > BLOCK_UPDATE_TIME)
		{
			updateBlocksTick();
			blockUpdateTimer = 0;
		}
		else
		{
			++blockUpdateTimer;	
		}
	
		
		if(blockUpspeedTimer == BLOCK_UPSPEED_TIME)
		{
			if(BLOCK_UPDATE_TIME > 1)
			{
				--BLOCK_UPDATE_TIME;
			}

			blockUpspeedTimer = 0;
		}
		else
		{
			++blockUpspeedTimer;
		}

		if(playerScoreTimer == PLAYER_SCORE_UPDATE_TIME)
		{
			
			++playerScore;
			itoa(playerScore, playerScoreStr, 10);
			LCD_DisplayString_M( 13, playerScoreStr );
			playerScoreTimer = 0;
		}
		else
		{
			++playerScoreTimer;
		}
		
						
		updateLCDTick();
						//if (USART_IsSendReady(0))
						//{
							//USART_Send("HELLO", 0);
						//}
		while (!TimerFlag){}
		TimerFlag = 0;
    }
}

