#include "derivative.h" /* include peripheral declarations */

unsigned long temp_handlers;

unsigned char FTM0_CH5;
unsigned char FTM0_CH6;
unsigned char FTM0_CH7;

unsigned char FTM0_CH5_cnt=0;
unsigned char FTM0_CH6_cnt=0;
unsigned char FTM0_CH7_cnt=0;

//Variables para ascii sending
unsigned long centenas;
unsigned long decenas;
unsigned long unidades;
unsigned long millares;
unsigned long decenasDeMillares;
unsigned long num_sn_unidades;

#define counts_0 23824
#define counts_1 23824
#define counts_2 23824

#define thresh_0 22500

/*
 * 1)El sistema es el control y monitoreo de una casa inteligente. El sistema incluye varios nodos, 
cada uno con la capacidad de comunicar por wi-fi.

2) Debe de leer una entrada análogica para monitorear la luz ambiental.

	monitor_naturalLight();

	Pin para entrada de luz ambiental --> ADC1_SE18 //No init required//
	
	Monitoreo por LPTMR cada 30seg

3) Debe tener una salida digital para activar una luz en el cuarto. La señal 
para la luz debe de ser proporcional a luz ambiental. Si no hay luz natural, 
debe de activar durante 1 ms la salida, si la luz natural está al 1 %, debe de 
generar un pulso de 10 us y después desactivar por 990 us.

	Salida a luz de cuarto --> PTB2
	
	Si no hay luz natural:
		thresh_0		

		FTM0_CH5	PTA0 1ms	counts_0
		
	Si la luz está al 1%
		thresh_1
		FTM0_CH6	PTA1 10us	counts_1
		FTM0_CH7	PTA2 990us 	counts_2
	
4) Debe tener entradas digitales para poder revisar si las ventanas están a
biertas o cerradas. Lo mismo para la puerta principal.

	Para ventana	-->	PTB3

	Para puerta	-->	PTB10

5) Debe tener entrada analógica para monitorear lluvia

6) Si hay lluvia, debe de generar una salida digital hasta que un microswitch 
indique que se cerró la ventana.

7) En cualquier evento importante, se debe de mandar un reporte por la nube a 
un nodo en internet.

8) Se debe de poder controlar de manera manual-remota, las salidas: prender foco, cerrar ventana, 
[abrir puerta principal] 
 */

void modules_init(void){
	//Ports
	SIM_SCGC5=0x00002E01; 	//Hab clk PORTB (PB16 y PB17 son Tx y Rx)

	PORTB_PCR2=0x00000100;  //Seleccionar GPIO para Salida a luz de cuarto
	PORTB_PCR3=0x00000100;  //Seleccionar GPIO para Salida a luz de cuarto
	PORTB_PCR10=0x00000100;  //Seleccionar GPIO para Salida a luz de cuarto
	GPIOB_PDDR|=(1<<2+0<<3+0<<10);		//Configure PTB2 as output

	SIM_SCGC3|=(1<<27);  	//Clock ADC1

	PORTA_PCR0|=3<<8;		//Output compare

	//ADCs
	ADC1_CFG1=0x00000000C;   //Conversion 16 bits
	ADC1_SC1A=18;

	NVICISER2|=(1<<(73%32));	//Enable NVIC for ADC1

	ADC0_CFG1=0x00000000C;   //Conversion 16 bits
	ADC0_SC1A=18;

	NVICISER2|=(1<<(39%32));	//Enable NVIC for ADC0

	//LPTMR0
	LPTMR0_PSR=5; //101 Bypass, clock= 1KHz (1ms) (1 = si es bypass, 01 =1khz) -PREESCALER
	LPTMR0_CMR=30000; //usando clock 1khz 0<=rango<= 65seg

	NVICICER1=(1<<58%32); //1 del reg.1 .. escribir un 1 en esa posición //apagar banderas pendientes
	NVICISER1|=(1<<58%32); //	Hab NVIC //el OR es para modificar solo 1 y dejar las otras como estaban

	//OCM
	FTM0_SC=1<<3;				//System clock enabling pre-escaler=1, fclk = 32768*640
	NVICICER1|=(1<<(42%32));	//NVIC configuration
	NVICISER1|=(1<<(42%32));	//NVIC configuration

	FTM0_C5V=counts_0;			//23824 are the counts for getting an A note with Fq=440Hz and a period of 23824 ml sec
	FTM0_C6V=counts_1;			//23824 are the counts for getting an A note with Fq=440Hz and a period of 23824 ml sec
	FTM0_C7V=counts_2;			//23824 are the counts for getting an A note with Fq=440Hz and a period of 23824 ml sec

	//ICM

	//UART
	//UART init
		SIM_SCGC4=0x00000C00; //hAB CLK uart1
		UART0_BDH=0;
		UART0_BDL=11;
		UART1_BDH=0;
		UART1_BDL=11;

		UART0_C2=12;  // bit 3: HAB Tx, bit 2: HAB Rx
		UART1_C2=12;

	

		//UART0 Rx and Tx pinout
		PORTB_PCR16=0x00000300;//Hab clk PB16 Rx
		PORTB_PCR17=0x00000300;//Hab clk PB17 Tx

		//UART1 Rx and Tx pinout
		PORTC_PCR3=0x00000300; //Hab clk PC3 Rx
		PORTC_PCR4=0x00000300; //Hab clk PC4 Tx

}

