/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

unsigned char mens1[] = {"Hola mundo"};
unsigned char buffer[80];
unsigned char cmpmens1[] = {"READY"};

void vUART_init(void){
	//UART init
	SIM_SCGC4 = 0x00000400;	//Hab clk UART0
	UART0_BDH = 0;
	UART0_BDL = 0x89;	//137 <- 136.53 para 9600 bps, clk = 640*32768
	//UART_C1 = 0;
	UART0_C2 = 12;	//bit 3: Hab TX, bit 2: Hab Rx

	//Pin terminal init
	SIM_SCGC5 = 0x00000400;	//Hab clk PORT (PTB16 y 17 son Rx y Tx)
	PORTB_PCR16 = 0x00000300;	//PTB16 Rx
	PORTB_PCR17 = 0x00000300;	//PTB17	Tx
}

void vUART_send(unsigned char dato){
	do{}while(!(UART0_S1 & 0x80));
	//UART0_D = 0x41;	//'A'
	UART0_D = dato;
}

void vUART_send_msg (void){
	unsigned char i = 0;
	do{
		vUART_send(mens1[i]);
	}
	while(mens1[++i] != 0);
}

unsigned char u8UART_receive (void){
	do{}while(!(UART0_S1 & 0x20));
	return UART0_D;
}

void UART_receive_buffer (void){
	unsigned char i = 0;
	unsigned long cont = 0;

	do{
		buffer[i++] = u8UART_receive();
	}
	while((i < 80) && (++cont <= 10000000));
}

unsigned char u8parser1(void){	//"READY"		Falta terminar el if de al final
	unsigned char i = 0;
	unsigned long cont;

	do{
		cont = 0;
		do{}
		while((!(UART0_S1 & 0x40)) && (++cont <= 10000000));
		if(cont!=10000000){
			if(cmpmens1[i] == UART0_D)i++;
			else i = 0;
			return 1; 	//*
		}
		else{
			return 0;
		}
	}while(cmpmens1[i] != 0);
	return 0;	//*
}

int main(void){
	vUART_init();

	while (1){
		vUART_send('A');
	}

	return 0;
}
