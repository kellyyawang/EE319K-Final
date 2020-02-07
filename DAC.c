#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
void DAC_Init(void){
	volatile int delay1;
	SYSCTL_RCGCGPIO_R |= 0x02;          	// activate Port B
  delay1=SYSCTL_RCGC2_R;         	// clock is stabilizing 
																		
  GPIO_PORTB_AMSEL_R &= ~0x0F;     	// disable analog functionality on PB3 to PB0
  GPIO_PORTB_PCTL_R &= ~0x0000FFFF;	// configure PB3 to PB0 as GPIO
  GPIO_PORTB_DIR_R |= 0x0F;        	// make PB3-  to PB0 out
	GPIO_PORTB_DR8R_R |= 0x0F;				// enable 8 mA drive on PB3 to PB0
  GPIO_PORTB_AFSEL_R &= ~0x0F;     	// disable alt funct on PB3 to PB0
  GPIO_PORTB_DEN_R |= 0x0F;        	// make input/output on PB3 to  PB0
}



// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & ~0xF) | (data & 0xF);
}
