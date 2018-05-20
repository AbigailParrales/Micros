#include "derivative.h" /* include peripheral declarations */

/*
 * IIC no tiene tiempos mínimos, tiene tiempos máximos
 * */


void cIIC_init(void){
	//clock pin SDA
	//clock pin SCL
	//mux selector SDA as GPIO
	//mux selector SCL as GPIO
	//SDA as output
	//SCL as output
	SCL=0;		//PORTx_PCOR=1<<y		PCOR is Port Clear Output Register
	SDA=0;
}

void vIIC_start(void){
	SDA=1;
	SCL=1;
	//wait time 7 in manual asm("nop")
	SDA=0;
	//wait time 6 in manual asm("nop")
	SCL=0;
}

void vIIC_stop(void){
	SDA=0;	
	SCL=1;
	//wait time 10 in manual asm("nop")
	SDA=1;
	//wait time 12 in manual asm("nop")
	SCL=0;
	SDA=0;
}

void vIIC_send_byte(unsigned char dato){
	unsigned char cont=8;
	do{
		if(dato&0x80)SDA=1;
		else SDA=0;
		SCL=1;
		//esperar tiempo minimo SCL=1 t2, reduciendo el tiempo
		//dado que la sig instrucción consue tiempo
		dato<<=1;
		SCL=0;
		//esperar tiempo minimo SCL=0 t3, reduciendo el tiempo
		//dado que la sig instrucción consue tiempo
	}while(--cont);
	SDA=0;
}

unsigned char vIIC_ack_inp(void){
	//SDA input
	SCL=1;
	if(SDA) temp=1;
	else temp=0;
	SCL=0;
	//SDA output
	return temp;
}

void vIIC_byte_write (unsigned short direccion, unsigned char dato){
	vIIC_start();
	vIIC_send_byte(0xa0);
	if(vIIC_ack_inp()==0){
		vIIC_send_byte(direccion>>8);	//direccion alta
		if(vIIC_ack_inp()==0){
			vIIC_send_byte(direccion);	//direccion alta
			if(vIIC_ack_inp()==0){
				vIIC_send_byte(dato);	//dato
				if(vIIC_ack_inp()==0){
					vIIC_stop();
					error=0;
				}else erro=1;
			}else error=1;
		}else error=1;
	}else error=1;
}

unsigned char vIIC_receive_byte(void){
	unsigned char temp;
	unsigned char cont=8;
	//SDA input
	do{
		SCL=1;
		temp<<=1;
		if(SDA) temp|=1;
		//else temp&=~1;
		//esperar
		SCL=0;
		//esperar
	}while(--cont);
	return temp;
}



int main(void)
{
	int counter = 0;




	for(;;) {	   
		counter++;
	}

	return 0;
}

