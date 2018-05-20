#include "derivative.h" /* include peripheral declarations */

unsigned char mens0[] = ("AT+CWJAP=\"riot-equipo5\",\"ensemestreiaprendoymedivierto\"\r\n");
//unsigned char mens1[] = ("AT+CIPSTA_DEF=\"172.21.0.1\",\"172.21.3.254\",\"255.255.252.0\"");
unsigned char mens1[] = ("AT+CIPSTA_DEF=\"172.21.6.2\",\"172.21.6.254\",\"255.255.255.0\"\r\n");
unsigned char mens2[] = ("AT+CIPMUX=1\r\n");
unsigned char mens3[] = ("AT+CIPSERVER=1,80\r\n");
unsigned char mens4[] = ("AT+CIPSTART=4,\"TCP\",\"172.21.6.1\",80\r\n");
unsigned char mens5[] = ("AT+CIPSEND=4,74\r\n");
unsigned char mens6[] = ("GET /micro.php?param=--MSG-suficiente- HTTP/1.1\r\nHost: localhost\r\n\r\n");
unsigned char mens7[] = ("AT+CIFSR\r\n");
unsigned char mens8[] = ("GET /micro.php?param=--MSGinsuficiente HTTP/1.1\r\nHost: localhost\r\n\r\n");
unsigned char mens9[] = ("AT+CIPSEND=4,74\r\n");
unsigned char mens10[] = ("AT+CIPCLOSE=4\r\n");

unsigned char mens_at[] = ("AT\r\n");
unsigned char mens_ok[] = ("OK\r\n");
unsigned char mens_symbol = '>';
unsigned char mens_linked[] = ("Linked\r\n");
unsigned char mens_equipo[] = ("/Equipo5");
unsigned char mens_closed[] = ("SEND");
unsigned char mens_conf[] = ("We did it!!!\r\n");
unsigned char mens_neg[] = ("We failed!!!\r\n");

unsigned char buffer[160];
unsigned char buffer_tst[160];
unsigned char buffer_tst2[1000];

unsigned char temp;
unsigned long bufPos;
unsigned char ok_return;
unsigned long j_debug;

unsigned char LED;
unsigned char IP_address[30]; //x.y.z.t*EQUIPOnR
unsigned char IP_lenght;

unsigned long temp_handlers;  

//Variables para ascii sending
unsigned long centenas;
unsigned long decenas;
unsigned long unidades;
unsigned long millares;
unsigned long decenasDeMillares;
unsigned long num_sn_unidades;

#define thresh 22500

void init_modules(void){
	//UART init
	SIM_SCGC4=0x00000C00; //hAB CLK uart1
	UART0_BDH=0;
	UART0_BDL=11;
	UART1_BDH=0;
	UART1_BDL=11;

	UART0_C2=12;  // bit 3: HAB Tx, bit 2: HAB Rx
	UART1_C2=12;

	//Pin terminal init
	SIM_SCGC5=0x00002C01; //Hab clk PORTB (PB16 y PB17 son Tx y Rx)

	//UART0 Rx and Tx pinout
	PORTB_PCR16=0x00000300;//Hab clk PB16 Rx
	PORTB_PCR17=0x00000300;//Hab clk PB17 Tx

	//UART1 Rx and Tx pinout
	PORTC_PCR3=0x00000300; //Hab clk PC3 Rx
	PORTC_PCR4=0x00000300; //Hab clk PC4 Tx

	//Led init
	PORTB_PCR2=0x00000100;  //Seleccionar GPIO LED
	GPIOB_PDDR|=(1<<2);

	/*PORTB_PCR2=(1<<8); //mux ->puerto para lampara
			GPIOB_PDDR|=(1<<7);
	 */
	/////////////////////////////////////////////////////////////////
	/////////////////////-----TIMER 0-----///////////////////////////
	/////////////////////////////////////////////////////////////////

	//Configuration
	LPTMR0_PSR=5; //101 Bypass, clock= 1KHz (1ms) (1 = si es bypass, 01 =1khz) -PREESCALER
	LPTMR0_CMR=30000; //usando clock 1khz 0<=rango<= 65seg

	//Page 75 INTERRUPTION
	NVICICER1=(1<<58%32); //1 del reg.1 .. escribir un 1 en esa posición //apagar banderas pendientes
	NVICISER1|=(1<<58%32); //	Hab NVIC //el OR es para modificar solo 1 y dejar las otras como estaban

	/////////////////////////////////////////////////////////////////
	/////////////////////-----ADC 1-----/////////////////////////////
	/////////////////////////////////////////////////////////////////
	//Configuration
	SIM_SCGC3|=(1<<27);  //Clock ADC1

	ADC1_CFG1=0x00000000C;   //Conversion 16 bits
	ADC1_SC1A=18;

	NVICISER2|=(1<<(73%32));	//Enable NVIC for ADC1

}

