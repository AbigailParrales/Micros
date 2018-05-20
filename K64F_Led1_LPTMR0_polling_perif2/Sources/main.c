/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

unsigned char mensajes[40][40]={"hola","adios","hellooooow","bye"};
#define m_hola 0
unsigned char *ptr_msg;

void LPTimer_IRQHandler(void){
	LPTMR0_CSR=0x80;		//Turn off the flag
	ADC1_SC1A=(1<<6)+18;
	//GPIOE_PTOR|=(1<<26);	//Toggle green LED
}

/*
 * void ADC1_IRQHandler(void){
 * 	ADC1_SC1A=(1<<7)+0x1F;	//Turn off COCO	and stop conversion
 * 	temp = ADC1_RA;			//RA is the data output
 * 	if(temp<luz_baja) GPIOE_PDOR=(1<<relay);
}*/



void UART1_Status_IRQHandler(){

	/*
	 * void vUART_send (unsigned char dato){
	 * do{}while(!(UART1_S1&0x80));	//Transmit Complete Flag
	 * UART1_D=dato;
	 * }*/

	if(UART1_S1&(1<<7)){	//There´s space for a new character, see if the TCIE is off
		UART1_D=*ptr_msg++;
		if(*ptr_msg==0){
			UART1_C2&=~(1<<7);	//Disable interruption
		}
	}
}

void UART1_send_msg(unsigned char *msg_id){		//msg_id es el número de mensaje que queremos mandar
	ptr_msg=&mensajes[msg_id,0];
	UART1_D=*ptr_msg++;

	//Enable interruption for UART1_Tx
	NVICICER1=(1<<(34%32));	//Configure interruption		We choose the 34 since we are using status handler for UART1
	NVICISER1|=(1<<(34%32));	//Enable NVIC interruption
}


int main(void){
	SIM_SCGC5 = (1<<13)+1;	//clock PORTE and LPTMR0
	SIM_SCGC3|= 1<<27;		//clock ADC1
	//clk UART0 & UART1

	PORTE_PCR26 = (1<<8);	//Multiplexer for output
	PORTE_PCR18 = (0<<8);	//Multiplexer for output
	//MUX UART0 & UART1
	//baud_rate: UART0 and UART1
	//config UART0 and UART1: hab tx and rx, hab interrupt tx and rx

	GPIOE_PDDR|=(1<<26);
	GPIOE_PDOR|=(1<<26);	//Turn off the green LED

	LPTMR0_PSR = 5;			//101 Bypass
	LPTMR0_CMR = 10;		//Using the 1Kh clock, 0<=range<=65
	LPTMR0_CSR = 1;			//Enable LPTMR0 and interruption local manager

	//ADC Configuration
	ADC1_CFG1=0x0C;

	//NVIC configuration
	NVICICER1=(1<<(58%32));		//Configure NVIC for LPTMR0
	NVICISER1|=(1<<(58%32));	//Enable NVIC
	NVICICER2|=(1<<(73%32));

	//NVIC for UART0 and UART1

	//UART1_send_msg("AT+CIFSR");
	//UART1_send_msg("AT+....");

	UART1_send_msg(m_hola);

	while(1){		//This infinite loop will convert to a function that makes the micro enter to a low consume mode

	}

	return 0;
}
