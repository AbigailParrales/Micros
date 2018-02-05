/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "derivative.h" /* include peripheral declarations */

void vUART_init(void)
{
//UART init    
    SIM_SCGC4=0x00000C00; //Hab clk UART0 y UART1
    UART0_BDH=0;
    UART0_BDL=11;
    UART1_BDH=0;
    UART1_BDL=11;
    //UART0_C1=0;
    UART0_C2=12; // bit 3: Hab Tx, bit 2: Hab Rx
    UART1_C2=12;
    
//Pin/terminal init
    SIM_SCGC5=0x00000C00; //Hab clk PORTB (PB16 y 17 son Rx y Tx) y PORTC
    PORTB_PCR16=0x00000300; //Hab clk PB16 Rx
    PORTB_PCR17=0x00000300; //Hab clk PB17 Tx
    PORTC_PCR3=0x00000300; //Hab clk PC3 Rx
    PORTC_PCR4=0x00000300; //Hab clk PC4 Tx
}

int main(void)
{
    vUART_init();
    
    while (1)
    {
        if (UART0_S1&0x40)
        {
            do{}while (!(UART1_S1&0x80));
            UART1_D=UART0_D;
        }
        if (UART1_S1&0x40)
        {
            do{}while (!(UART0_S1&0x80));
            UART0_D=UART1_D;
        }
        
            }//while (1)
    return 0;
} //main