void vUART_send (unsigned char dato){
	do{}while(!(UART1_S1&0x80));	//Transmit Complete Flag
	UART1_D=dato;
	//UART0_D=dato;
}
void vUART_send_debug (unsigned char dato){
	do{}while(!(UART0_S1&0x80));	//Transmit Complete Flag
	UART0_D=dato;
}



//Send methods for all of the messages
void vUART_send_CWJAP (void){
	unsigned char i=0;
	do{
		vUART_send(mens0[i]);
	}while(mens0[++i]!=0);
}
void vUART_send_CIPSTADEF (void){
	unsigned char i=0;
	do{
		vUART_send(mens1[i]);
	}while(mens1[++i]!=0);
}
void vUART_send_mux (void){
	unsigned char i=0;
	do{
		vUART_send(mens2[i]);
	}while(mens2[++i]!=0);
}
void vUART_send_server (void)
{
	unsigned char i=0;
	do{
		vUART_send(mens3[i]);
	}while(mens3[++i]!=0);
}
void vUART_send_start (void){
	unsigned char i=0;
	do{
		vUART_send(mens4[i]);
	}while(mens4[++i]!=0);
}
void vUART_send_sendDataON (void){
	unsigned char i=0;
	do{
		vUART_send(mens5[i]);
	}while(mens5[++i]!=0);
}
void vUART_send_sendDataOFF (void){
	unsigned char i=0;
	do{
		vUART_send(mens9[i]);
	}while(mens9[++i]!=0);
}
void vUART_send_GET_ON (void){
	unsigned char i=0;
	do{
		vUART_send(mens6[i]);
	}while(mens6[++i]!=0);
}
void vUART_send_GET_OFF (void){
	unsigned char i=0;
	do{
		vUART_send(mens8[i]);
	}while(mens8[++i]!=0);
}
void vUART_send_CIFSR (void){
	unsigned char i=0;
	do{
		vUART_send(mens7[i]);
	}while(mens7[++i]!=0);
}
void vUART_send_CLOSE (void){
	unsigned char i=0;
	do{
		vUART_send(mens10[i]);
	}while(mens10[++i]!=0);
}

void vUART_send_confirmation (void){
	unsigned char i=0;
	do{
		vUART_send(mens_conf[i]);
	}while(mens_conf[++i]!=0);
}

void vUART_send_neg (void){
	unsigned char i=0;
	do{
		vUART_send(mens_neg[i]);
	}while(mens_neg[++i]!=0);
}

