// Segunda Inmersion

#include <derivative.h> /* include peripheral declarations /*/

#define ON 30000 //parametro para encender
unsigned char status_act;
unsigned char status_prev;
unsigned char status_post;

unsigned char buffer[300];

unsigned char LED;

unsigned char automatico = '1';
unsigned char isOn;

unsigned long adc_value;
unsigned long captura[5];
unsigned long km = 0;
unsigned long cont;

unsigned char cmpmens3[] = { "Equipo2" };
unsigned char cmpmens4[] = { ">" };
unsigned char cmpmens1[] = { "OK\r\n" };

unsigned char mens[10][70] = {
		{ "AT+CWMODE=1\r\n" },
		{ "AT+CWJAP=\"riot-equipo2\",\"ensemestreiaprendoymedivierto\"\r\n" },
		{ "AT+CIPSTA_DEF=\"172.18.0.5\",\"172.18.3.254\",\"255.255.252.0\"\r\n" },
		{ "AT+CIFSR\r\n" },
		{ "AT+CIPMUX=1\r\n" },
		{ "AT+CIPSERVER=1,80\r\n" },
		{ "AT+CIPSTART=4,\"TCP\",\"172.18.0.15\",80\r\n" },
		{ "AT+CIPSEND=4,64\r\n" }
};

unsigned char mensStatus[2][70] = {
		{ "GET /status.php?cmd=LED-R1X50ANDOF HTTP/1.1\r\nHost: localhost\r\n\r\n" },
		{ "GET /status.php?cmd=LED-R1X50ANDON HTTP/1.1\r\nHost: localhost\r\n\r\n" } 
};

unsigned char close[5][16] = { 
		{ "AT+CIPCLOSE=0\r\n" }, 
		{ "AT+CIPCLOSE=1\r\n" },
		{ "AT+CIPCLOSE=2\r\n" }, 
		{ "AT+CIPCLOSE=3\r\n" },
		{ "AT+CIPCLOSE=4\r\n" } 
};

unsigned char cmpmens[5][10] = {
		{ "ready\r\n" },
		{ "OK\r\n" },
		{ "Equipo2" },
		{ ">" },
		{ "4,CLOSED" }
};

void vUART_init(void) {
	SIM_SCGC4 = 0x00000C00; 	//Hab clk UART0 y UART1
	UART0_BDH = 0;
	UART0_BDL = 11;   		//115200 bps
	UART1_BDH = 0;
	UART1_BDL = 11;    		//115200 bps

	UART0_C2 = 12; 			// bit 3: Hab Tx, bit 2: Hab Rx
	UART1_C2 = 12;

	//Pines
	SIM_SCGC5 = 0x00000C00; 	//Hab clk PORTB (PB16 y 17 son Rx y Tx) y PORTC
	PORTB_PCR16 =0x00000300; //Hab clk PB16 Rx
	PORTB_PCR17 =0x00000300; //Hab clk PB17 Tx
	PORTC_PCR3 =0x00000300; 	//Hab clk PC3 Rx
	PORTC_PCR4 =0x00000300; 	//Hab clk PC4 Tx

	// LED
	SIM_SCGC5 |= 0x00002400;			//HABILITAR RELOJ
	PORTB_PCR21 =0X00000100;		//Seleccionar pin como GPIO
	PORTB_PCR22 =0X00000100;		//Seleccionar pin como GPIO
	PORTE_PCR26 =0X00000100;		//Seleccionar pin como GPIO

	GPIOB_PDDR |= (1 << 21);

	PORTC_PCR2 =(1<<8);
	GPIOC_PDDR = (1 << 2);

	//Lampara
	PORTB_PCR2=0x00000100;  //Seleccionar GPIO L�mpara
	GPIOB_PDDR|=(1<<2);

	//ADC1 init
	SIM_SCGC3 |= (1 << 27);  //Clock ADC1
	ADC1_CFG1 = 0xC;   //Conv 16 bits
	ADC1_SC1A =18;

}

