/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

int main(void){
	unsigned char temp;
	//configurar NVIC
			//NVICICPR
			//NVICISER
		
		//inicializar ADC
		SIM_SCGC3|= 1<<27;		//Clk ADC1
		//Entrada SE18 es analog input por default
		//ADC1_CFG1 = 0;		//Set mode to 8 bits
		ADC1_SC3=1<<3;			//Conversion Cont
		
		ADC1_SC1A=18;			//Inicia conversion
	
	for(;;) {
		do{}while((ADC1_SC1A&80)==0);
		temp = ADC1_RA;
	}
	
	return 0;
}