void vUART_send (unsigned char dato){
	do{}while(!(UART1_S1&0x80));	//Transmit Complete Flag
	UART1_D=dato;
	//UART0_D=dato;
}

void vUART_send_adcReceive (void){
	//Prepararse para enviar Ascii
	decenasDeMillares=(temp_handlers/10000);
	millares=((temp_handlers-(decenasDeMillares*10000))/1000);
	centenas=((temp_handlers-((decenasDeMillares*10000)+(millares*1000)))/100);
	decenas=((temp_handlers-((decenasDeMillares*10000)+(millares*1000)+(centenas*100)))/10);
	num_sn_unidades=(decenasDeMillares*10000)+(millares*1000)+(centenas*100)+(decenas*10);
	unidades=temp_handlers-num_sn_unidades;

	vUART_send(decenasDeMillares+48);
	vUART_send(millares+48);
	vUART_send(centenas+48);
	vUART_send(decenas+48);
	vUART_send(unidades+48);

	vUART_send('\r');
	vUART_send('\n');
}


void monitor_naturalLight(void){

}


int main(void)
{
	modules_init();
	
	for(;;) {	   
	}

	return 0;
}

void LPTimer_IRQHandler(void){
	LPTMR0_CSR|=(1<<7); 			//Apagar bandera TIMER
	ADC1_SC1A=(1<<6)+18;			//ADC1 interrupt enable
}

void ADC1_IRQHandler(void){

	ADC1_SC1A=(1<<7)+0x1F;	//Turn off COCO	and stop conversion
	temp_handlers = ADC1_RA;			//RA is the data outpuT
	//vUART_send_adcReceive()

	if (temp_handlers < thresh){
		FTM0_CH5=1;
		FTM0_CH6=0;
		FTM0_CH7=0;

		FTM0_C5SC=1<<6+5<<2;			//FTM0 CH5 interrupt enable
	}
	else{
		FTM0_CH5=0;
		FTM0_CH6=1;
		FTM0_CH7=0;
		FTM0_C5SC=1<<6+5<<2;			//FTM0 CH6 interrupt enable
	}

}

void FTM0_IRQHandler(void){
	if(FTM0_CH5==1){
		FTM0_C5SC|=1<<7;	//Turn off the flag by reading this reg
		switch (FTM0_CH5_cnt) {
		case 0:
			GPIOB_PD0R=0x00000000;	//Turn on the lap
			FTM0_CH5_cnt=1;
			break;
		case 1:
			GPIOB_PDDR=(1<<2); 		//Turn off the lap
			FTM0_CH5_cnt=0;
			break;
		default:
			break;
		}
	}
	else if(FTM0_CH6==1){
		FTM0_C6SC|=1<<7;	//Turn off the flag by reading this reg
		switch (FTM0_CH6_cnt) {
		case 0:
			GPIOB_PD0R=0x00000000;	//Turn on the lap
			FTM0_CH6_cnt=1;
			break;
		case 1:
			GPIOB_PDDR=(1<<2); 		//Turn off the lap
			FTM0_CH6_cnt=0;

			FTM0_CH5=0;
			FTM0_CH6=0;
			FTM0_CH7=1;
			FTM0_C5SC=1<<6+5<<2;			//FTM0 CH7 interrupt enable
			break;
		default:
			break;
		}
	}
	else if(FTM0_CH7==1){
		FTM0_C7SC|=1<<7;	//Turn off the flag by reading this reg
		GPIOB_PDDR=(1<<2); 	//Turn off the lap
	}
}