void vADC_LPTMR_init(void) {
	SIM_SCGC5 = (1 << 11) + (1 << 10) + 1; //reloj PORTC y LPTMR0

	NVICICER1 =(1<<58%32); //
	NVICISER1 |=(1<<58%32); //	
	NVICISER2 |=(1<<(73%32));	//adc1

	LPTMR0_PSR = 5; //PREESCALER
	LPTMR0_CMR = 30000; //5 seg
	LPTMR0_CSR = (1 << 6) + 1; //LPTMR, interrupcion
}

void vUART_send(unsigned char dato)				// Metodo para enviar datos
{
	do {
	} while (!(UART1_S1 & UART_S1_TDRE_MASK));
	UART1_D = dato;
}

void vUART_echo(unsigned char dato)				// Metodo para enviar datos
{
	do {
	} while (!(UART1_S1 & UART_S1_TDRE_MASK));
	UART0_D = dato;								//Imprime dato en pantalla
}


unsigned char u8UART_receive(void)			// Metodo para leer datos recibidos
{
	do {
	} while (!(UART1_S1 & UART_S1_RDRF_MASK));
	unsigned char receiveValue = UART1_D; // Lee contenido de UART1_D, lo almacena en receiveValue y lo limpia
	UART0_D = receiveValue;
	return receiveValue;
}

void vUART_receive_buffer(void)		// Buffer para guardar carcteres entrantes
{
	unsigned char i = 0;
	unsigned long cont = 0;
	do {
		buffer[i] = u8UART_receive();
	} while ((i < 80) && (++cont <= 10000000));
}

void vUART_send_msg(unsigned char mens[])		// Envia mensaje al ESP8266
{
	unsigned char i = 0;
	do {
		vUART_send(mens[i]);
	} while (mens[++i] != 0);
}

unsigned char u8parser(unsigned char cmpmens[])  // Parser busca la palabra "OK"
{
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned char temp;

	do {
		cont = 0;

		do {
		} while ((!(UART1_S1 & UART_S1_RDRF_MASK)) && (++cont <= 1000000));

		if (cont <= 1000000) {
			temp = UART1_D;
			UART0_D = temp;
			buffer[j++] = temp;

			if (cmpmens[i] == temp)
				i++;
			else
				i = 0;
		}

	} while ((cmpmens[i] != 0));

	if (cont >= 1000000) {
		return 0;
	} else

		return 1;
}

void vUART_send_ADC_value(void){
	//Prepararse para enviar Ascii
	captura[5]=(adc_value/10000);
	captura[4]=((adc_value-(captura[5]*10000))/1000);
	captura[3]=((adc_value-((captura[5]*10000)+(captura[4]*1000)))/100);
	captura[2]=((adc_value-((captura[5]*10000)+(captura[4]*1000)+(captura[3]*100)))/10);
	captura[0]=(captura[5]*10000)+(captura[4]*1000)+(captura[3]*100)+(captura[2]*10);
	captura[1]=adc_value-captura[0];

	unsigned char i=5;
	do {
		vUART_echo(captura[i]+48);
		i--;
	}
	while (i!=0);

	vUART_echo('\r');
	vUART_echo('\n');
}

void vUART_connect(void){

	// LED Rojo: Indica que proceso de conexiones ha empezado
	GPIOB_PDDR = 0x00400000;
	GPIOE_PDDR = 0x00000000;

	vUART_send_msg(mens[0]);									// Configura CWMODE
	if (u8parser(cmpmens[1]) == 1) {							// Espera respuesta OK	
		vUART_send_msg(mens[1]);								// Configura red WiFi
		if (u8parser(cmpmens[1]) == 1) {	  					// Espera respuesta OK
			vUART_send_msg(mens[2]);							// Configura IP estatica
			if (u8parser(cmpmens[1]) == 1) {					// Espera respuesta OK
				vUART_send_msg(mens[3]);						// Envia mensaje CIFSR para verificar IP
				if (u8parser(cmpmens[1]) == 1) {				// Espera respuesta OK
					vUART_send_msg(mens[4]);					// Configura modo CIPMUX
					if (u8parser(cmpmens[1]) == 1) {			// Espera respuesta OK
						vUART_send_msg(mens[5]);				// Levanta el servidor
						if (u8parser(cmpmens[1]) == 1) {		// Espera OK
							vUART_send_msg(mens[6]);			// Comienza conexion TCP
							if (u8parser(cmpmens[1]) == 1) {	// Espera OK

								// LED Verde: Indica que proceso de conexiones ha terminado
								GPIOB_PDDR=0x00000000;
								GPIOE_PDDR=0x04000000;


							}}}}}}}}

