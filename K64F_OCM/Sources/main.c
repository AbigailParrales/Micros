/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "derivative.h" /* include peripheral declarations */

unsigned char cont=0;

#define counts 23824

void FTM_init(){
	FTM0_SC=1<<3;				//System clock enabling pre-escaler=1, fclk = 32768*640
	NVICICER1|=(1<<(42%32));	//NVIC configuration
	NVICISER1|=(1<<(42%32));	//NVIC configuration

	FTM0_C5V=counts;			//23824 are the counts for getting an A note with Fq=440Hz and a period of 23824 ml sec

	/*Configuration of the pin PTA0
	 * Mux=ALT3	
	 * 100LQFP=34
	 */
	//Clock enabling of PTA
	SIM_SCGC5|=1<<9;
	//Mux selection
	PORTA_PCR0|=3<<8;

	/*We use the desired channel, in this case ch5, you just check in the manual if it´s available 
	 * In the microcontroller the port PTA0 its available, and it stands for FTM0_ch5
	 */
	FTM0_C5SC=1<<6+5<<2;			//Interruption enable, output compare toggle
	//CHIE=1
	//MSB=0
	//MSA=1
	//ELSB=0
	//ELSA=1

}

int main(void){

	return 0;
}

void FTM0_C5VHandler (void){
	cont++;
	FTM0_C5SC|=1<<7;	//Turn off the flag by reading this reg
	FTM0_C5V+=counts;
	if(cont==2){
		FTMx_CNT
		cont=0;
		FTM0_C5V=FTM0_C5V-65536;
	}
}
