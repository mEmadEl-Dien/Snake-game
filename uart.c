
/*-------------------------------- INCLUDES -------------------------------*/
#include "uart.h"
/*-------------------------------- UTILITY FUNCTIONS -------------------------------*/
/*
  Discription: a utility function that reverses an array of characters
                
  arguments:
  [in] uint8* str:      a pointer to the character array that will be reversed
  [in] uint32 length:   the length of that array
*/
static void reverse(uint8 *str, uint32 length)
{
    uint16 start = 0;
    uint16 end = length -1;
    uint8 temp;
    while (start < end)
    {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
 
/*
  Discription: a utility function that converts a signed integer to a character array specified
               by base for printing
                
  arguments:
  [in] sint32 num:      the input number 
  [in] uint8* str:      a pointer to the character array that will store the converted number
  [in] uint8 base:      base that will be used for conversion

  Return:               a uint32 that represents the number of digits of the converted number
*/
static uint32 itoa(sint32 num, uint8* str, uint8 base)
{
    /*an index to iterate over array*/
    uint32 i = 0;
    /*used to indicate that the number is negative*/
    uint8 isNegative = FALSE;
    /*remainder used to parse integer*/
    uint32 rem = 0;
    /* if input is 0 print 0*/
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
 
    // if number is negative set isNegative to true and make the number positive again
    if (num < 0 && base == 10)
    {
        isNegative = TRUE;
        num = -num;
    }
 
    while (num != 0)
    {
        /*get current last digit in number*/
        rem = num % base;
        /*put it in index i*/
        /*if that number is greater than 9 this means we are working with base larger than 10*/
        /*in that case start using letters as in hexa*/
        /*otherwise convert the current number to ascii and store it in array*/
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        /*remove the last digit through integer division*/
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
    /*i is now the length of the array character*/
    return i;
}  
 
/*-------------------------- FUNCTION DEFINITIONS --------------------------*/

/*
  Discription: this function should initialize the uart0 module of tiva c according to
  the input clk and baudrate
  arguments:
  [in] uint32 clk:      the cpu clock in hz
  [in] uint32 baudrate: the baudrate that the uart should operate at
*/
void uart0_init(uint32 clk, uint32 baudrate){
  /*enable the clock to UART0*/
  SYSCTL_RCGCUART_R |= (1 << UART0_RCGC_BIT);
  /*enable clock for PORTA*/
  SYSCTL_RCGCGPIO_R |= (1 << GPIO_PORTA_RCGC_BIT);
  /*digital enable for PA0 PA1*/
  GPIO_PORTA_DEN_R |= (1 << 0) | (1 << 1);
  /*setting the GPIO AFSEL for porta*/
  GPIO_PORTA_AFSEL_R |= (1 << 0) | (1 << 1);
  GPIO_PORTA_PCTL_R  |= (1 <<0) | (1 << 4);
  /*Disable the UART*/
  UART0_CTL_R &= ~(1 << 0);
  /*BRD = BRDI + BRDF = UARTSysClk / (16 * Baud Rate)*/
   float32 BRD = clk / (16.0 * baudrate);
   uint16 BRDF = (int)((BRD - (int)BRD) * 64 + 0.5);
  /*write integer portion to UARTIBRD*/
  UART0_IBRD_R = (int) BRD; /*value should be 104 for 9600 baud*/
  /*write the fraction portion to UARTFBRD*/
  UART0_FBRD_R = BRDF; /*value should be 11*/
  /*configure the serial parameters in UARTLCRH*/
  /*
      word length = 8bit
      no parity
      1 stop bit
  */
  UART0_LCRH_R |= (1 << 6) | (1 << 5);
  UART0_LCRH_R &= ~(1 << 7) & ~(1 << 4) & ~(1<< 3);
  /*configure the uart clock source to the system clock*/
  UART0_CC_R = 0;
  /*enable UART*/
  UART0_CTL_R |= (1 << 0) | (1 << 8) | (1<<9);
	/*enable the intterupt for UART0*/
	NVIC_PRI1_R = (6 << 13) | (NVIC_PRI1_R & 0xFFFF1FFF);
	NVIC_EN0_R |= (1 << 5);
	UART0_ICR_R &= ~(1 << 4);
	UART0_IM_R |= (1 << 4);
}

/*
  Discription: sends a character(byte) through uart
  arguments:
  [in] uint8 ch:      the character to be sent by uart
*/
void uart0_putchar(uint8 ch){
  /*wait until the transmitter has no data to transmit*/
  while(!(UART0_FR_R & (1 << 7)));
  /*insert data into the data register*/
  UART0_DR_R = (UART0_DR_R & 0xFFFFFF00) | (ch & 0x000000FF);
}

/*
  Discription: sends a string through uart
  arguments:
  [in] const uint8 *str: a pointer to the string array you want to send through uart
*/
void print(const uint8 *str){
  while(*str != '\0'){
    if(*str == '\n'){
      uart0_putchar('\r');
    }
    uart0_putchar(*str);
    str++;
  }
}


/*
  Discription: sends an integer through uart and prints its sign as well
  arguments:
  [in] sint32 n: the signed integer you want to send through uart
  Return:        a uint32 that represents the number of digits printed
*/
uint32 print_idec(sint32 n){
  char buff[12];
  uint32 len = itoa(n , buff , 10);
  print(buff);
  return len;
}
/*
  Discription: sends an integer through uart and prints in hexadecimal format
  arguments:
  [in] uint32 n: the signed integer you want to send through uart
  Return:        a uint32 that represents the number of digits printed
*/
uint32 print_ihex(uint32 n){
  char buff[12];
  uint32 len = itoa(n , buff , 16);
  print("0x");
  for(uint8 i = 0 ; i < (8 - len) ; i++){
    uart0_putchar('0');
  }
  print(buff);
  return len;
}
/*
  Discription: sends an integer through uart and prints in binary format
  arguments:
  [in] uint32 n: the signed integer you want to send through uart
  Return:        a uint32 that represents the number of digits printed
*/
uint32 print_ibin(uint32 n){
  char buff[34];
  uint32 len = itoa(n , buff , 2);
  print("0b");
    for(uint8 i = 0 ; i < (32 - len) ; i++){
    uart0_putchar('0');
  }
  print(buff);
  return len;
}

