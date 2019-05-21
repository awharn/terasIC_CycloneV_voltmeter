#include "address_map_arm.h"
extern int flag;
extern int * TMR_FLAG_ptr;
/****************************************************************************************
 * Clock - Interrupt Service Routine                                
 *                                                                          
 * Sets a flag when the timer triggers
 ***************************************************************************************/
void clock_ISR( void )
{
	*TMR_FLAG_ptr = 0x1; //Clear Timer0 flag
	flag = 1;
	return;
}
