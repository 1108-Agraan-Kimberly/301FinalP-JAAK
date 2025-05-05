// Names: Kimberly Agraan, Adrian Delibassis, Aidan Madonna, Joe Wu
// Project: CPE 301 Final Project

#include <LiquidCrystal.h>
#include <RTClib.h>

#define RDA 0x80
#define TBE 0x20

#define START_BUTTON 7
#define STOP_BUTTON 6
#define RESET_BUTTON 5
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

// global variables
int status = DISABLED;
RTC_DS3231 rtc;
unsigned long previousMillis = 0; 
const long interval = 3000;  


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
  //starts rtc 
  rtc.begin();
  // initialize ADC
  adc_init();
}

void loop()
{
  unsigned long currentMillis = millis();

  //updates the time every 3 seconds
  if(currentMillis - previousMillis >= interval){
  previousMillis = currentMillis;
  DateTime now = rtc.now(); 
  printTime(now.hour(), now.minute(), now.second()); //shows time
  printNumber(status);  //needs to print status
  putChar('\n');
  }

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
        // Check temps + other stuff
      }
      break;
    // running = blue LED
    case(RUNNING):
      // TURN LED ON
      if(checkWaterStatus(status))
      {
        // Check temps + other stuff
      }
      break;
    // error = red LED
    case(ERROR):
      // TURN LED ON
      char message[] = "Water level is too low";
      for(int i = 0; message[i] != '\0'; i++)
        putChar(message[i]);
      putChar('\n');
      break;
  }
}

//prints string
void U0printString(const char* str) {
  while (*str) {
    putChar(*str++);
  }
}

//neeed to change registers but its the formatting of how it should be printed
void printTime(uint8_t h, uint8_t m, uint8_t s) {
  char buffer[16];
  sprintf(buffer, "%02d:%02d:%02d\r\n", h, m, s);
  U0printString(buffer);
}

// Check water value and return status
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

//
void printNumber(int number) {
  // Convert the number to a string manually
  char buffer[10];         // Enough to hold a 32-bit integer
  int i = 0;

  if (number == 0) {
    putChar('0');
    return;
  }

  if (number < 0) {
    putChar('-');
    number = -number;
  }

  // Extract digits (in reverse order)
  while (number > 0) {
    buffer[i++] = '0' + (number % 10);
    number /= 10;
  }

  // Print digits in correct order
  for (int j = i - 1; j >= 0; j--) {
    putChar(buffer[j]);
  }
}