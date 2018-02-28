/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */

void LPTimer_IRQHandler(void){
	LPTMR0_CSR=0x80;
	GPIOE_PTOR|=(1<<26);
}

int main(void){
	SIM_SCGC5 = (1<<13)+1;	//clock PORTE and LPTMR0
	
	PORTE_PCR26 = (1<<8);	//Multiplexer for output
	
	GPIOE_PDDR|=(1<<26);
	GPIOE_PDOR|=(1<<26);	//Turn off the green LED
	
	LPTMR0_PSR = 5;			//101 Bypass
	LPTMR0_CMR = 1000;		//Using the 1Kh clock, 0<=range<=65
	LPTMR0_CSR = 1;			//Enable LPTMR0
	
	NVICICER1=58%32;
	NVICISER1=58%32;		//Enable NVIC
	
	while(1){
		
	}
	
	return 0;
}