void vUART_send_IP (void){
	unsigned char i=0;
	do{
		vUART_send(IP_address[i]);
	}while(IP_address[++i]!=0);
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

unsigned char u8UART_receive (void){
	do{}while(!(UART1_S1&0x20));

	do{}while (!(UART0_S1&0x80));	//Eco to PC
	UART0_D = UART1_D;
	return UART1_D;
}
void vUART_receive_buffer (void){
	unsigned char i=0;
	unsigned long cont;
	do{
		buffer[i++]=u8UART_receive();
	}while((i<160) && (++cont<=10000000));
}

unsigned char get_buffer_pos (void){
	unsigned long i=0;    
	unsigned long cont=0;
	do{
		if(buffer[i]=='\n'){
			cont++;
		}
		i++;
	}while ((i<160) && (cont != 3));
	if(cont==3){
		return i; 
	}
	else return 0;
}

void IP_copy(void){
	unsigned char i=0;
	bufPos=get_buffer_pos();
	unsigned long j=(bufPos+14); 
	do{
		IP_address[i++]=buffer[j++];
	}while (buffer[j]!=13); 
	IP_lenght=i;
}
void completa_mensaje (void){		//not used yet
	unsigned char complemento[]={"/EQUIPO5"};
	unsigned char i=0;
	do{
		IP_address[IP_lenght++]=complemento[i++];
	}while (complemento[i]!=0);
	IP_lenght--; //para que quede apuntando al último caracter y usarlo
	// en el parser
}

/**
 * Waits until an OK comes or a timeout is reached 
 * Returns: 1 if OK arrives or returns 0 if timeout is reached
 */
unsigned char parser_CIFSR(void){	//u8parser2
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned long cont;

	do{	
		cont = 0;
		do{}while ((!(UART1_S1&0x20))&&(++cont<=10000000));

		if(cont!=10000000){

			temp = UART1_D; //u8UART_receive();
			UART0_D=temp;

			/*buffer[j++]=temp;
			//timeout
			if (j==50) 
			{ temp=5;
			buffer[j]=temp;
			}*/

			if (mens_ok[i]==temp){
				i++;
			}
			else{
				i=0;
			}
		}
		else{
		}
	}while((mens_ok[i]!=0) && (++cont <= 10000000));

	if (cont==1000000){
		return 0;
	}
	else{
		return 1;
	}
}

/**
 * Waits until an OK comes or a timeout is reached 
 * Returns: 1 if OK arrives or returns 0 if timeout is reached
 */
unsigned char parser_OK(void){	
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned long cont;

	do{	
		cont = 0;
		do{}while ((!(UART1_S1&0x20))&&(++cont<=10000000));

		if(cont!=10000000){
			temp = UART1_D; //u8UART_receive();
			UART0_D=temp;

			/*buffer_tst[j++]=temp;
			//timeout
			if (j==50) 
			{ temp=5;
			buffer_tst[j]=temp;
			}*/

			if (mens_ok[i]==temp){
				i++;
			}
			else{
				i=0;
			}
		}
	}while((mens_ok[i]!=0) && (++cont <= 10000000));
	if (cont==1000000){
		return 0;
	}
	else{
		return 1;
	}
}

/**
 * Waits until an '>' comes or a timeout is reached 
 * Returns: 1 if '>' arrives or returns 0 if timeout is reached
 */

unsigned char parser_sendData(void){	
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned long cont;

	do{	
		cont = 0;
		do{}while ((!(UART1_S1&0x20))&&(++cont<=10000000));

		if(cont!=10000000){
			temp = UART1_D; //u8UART_receive();
			UART0_D=temp;

			/*buffer_tst[j++]=temp;
			//timeout
			if (j==50) 
			{ temp=5;
			buffer_tst[j]=temp;
			}*/

			if (mens_symbol==temp){
				i=1;
				/*GPIOB_PDDR=0x00600000;		//GPIO Output
				GPIOE_PDDR=0x04000000;*/
			}
			else{
				i=0;
			}
		}
	}while((i==0) && (++cont <= 10000000));
	if (cont==1000000){
		return 0;
	}
	else{
		return 1;
	}
}

/**
 * Waits until an 'Linked' comes or a timeout is reached 
 * Returns: 1 if 'Linked' arrives or returns 0 if timeout is reached
 */
unsigned char parser_linked(void){	
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned long cont;

	do{	
		cont = 0;
		do{}while ((!(UART1_S1&0x20))&&(++cont<=10000000));

		if(cont!=10000000){
			temp = UART1_D; //u8UART_receive();
			UART0_D=temp;

			/*buffer_tst[j++]=temp;
			//timeout
			if (j==50) 
			{ temp=5;
			buffer_tst[j]=temp;
			}*/

			if (mens_linked[i]==temp){
				i++;
			}
			else{
				i=0;
			}
		}
	}while((mens_linked[i]!=0) && (++cont <= 10000000));
	if (cont==1000000){
		return 0;
	}
	else{
		return 1;
	}
}

/**
 * Waits until it finds mens_equipo and receives the next character and stores it in the LED variable
 */
unsigned char parser_IP(void){
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned long cont;

	do{	
		cont = 0;
		do{}while ((!(UART1_S1&0x20))&&(++cont<=10000000));
		if(cont!=10000000){
			temp = UART1_D; //u8UART_receive();
			UART0_D=temp;
			/*buffer_tst2[j++]=temp;
			//timeout
			if (j==50)
			{ temp=5;
			buffer[j]=temp;
			}*/
			if (mens_equipo[i]==temp){
				i++;
			}
			else{
				i=0;
			}
		}
	}while((mens_equipo[i]!=0) && (++cont <= 10000000));
	if (cont==1000000){
		return 0;
	}
	else{
		do{}while ((!(UART1_S1&0x20)));
		LED = UART1_D; //u8UART_receive();
		UART0_D=LED;
		return 1;
	}
}

/**
 * Waits until an 'closed' comes or a timeout is reached 
 * Returns: 1 if 'closed' arrives or returns 0 if timeout is reached
 */
unsigned char parser_closed(void){	
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned long cont;

	do{	
		cont = 0;
		do{}while ((!(UART1_S1&0x20))&&(++cont<=10000000));

		if(cont!=10000000){
			temp = UART1_D; //u8UART_receive();
			UART0_D=temp;

			/*buffer_tst[j++]=temp;
			//timeout
			if (j==50) 
			{ temp=5;
			buffer_tst[j]=temp;
			}*/

			if (mens_closed[i]==temp){
				i++;
			}
			else{
				i=0;
			}
		}
	}while((mens_closed[i]!=0) && (++cont <= 10000000));
	if (cont==1000000){
		return 0;
	}
	else{
		return 1;
	}
}

unsigned char parser_debug(void){	
	unsigned char i = 0;
	j_debug = 0;
	unsigned long cont;

	do{	
		cont = 0;
		do{}while ((!(UART1_S1&0x20))&&(++cont<=10000000));

		if(cont!=10000000){
			temp = UART1_D; //u8UART_receive();
			UART0_D=temp;

			buffer_tst2[j_debug++]=temp;
			//timeout
			/*if (j_debug==350) { 
				temp=5;
				buffer_tst2[j_debug]=temp;
			}
			 */
			if (mens_closed[i]==temp){
				i++;
			}
			else{
				i=0;
			}
		}
	}while((mens_closed[i]!=0) && (++cont <= 10000000));
	if (cont==10000000){
		return 0;
	}
	else{
		return 1;
	}
}

int main(void){
	//Initialise 
	init_modules();

	vUART_send('\r');
	vUART_send('\n');
	vUART_send_CIPSTADEF();
	if(parser_OK()==1){
		vUART_send_CWJAP();
		if(parser_OK()==1){
			vUART_send_mux();
			if(parser_OK()==1){
				vUART_send_server();
				if(parser_OK()==1){
					//LPTMR enable (interruption and module enabling)
					LPTMR0_CSR=(1<<6)+1; //Hab LPTMR y Hab local de interrupcion -INTERRUPCION
					while(1){}
				}
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////
/////////////////////-----INTERRUPTIONS-----/////////////////////
/////////////////////////////////////////////////////////////////
void LPTimer_IRQHandler(void){
	LPTMR0_CSR|=(1<<7); //Apagar bandera TIMER
	ADC1_SC1A=(1<<6)+18;
}
void ADC1_IRQHandler(void){

	ADC1_SC1A=(1<<7)+0x1F;	//Turn off COCO	and stop conversion
	temp_handlers = ADC1_RA;			//RA is the data outpuT
	vUART_send_adcReceive();

	if (temp_handlers < thresh){
		GPIOB_PDDR=0x00000000;//APAGAT	
		//GPIOB_PDOR=(1<<21); //LED MICRO
	}else
		GPIOB_PDDR=(1<<2); //PRENDET FOCO	
	//Sequence for server connection
	vUART_send_start();
	if (parser_OK()==1){
		if (temp_handlers <= thresh){
			vUART_send_sendDataON();
			if(parser_sendData()==1){
				//GPIOB_PDOR=(1<<2);
				vUART_send_GET_ON();
				if(parser_debug()==1){
					vUART_send_confirmation();
				}
				else{
					vUART_send_neg();
				}
			}
		}
		else{
			vUART_send_sendDataOFF();
			if(parser_sendData()==1){
				//GPIOB_PDOR=(0<<2);//APAGAT
				vUART_send_GET_OFF();
				if(parser_debug()==1){
					vUART_send_confirmation();

				}
				else{
					vUART_send_neg();
				}
			}
		}
	}
}
