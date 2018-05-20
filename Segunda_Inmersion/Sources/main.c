/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

#define thresh 32500

unsigned long temp;  

//Variables para ascii sending
unsigned long centenas;
unsigned long decenas;
unsigned long unidades;
unsigned long millares;
unsigned long decenasDeMillares;
unsigned long num_sn_unidades;

void vUART_init(void){
	SIM_SCGC4=1<<10; //clk uart0
	UART0_BDH=0;
	UART0_BDL=0x86;

	UART0_C2=12;  // bit 3: HAB Tx, bit 2: HAB Rx

	//UART0 Rx and Tx pinout
	PORTB_PCR16=0x00000300;//Hab clk PB16 Rx
	PORTB_PCR17=0x00000300;//Hab clk PB17 Tx

}

void vUART_send (unsigned long dato){
	do{}while(!(UART0_S1&0x80));	//Transmit Complete Flag
	UART0_D=dato;
}
/////////////////////////////////////////////////////////////////

int main(void)
{
	//	vUART_init();

	SIM_SCGC5=(1<<10)+1; //reloj PORTC y LPTMR0

	PORTB_PCR21=0x00000100;  //Seleccionar GPIO LED
	GPIOB_PDDR|=(1<<21);

	/*PORTB_PCR2=(1<<8); //mux ->puerto para lampara
	GPIOB_PDDR|=(1<<7);
	 */
	/////////////////////////////////////////////////////////////////
	/////////////////////-----TIMER 0-----///////////////////////////
	/////////////////////////////////////////////////////////////////


	LPTMR0_PSR=5; //101 Bypass, clock= 1KHz (1ms) (1 = si es bypass, 01 =1khz) -PREESCALER
	LPTMR0_CSR=(1<<6)+1; //Hab LPTMR y Hab local de interrupcion -INTERRUPCION

	LPTMR0_CMR=10; //para pruebas rapidas
	//LPTMR0_CMR=1000; //usando clock 1khz 0<=rango<= 65seg

	/////////////////////////////////////////////////////////////////
	/////////////////////-----INTERRUP-----//////////////////////////
	////////////////////////////////////////////////////////////////
	//config. del nvic pág 75 INTERRUPCION
	NVICICPR1=(1<<58%32); //1 del reg.1 .. escribir un 1 en esa posición //apagar banderas pendientes
	NVICISER1|=(1<<58%32); //	Hab NVIC //el OR es para modificar solo 1 y dejar las otras como estaban


	///////////////////////////////////////////////////////////////
	////////////////////----ADCH 1----////////////////////////////
	/////////////////////////////////////////////////////////////
	SIM_SCGC3|=(1<<27);  //Clock ADC1
	//Entrada SE18 es analog input por default

	ADC1_CFG1=0x00000000C;   //Conv 16 bits
	//ADC1_SC3=1<<3;  // conversiones continuas
	ADC1_SC1A=18;

	vUART_init();
	while(1)
	{

	}
	return 0;
}

void sendNumber (void){
	//Prepararse para enviar Ascii
	decenasDeMillares=(temp/10000);
	millares=((temp-(decenasDeMillares*10000))/1000);
	centenas=((temp-((decenasDeMillares*10000)+(millares*1000)))/100);
	decenas=((temp-((decenasDeMillares*10000)+(millares*1000)+(centenas*100)))/10);
	num_sn_unidades=(decenasDeMillares*10000)+(millares*1000)+(centenas*100)+(decenas*10);
	unidades=temp-num_sn_unidades;

	vUART_send(decenasDeMillares+48);
	vUART_send(millares+48);
	vUART_send(centenas+48);
	vUART_send(decenas+48);
	vUART_send(unidades+48);

	vUART_send('\r');
	vUART_send('\n');

}

void LPTimer_IRQHandler(void){

	LPTMR0_CSR=0x80; //Apagar bandera TIMER

	do{}while((ADC1_SC1A&0x80)==1);  //Esperar a COCO
	temp=ADC1_RA;

	sendNumber();

	if (temp > thresh){
		//	GPIOB_PDOR|=(1<<2); //PRENDET
		GPIOB_PDOR|=(1<<21);
	}else
		GPIOB_PDOR|=0x00000000;//APAGAT
}