void vUART_update_process_autom(void){
	vUART_send_msg(mens[7]); 								// Envia comando CIPSEND


	//Abrir conexi�n en caso de cerrada
	if (u8parser(cmpmens[4]) == 1){									// Espera OK
		vUART_send_msg(mens[6]);									// Comienza conexion TCP
		if (u8parser(cmpmens[1]) == 1){}							// Espera OK
	}

	// Comando de ON/OFF
	if (LED != 'S' && LED != 'M') {	
		if (u8parser(cmpmens[1]) == 1) { 					// Espera OK
			if (u8parser(cmpmens[3]) == 1) {				// Espera >

				switch (automatico) {
				case '1':
					if (adc_value < ON)
						vUART_send_msg(mensStatus[1]); 		// Envia query ON
					else if (adc_value > ON)
						vUART_send_msg(mensStatus[0]); 		// Envia query OFF
					break;
				case '0':
					if (isOn == 'Y')
						vUART_send_msg(mensStatus[1]); 		// Envia query ON
					else if (isOn == 'N')
						vUART_send_msg(mensStatus[0]); 		// Envia query OFF
					break;
				default:
					if (adc_value < ON)
						vUART_send_msg(mensStatus[1]); 		// Envia query ON
					else if (adc_value > ON)
						vUART_send_msg(mensStatus[0]); 		// Envia query OFF
					break;
				}

			}
		}
	}
}

void receive_LED (void){
	//Recibe Equipo2
	if (u8parser(cmpmens[2]) == 1) {
		LED = u8UART_receive();

		switch (LED) {
		case 'R':	//Prender Lamp
			status_post = '1';
			break;
		case 'A':	//Apagar Lamp
			status_post = '0';
			break;
		case 'M':	//Modo manual
			automatico = '0';
			break;
		case 'S':	//Modo automatico
			automatico = '1';
			break;

		default:
			break;
		}
	}
}


int main(void) {
	vUART_init();										// Inicializa los Pins
	vUART_connect();									// Realiza conexiones iniciales
	vADC_LPTMR_init();

	automatico='1';
	LED='A';
	isOn='N';
	status_act='0';
	status_prev='0';
	adc_value=30001;

	LPTMR0_CSR = (1 << 6) + 1; //LPTMR, interrupcion 

	while(1){
		receive_LED();

		if(status_act != status_post){
			if(automatico == '0'){			//Manual
				if(status_post == '0'){			//Off
					GPIOB_PDoR=(1<<2);
					GPIOB_PDOR=(1<<21);				
					GPIOC_PDOR=(1<<2);
				}
				else if(status_post == '1'){	//On
					//GPIOB_PDDR=0x00000000;
					GPIOB_PDOR=0x00000000;			
					GPIOC_PDOR=0x0;
				}
			}
			else if(automatico == '1'){		//Automatico
				vUART_send_msg(mens[7]); 							//
				if (u8parser(cmpmens[1]) == 1) { 					// Espera OK
					if (u8parser(cmpmens[3]) == 1) {				// Espera >
						if (adc_value < ON){			//ON
							//GPIOB_PDDR=0x00000000;
							GPIOB_PDOR=0x00000000;			
							GPIOC_PDOR=0x0;
							vUART_send_msg(mensStatus[1]); 		// Envia query ON
						}
						else if (adc_value > ON){		//OFF
							GPIOB_PDOR=(1<<2);
							GPIOB_PDOR=(1<<21);				
							GPIOC_PDOR=(1<<2);
							vUART_send_msg(mensStatus[0]); 		// Envia query OFF
						}
					}
				}
			}
		}
		status_act=status_post;
	}
	return 0;
}
void LPTimer_IRQHandler(void) {
	LPTMR0_CSR |= (1 << 7); //Apagar bandera TIMER
	ADC1_SC1A =(1<<6)+18;
}

void ADC1_IRQHandler(void) {
	ADC1_SC1A =(1<<7)+0x1F;	//coco
	adc_value = ADC1_RA;
}

