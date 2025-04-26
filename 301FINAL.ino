// Names: Kimberly Agraan, Adrian Delibassis, Aidan Madonna, Joe Wu
// Project: CPE 301 Final Project

#include <LiquidCrystal.h>

#define RDA 0x80
#define TBE 0x20

#define START_BUTTON 5
#define STOP_BUTTON 6
#define RESET_BUTTON 7
#define STEP_BUTTON 4

#define DISABLED 1
#define IDLE 2
#define RUNNING 3
#define ERROR 4

// UART Pointers
volatile unsigned char* myUCSR0A = (unsigned char*) 0x00C0;
volatile unsigned char* myUCSR0B = (unsigned char*) 0x00C1;
volatile unsigned char* myUCSR0C = (unsigned char*) 0x00C2;
volatile unsigned int* myUBRR0 = (unsigned int*) 0x00C4;
volatile unsigned char* myUDR0 = (unsigned char*) 0x00C6;

// ADC Pointers
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

// Port B Register Pointers
volatile unsigned char* port_b = (unsigned char*) 0x25; 
volatile unsigned char* ddr_b = (unsigned char*) 0x24; 
volatile unsigned char* pin_b = (unsigned char*) 0x23; 

void setup()
{
  // set PB4, PB5, PB6, PB7 to INPUT
  *port_b &= ~(1 << START_BUTTON);
  *port_b &= ~(1 << STOP_BUTTON);
  *port_b &= ~(1 << RESET_BUTTON);
  *port_b &= ~(1 << STEP_BUTTON);
  // enable the pull-ups on buttons
  *ddr_b |= (1 << START_BUTTON);
  *ddr_b |= (1 << STOP_BUTTON);
  *ddr_b |= (1 << RESET_BUTTON);
  *ddr_b |= (1 << STEP_BUTTON);
  // initialize UART
  U0Init(9600);
  // initialize ADC
  adc_init();
}

void loop()
{
  int status = DISABLED;

  // start button
  if(*pin_b & (1 << START_BUTTON) && status == DISABLED)
  {
    status = IDLE;
  }
  // stop button
  if(*pin_b & (1 << STOP_BUTTON)) 
  {
    status = DISABLED;
  }
  // reset button
  if(*pin_b & (1 << RESET_BUTTON) && status == ERROR)
  {
    status = IDLE;
  }

  switch(status)
  {
    // disabled = yellow LED
    case(DISABLED):
      // TURN LED ON
      break;
    // idle = green LED
    case(IDLE):
      // TURN LED ON
      if(checkWaterStatus(status))
      {

      }
      break;
    // running = blue LED
    case(RUNNING):
      // TURN LED ON
      if(checkWaterStatus(status))
      {

      }
      break;
    // error = red LED
    case(ERROR):
      // TURN LED ON
      char message[] = "Water level is too low";
      for(int i = 0; message[i] != '\0'; i++)
        U0putchar(message[i]);
      U0putchar('\n');
      break;
  }
}

bool checkWaterStatus(int& status)
{
  int waterValue = adc_read(0);
  if(waterValue > 220)  // threshold subject to change
  {
    return true;
  }
  else if(waterValue <= 220)  // threshold subject to change
  {
    status = ERROR;
    return false;
  }
}

// UART functions
void U0Init(int U0baud)
{
  unsigned long FCPU = 16000000;
  unsigned int tbaud;
  tbaud = (FCPU / 16 / U0baud - 1);
  *myUCSR0A = 0x20;
  *myUCSR0B = 0x18;
  *myUCSR0C = 0x06;
  *myUBRR0 = tbaud;
}
unsigned char kbhit()
{
  return *myUCSR0A & RDA;
}
unsigned char getChar()
{
  return *myUDR0;
}
void putChar(unsigned char U0pdata)
{
  while(!(*myUCSR0A & TBE));
  *myUDR0 = U0pdata;
}

// ADC functions
void adc_init() 
{
  // setup the A register
  *my_ADCSRA |= 0b10000000;
  *my_ADCSRA &= 0b10111111;
  *my_ADCSRA &= 0b11011111;
  *my_ADCSRA &= 0b11111000;
  // setup the B register
  *my_ADCSRB &= 0b11110111;
  *my_ADCSRB &= 0b11111000;
  // setup the MUX Register
  *my_ADMUX &= 0b01111111;
  *my_ADMUX |= 0b01000000;
  *my_ADMUX &= 0b11011111;
  *my_ADMUX &= 0b11110000;
}
unsigned int adc_read(unsigned char adc_channel_num)
{
  *my_ADMUX &= 0b11110000;
  *my_ADCSRB &= 0b11110111;
  *my_ADMUX += 0;
  *my_ADCSRA |= 0b01000000;
  while(*my_ADCSRA & 0x40);
  unsigned int val = (*my_ADC_DATA & 0x03FF);
  return val;
}
