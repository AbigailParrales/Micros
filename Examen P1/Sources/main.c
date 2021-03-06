/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */

unsigned char enter[] = ("\r\n");
unsigned char buffer[5];
unsigned char f_operand;
unsigned char s_operand;
unsigned char operation;
unsigned char resultado[2];

void vUART_init(void){
	SIM_SCGC4=0x00000400; //hAB CLK UART0
	UART0_BDH=0;
	UART0_BDL=0x89;
	UART0_C2=12;  // bit 3: HAB Tx, bit 2: HAB Rx

	//Pin terminal init
	SIM_SCGC5=0x00000400; //Hab clk PORTB (PB16 y PB17 son Tx y Rx)

	//UART0 Rx and Tx pinout
	PORTB_PCR16=0x00000300;//Hab clk PB16 Rx
	PORTB_PCR17=0x00000300;//Hab clk PB17 Tx
}

void vUART_send (unsigned char dato){
	do{}while(!(UART0_S1&0x80));
	UART0_D=dato;
	//UART0_D=dato;
}

unsigned char parser_enter(void){	//u8parser2
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned char temp;
	unsigned long cont;

	do{	
		cont = 0;
		do{}while ((!(UART0_S1&0x20))&&(++cont<=10000000));

		if(cont!=10000000){
			temp = UART0_D; //u8UART_receive();

			buffer[j++]=temp;

			if (enter[i]==temp){
				i++;
			}
			else{
				i=0;
			}
		}
	}while((enter!=0) && (++cont <= 10000000));
	if (cont==1000000){
		return 0;
	}
	else{
		return 1;
	}
}

void separate_buffer (void){
	f_operand = buffer[0]+(buffer[1]*10);
	s_operand = buffer[3]+(buffer[4]*10);
	operation = buffer[2];
}


int main(void){
	unsigned char result_tmp;
	unsigned char tmp;

	//Initialize terminal UART
	vUART_init();
	if(parser_enter()==1){
		switch(operation){
		case '/':		//Division
			result_tmp= f_operand / s_operand;
			break;
		case '+':		//Addition
			result_tmp = f_operand + s_operand;
			break;
		case '-':		//Substraction
			result_tmp = f_operand - s_operand;
			break;
		case '*':		//Multiplication
			result_tmp = f_operand * s_operand;
			break;
		}
		tmp=(result_tmp/10)*10;		//decenas
		resultado[0]=tmp;
		resultado[1]=result_tmp-tmp;
		vUART_send(resultado[1]);
		vUART_send(resultado[0]);
	}

	return 0;
}
