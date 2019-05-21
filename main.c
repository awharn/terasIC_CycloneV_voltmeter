#include "address_map_arm.h"

void config_clock(void);
int HEX_Lookup(char);
void disable_A9_interrupts (void);
void set_A9_IRQ_stack (void);
void config_GIC (void);
void config_clock (void);
void enable_A9_interrupts (void);

volatile int * TMR_LOAD_ptr = (int *) 0xFFFEC600;
volatile int * TMR_CRNT_ptr = (int *) 0xFFFEC604;
volatile int * TMR_CTRL_ptr = (int *) 0xFFFEC608;
volatile int * TMR_FLAG_ptr = (int *) 0xFFFEC60C;
volatile int * HEX3_HEX0_ptr = (int *) HEX3_HEX0_BASE;
volatile int * ADC_CH0_ptr = (int *) 0xFF204000;
volatile int * ADC_CH1_ptr = (int *) 0xFF204004;
int flag = 0;

int main(void)
{	
	int temp = 0;
	int count = 0;
	int sample = 0;
	int value = 0;
	int display = 0;
	int divisor = 0;
	int index;
	char hex[4] = {'X'};
	
	disable_A9_interrupts ();	// disable interrupts in the A9 processor
	set_A9_IRQ_stack ();			// initialize the stack pointer for IRQ mode
	config_GIC ();					// configure the general interrupt controller
	config_clock ();			// configure the timer
	enable_A9_interrupts ();	// enable interrupts in the A9 processor

	*HEX3_HEX0_ptr = 0x00; //Clear LEDs
	*ADC_CH1_ptr = 1; //Set to auto update ADC
	*TMR_CTRL_ptr = *TMR_CTRL_ptr | 0x1; //enable timer
	
	while (1)
	{
		if (flag == 1)
		{
			flag = 0;
			sample = sample + (*ADC_CH0_ptr & 0xFFF);
			count = count + 1;
		}
	
		if (count == 1000)
		{
			count = 0; //Clear count
			value = (sample / 1000); //Average the 1000 counts
			for (index = 0; index < 4; index++)
			{
				switch (index)
				{
					case 0:
						divisor = 1000;
						break;
					case 1:
						divisor = 100;
						break;
					case 2:
						divisor = 10;
						break;
					case 3:
						divisor = 1;
						break;
				}
				temp = (value / divisor);
				value = (value % divisor);
				if (flag != 2 && temp == 0 && index < 3)
				{
					hex[index] = 0x00;
				}
				else
				{
					hex[index] = HEX_Lookup(temp + 48);
					flag = 2;
				}
			}
			
			display = 0x0000000;
			for (index = 0; index < 4; index++)
			{
				display = (display << 8);
				display = display | hex[index];
			}
			*HEX3_HEX0_ptr = display;
			sample = 0;
			flag = 0;
		}
	}
}

int HEX_Lookup(char c)  //Lookup switch statement
{ 
	switch(c)
	{
		case '0':
			return 0x3F;
		case '1':
			return 0x06;
		case '2':
			return 0x5B;
		case '3':
			return 0x4F;
		case '4':
			return 0x66;
		case '5':
			return 0x6D;
		case '6':
			return 0x7D;
		case '7':
			return 0x07;
		case '8':
			return 0x7F;
		case '9':
			return 0x6F;
		case 'X':
			return 0x00;
	}
	return 0x00;	
}

void config_clock() //Configure clock to interrupt every millisecond
{
	*TMR_LOAD_ptr = 200000;
	*TMR_CRNT_ptr = 200000;
	*TMR_CTRL_ptr = 0x6; //Set timer to not be enabled, load values, auto-reload, allow interrupt
	*TMR_FLAG_ptr = 0x1; //Clear Timer0 flag
}