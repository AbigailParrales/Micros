#include "derivative.h" /* include peripheral declarations */

unsigned char mens0[] = ("AT");
unsigned char mens1[] = ("AT+CIFSR\r\n");
unsigned char mens2[] = ("AT+CIPMUX=1\r\n");
unsigned char mens3[] = ("AT+CIPSERVER=1,80\r\n");

unsigned char mens_ok[] = ("OK\r\n");
unsigned char mens_equipo[] = ("/Equipo5");

unsigned char buffer[160];
unsigned char buffer_tst[160];
unsigned char buffer_tst2[500];

unsigned char temp;
unsigned long bufPos;
unsigned char ok_return;

unsigned char LED;
unsigned char IP_address[30]; //x.y.z.t*EQUIPOnR
unsigned char IP_lenght;

void vUART_init(void){
	//UART init
	SIM_SCGC4=0x00000C00; //hAB CLK uart1
	UART0_BDH=0;
	UART0_BDL=11;
	UART1_BDH=0;
	UART1_BDL=11;

	UART0_C2=12;  // bit 3: HAB Tx, bit 2: HAB Rx
	UART1_C2=12;

	//Pin terminal init
	SIM_SCGC5=0x00002C00; //Hab clk PORTB (PB16 y PB17 son Tx y Rx)

	//UART0 Rx and Tx pinout
	PORTB_PCR16=0x00000300;//Hab clk PB16 Rx
	PORTB_PCR17=0x00000300;//Hab clk PB17 Tx

	//UART1 Rx and Tx pinout
	PORTC_PCR3=0x00000300; //Hab clk PC3 Rx
	PORTC_PCR4=0x00000300; //Hab clk PC4 Tx

	//Led init
	PORTB_PCR21=0x00000100;		//Select GPIO
	PORTB_PCR22=0x00000100;
	PORTE_PCR26=0x00000100;

	GPIOB_PDDR=0x00600000;		//GPIO Output
	GPIOE_PDDR=0x04000000;
}

void vUART_send (unsigned char dato){
	do{}while(!(UART1_S1&0x80));
	UART1_D=dato;
	//UART0_D=dato;
}
void vUART_send_debug (unsigned char dato){
	do{}while(!(UART0_S1&0x80));
	UART0_D=dato;
}

void vUART_send_CIFSR (void){
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
void vUART_send_IP (void)
{
	unsigned char i=0;
	do{
		vUART_send(IP_address[i]);
	}while(IP_address[++i]!=0);
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
		if(buffer[i]=='\n'){		//**Revisar si lo acepta como un solo char
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
	unsigned long j=(bufPos+14);  //revisar si es la posición 2
	do{
		IP_address[i++]=buffer[j++];
	}while (buffer[j]!=13); //revisar si es 13 (enter) o 10 (line feed)
	IP_lenght=i;
}
void completa_mensaje (void){
	unsigned char complemento[]={"/EQUIPO5"};
	unsigned char i=0;
	do{
		IP_address[IP_lenght++]=complemento[i++];
	}while (complemento[i]!=0);
	IP_lenght--; //para que quede apuntando al último caracter y usarlo
	// en el parser
}

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
unsigned char parser_OK(void){	//u8parser2
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

unsigned char parser_IP(void){	//u8parser2
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

int main(void){
	//Initialize terminal UART
	vUART_init();

	/*vUART_send_server();
	if(parser_OK2()==1){}*/

	vUART_send_CIFSR();
	if(parser_CIFSR()==1){
		IP_copy();
		vUART_send_mux();
		if(parser_OK()==1){
			vUART_send_server();
			if(parser_OK()==1){
				do{
					if(parser_IP()==1){
						//vUART_send_IP();
						//LED=u8UART_receive();//**
						switch(LED){
						case 'R':
							GPIOB_PDOR=0x00200000;
							GPIOE_PDOR=0x04000000;
							break;
						case 'G':
							GPIOB_PDOR=0x00600000;
							GPIOE_PDOR=0x00000000;
							break;
						case 'B':
							GPIOB_PDOR=0x00400000;
							GPIOE_PDOR=0x04000000;
							break;
						case 'M':
							GPIOB_PDOR=0x00000000;
							GPIOE_PDOR=0x04000000;
							break;
						case 'C':
							GPIOB_PDOR=0x00400000;
							GPIOE_PDOR=0x00000000;
							break;
						case 'Y':
							GPIOB_PDOR=0x00200000;
							GPIOE_PDOR=0x00000000;
							break;
						case 'W':
							GPIOB_PDOR=0x00000000;
							GPIOE_PDOR=0x00000000;
							break;
						}
					}
				}while(LED!='S');
			}
		}
	}
}
