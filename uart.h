
#ifndef UART_H
#define UART_H
/*--------------------------------- INCLUDES---------------------------------*/
#include "std_types.h"
#include "tm4c123gh6pm_registers.h"

/*--------------------- Definitions and configurations ----------------------*/
#define UART0_RCGC_BIT                  (0)
#define GPIO_PORTA_RCGC_BIT             (0)

/*-------------------------- FUNCTION PROTOTYPES----------------------------*/
void uart0_init(uint32 clk, uint32 baudrate);
void uart0_putchar(uint8 c);
void print(const uint8 *str);
uint32 print_idec(sint32 n);
uint32 print_ihex(uint32 n);
uint32 print_ibin(uint32 n);

#endif 
