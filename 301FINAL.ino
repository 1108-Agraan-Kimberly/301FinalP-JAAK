// Names: Kimberly Agraan, Adrian Delibassis, Aidan Madonna, Joe Wu
// Project: CPE 301 Final Project

#include <LiquidCrystal.h>
#include <RTClib.h>
#include <dht.h>
#include <Stepper.h>

// UART Pointers
#define RDA 0x80
#define TBE 0x20
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

// Port A Register Pointers
volatile unsigned char* port_a = (unsigned char*) 0x22;
volatile unsigned char* ddr_a = (unsigned char*) 0x21;
volatile unsigned char* pin_a = (unsigned char*) 0x20;

// Port B Register Pointers
volatile unsigned char* port_b = (unsigned char*) 0x25; 
volatile unsigned char* ddr_b = (unsigned char*) 0x24; 
volatile unsigned char* pin_b = (unsigned char*) 0x23;

// Port C Register Pointers
volatile unsigned char* port_c = (unsigned char*) 0x28;
volatile unsigned char* ddr_c = (unsigned char*) 0x27;
volatile unsigned char* pin_c = (unsigned char*) 0x26;

// Port D Register Pointers
volatile unsigned char* port_d = (unsigned char*) 0x2B;
volatile unsigned char* ddr_d = (unsigned char*) 0x2A;
volatile unsigned char* pin_d = (unsigned char*) 0x29;

// Port E Register Pointers
volatile unsigned char* port_e = (unsigned char*) 0x2E;
volatile unsigned char* ddr_e = (unsigned char*) 0x2D;
volatile unsigned char* pin_e = (unsigned char*) 0x2C;

// Port F Register Pointers
volatile unsigned char* port_f = (unsigned char*) 0x31;
volatile unsigned char* ddr_f = (unsigned char*) 0x30;
volatile unsigned char* pin_f = (unsigned char*) 0x2F;

// Port H Register Pointers
volatile unsigned char* port_h = (unsigned char*) 0x102; 
volatile unsigned char* ddr_h = (unsigned char*) 0x101; 
volatile unsigned char* pin_h = (unsigned char*) 0x100;

// Timer Pointers
volatile unsigned char* myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char* myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char* myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char* myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned char* myTIFR1 = (unsigned char *) 0x36;
volatile unsigned int* myTCNT1 = (unsigned int *) 0x84;

// Fan Pin
#define FAN_PIN 7
// DHT Pin
dht DHT;
#define DHT11_PIN 38
// Button Pins
#define START_BUTTON 3
#define STOP_BUTTON 5 
#define RESET_BUTTON 6
#define STEP_BUTTON 4
// LED Pins
#define YELLOW_LED 3
#define GREEN_LED 0
#define BLUE_LED 2
#define RED_LED 1

// LCD Pins
LiquidCrystal lcd(11, 12, 2, 3, 4, 5);
// Stepper Motor Pins
Stepper myStepper(60, 40, 42, 41, 43);

// Water Sensor 
int waterSensorPin = A0;
#define WATER_THRESHOLD 300
#define TEMP_THRESHOLD 27

enum State {DISABLED, IDLE, RUNNING, ERROR, LEFT, RIGHT};

// Global Variables
State currState = DISABLED;
State prevState = DISABLED;
State stepState = DISABLED;
bool hasEnabled = false;
RTC_DS3231 rtc;

unsigned long previousMillis = 0;
const long interval = 6000;
unsigned long time = 0;

void setup()
{
  // Set Buttons to INPUT
  *ddr_d &= ~(0x01 << START_BUTTON);
  *ddr_h &= ~(0x01 << STOP_BUTTON);
  *ddr_h &= ~(0x01 << RESET_BUTTON);
  *ddr_b &= ~(0x01 << STEP_BUTTON);
  // Enable Pull-Up Resistors
  *port_d |= (0x01 << START_BUTTON);
  *port_h |= (0x01 << STOP_BUTTON);
  *port_h |= (0x01 << RESET_BUTTON);
  *port_b |= (0x01 << STEP_BUTTON);
  // Set LEDs to OUTPUT
  *ddr_b |= (0x01 << YELLOW_LED);
  *ddr_b |= (0x01 << GREEN_LED);
  *ddr_b |= (0x01 << BLUE_LED);
  *ddr_b |= (0x01 << RED_LED);
  // Setup Water Sensor Ports
  *ddr_f &= ~(0x01 << 0);
  // Setup Fan Ports
  *ddr_c |= (0x01 << FAN_PIN);
  *port_c &= ~(0x01 << FAN_PIN); 

  // Initialize UART
  U0Init(9600);
  // Initialize rtc
  setup_timer_regs();
  rtc.begin();
  // DateTime now = DateTime(2025, 5, 9, 15, 49, 0);
  // rtc.adjust(now);
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  // Setup Stepper
  myStepper.setSpeed(60);
  // Initialize ADC
  adc_init();

  // Setup Interupt
  attachInterrupt(digitalPinToInterrupt(18), startSystem, HIGH);
}

void loop()
{
  // Measure values
  unsigned int waterValue = adc_read(waterSensorPin);
  int chk = DHT.read11(DHT11_PIN);
  int temperature = DHT.temperature;
  int humidity = DHT.humidity;

  // stop button
  if(!(*pin_h & (0x01 << STOP_BUTTON)) )
  {
    hasEnabled = false;
  }

  switch(currState)
  {
    // disabled = yellow LED
    case DISABLED:
      disabled();
      break;
    // idle = green LED    
    case IDLE:
      idle(waterValue, temperature);
      break;
    // running = blue LED
    case RUNNING:
      running(waterValue, temperature);
      break;
    // error = red LED
    case ERROR:
      error(waterValue);
      break;
  }
  
  // Disaplay to LCD
  if(currState != DISABLED)
  {
    displayDHTData();
    stepperMotor();
  }
  printTime();

  time = millis();
  while(millis() < time + 500);
}

