/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */
void delay (void);


int main(void){
	SIM_SCGC5=0x00002400;		//Enable clock in page 323
	
	PORTB_PCR21=0x00000100;		//Select GPIO
	PORTB_PCR22=0x00000100;
	PORTE_PCR26=0x00000100;
	
	GPIOB_PDDR=0x00600000;		//GPIO Output
	GPIOE_PDDR=0x04000000;
	
	for(;;) {
		GPIOB_PDOR=0x00400000;	//Turn on the pin, turn off the red-LED
		delay();
		GPIOB_PDOR=0x00000000;	//Turn off the pin, turn on the red-LED
		delay();
	}
	
	return 0;
}
void delay(void){
	unsigned long i;
	for(i=0;i<10000000;i++);
}
