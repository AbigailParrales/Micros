#include "derivative.h" /* include peripheral declarations */

void UART_init(void){
	//UART init
		SIM_SCGC4=0x00000400; //hAB CLK uart0
		UART0_BDH=0;
		UART0_BDL=0x86;

		UART0_C2=12;  // bit 3: HAB Tx, bit 2: HAB Rx

		//Pin terminal init
		SIM_SCGC5=0x00002C01; //Hab clk PORTB (PB16 y PB17 son Tx y Rx)

		//UART0 Rx and Tx pinout
		PORTB_PCR16=0x00000300;//Hab clk PB16 Rx
		PORTB_PCR17=0x00000300;//Hab clk PB17 Tx

		//Page 75 INTERRUPTION
		NVICICER0=(1<<31%32); //1 del reg.1 .. escribir un 1 en esa posición //apagar banderas pendientes
		NVICISER0|=(1<<31%32); //	Hab NVIC //el OR es para modificar solo 1 y dejar las otras como estaban
}


int main(void)
{
	int counter = 0;
	
	
	
	
	for(;;) {	   
	   	counter++;
	}
	
	return 0;
}

void UART0_Status_IRQHandler(){
	
}