// Disabled state
void disabled()
{
  // Turn yellow LED on
  *port_b |= (0x01 << YELLOW_LED);
  *port_b &= ~(0x01 << GREEN_LED);
  *port_b &= ~(0x01 << BLUE_LED);
  *port_b &= ~(0x01 << RED_LED);
  // Turn fan off
  *port_c &= ~(0x01 << FAN_PIN);
  lcd.clear();
  if(prevState != DISABLED)
  {
    statusSwapped();
    prevState = DISABLED;
  }
  if(hasEnabled)
  {
    currState = IDLE;
    prevState = DISABLED;
  }
}

// Idled state
void idle(int w, int t)
{
  // Turn green LED on
  *port_b &= ~(0x01 << YELLOW_LED);
  *port_b |= (0x01 << GREEN_LED);
  *port_b &= ~(0x01 << BLUE_LED);
  *port_b &= ~(0x01 << RED_LED);
  // Turn fan off
  *port_c &= ~(0x01 << FAN_PIN); 

  if(prevState != IDLE)
  {
    statusSwapped();    
    prevState = IDLE;
  }
  if(!hasEnabled)
  {
    currState = DISABLED;
    prevState = IDLE;
  }
  else if(w < WATER_THRESHOLD)
  {
    currState = ERROR;
    prevState = IDLE;
  }
  else if(t >= TEMP_THRESHOLD)
  {
    currState = RUNNING;
    prevState = IDLE;
  }
}

void running(int w, int t)
{
  // Turn blue LED on
  *port_b &= ~(0x01 << YELLOW_LED);
  *port_b &= ~(0x01 << GREEN_LED);
  *port_b |= (0x01 << BLUE_LED);
  *port_b &= ~(0x01 << RED_LED);
  // Turn fan on
  *port_c |= (0x01 << FAN_PIN);

  if(prevState != RUNNING)
  {
   statusSwapped();
   prevState = RUNNING;
  }
  if(!hasEnabled)
  {
    currState = DISABLED;
    prevState = RUNNING;
  }
  else if(w < WATER_THRESHOLD)
  {
    currState = ERROR;
    prevState = RUNNING;
  }
  else if(t < TEMP_THRESHOLD)
  {
    currState = IDLE;
    prevState = RUNNING;
  }
}
void error(int w)
{
  // Turn red LED on
  *port_b &= ~(0x01 << YELLOW_LED);
  *port_b &= ~(0x01 << GREEN_LED);
  *port_b &= ~(0x01 << BLUE_LED);
  *port_b |= (0x01 << RED_LED);
  // Turn fan off
  *port_c &= ~(0x01 << FAN_PIN);

  time = millis();
  while(millis() < time + 500); 

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water level is");
  lcd.setCursor(0, 1);
  lcd.print("too low");

  if(prevState != RUNNING)
  {
    statusSwapped();
    prevState = RUNNING;
  }
  if(!hasEnabled)
  {
    currState = DISABLED;
    prevState = ERROR;
  }
  else if(!(*pin_h & (0x01 << RESET_BUTTON)) && w >= WATER_THRESHOLD)
  {
    currState = IDLE;
    prevState = ERROR;
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

// Timer Setup
void setup_timer_regs()
{
  // setup the timer control registers  
  *myTCCR1A = 0x00;
  *myTCCR1B = 0X00;
  *myTCCR1C = 0x00;
  // reset TOV flag and enable TOV interrupt
  *myTIFR1 |= 0x01;
  *myTIMSK1 |= 0x01;  
}

// Interrupt Function
void startSystem()
{
  if(*pin_d & (0x01 << START_BUTTON))
  {
    hasEnabled = true;
  }
}

// Temp and humidity for LCD screen
void displayDHTData() 
{
  time = millis();
  while(millis() < time + 500); 
  int chk = DHT.read11(DHT11_PIN);

  lcd.setCursor(0, 0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223); // Degree symbol
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");
}

// Print time to Serial Monitor
void printTime()
{
  unsigned long currentMillis = millis();

  //updates the time every 3 seconds
  if(currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    DateTime now = rtc.now(); 
    
    char buffer[16];
    sprintf(buffer, "%02d:%02d:%02d\r\n", now.hour(), now.minute(), now.second());
    U0printString(buffer); 
    putChar('\n');
  }
}

//prints string
void U0printString(const char* str) 
{
  while (*str) 
  {
    putChar(*str++);
  }
}

void stepperMotor()
{
  if(!(*pin_b & (0x01 << STEP_BUTTON)))
  {
    if(stepState == DISABLED)
    {
      stepState = LEFT;
      myStepper.setSpeed(25);
      myStepper.step(100);
      
    }    
    else if(stepState == LEFT)
    {
      stepState = RIGHT;
      myStepper.setSpeed(50);
      myStepper.step(-100);
      
    }    
    else
    {
      stepState = DISABLED;
      myStepper.setSpeed(0);
      
    }
  }
}

void statusSwapped()
{
  char message[] = "Status changed";
  for(int i = 0; message[i] != '\0'; i++)
  {
    putChar(message[i]);
  }
  putChar('\n');
}
