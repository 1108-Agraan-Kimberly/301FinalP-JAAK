//Kimberly Agraan, Adrian Delibassis, Adian Modanna, Joe Wu

 #define RDA 0x80
 #define TBE 0x20  
 volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
 volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
 volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
 volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
 volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

void setup() {
  // put your setup code here, to run once:
U0init(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

}


//USART0 functions for serial functions
void U0init(unsigned long U0baud)
{//sets registers
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}

unsigned char U0kbhit()
{ //if can be read 1 = cant be read, 0 = no data to be read
  if(*myUCSR0A & RDA)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

unsigned char U0getchar()
{ //reads input from serial monitor
  unsigned char ch;
  while(!(*myUCSR0A & (1 << 7)));
  ch = *myUDR0;
  return ch;
}

void U0putchar(unsigned char input_char)
{//wrties the char
  while(!(*myUCSR0A & (1 << 5)));
  *myUDR0 = input_char;
}
