# 301FinalP-JAAK
Group Members: Kimberly Agraan, Adrian Delibassis, Aidan Modonna, Joe Wu

## Demonstrationm Video
``

https://drive.google.com/drive/folders/1xf9lX4sYT3UEeZzeGde0xebWIoNEsbTk?usp=sharing

``

# Overview:
## Description 
``

Our project will simulate the functions of a swamp cooler. It will cool dry air by blowing it through water-saturated material, increasing humidity and decreasing temperature. The components that will be used include sensors to measure the temperature and humidity. A water level sensor will be used for detecting the water reservoir to check if it is wet or not. The temperature and humidity will be displayed on an LCD, it will update and show the changes. A fan 3-6v Motor will be used to help cool down the water while the stepper motor will simulate a vent that will change airflow/water flow. 

 ``

## Program
``

The program supports the functionalities of how the system will behave. There are four different states the system will be in. The first state it starts in is disabled. This is where the system is off. The LCD won’t display anything and the fan is also off. The motor will not be able to adjust while it is disabled. To signify that it is in the disabled state a yellow LED will be turned on. Then pressing one of the buttons will change its state. 
	
 The next state would be idle. The idle state will keep track of the temperature, humidity, water level, and the time. The water level is going to be continuously monitored and the stage will be changed if there’s an error in the water being too low in level. The LED at this state should be green signifying it is on.
	
 Another state of the circuit is the error state. The motor shuts off and everything stops and stays in place. There needs to be a reset button to change to the idle state if the water is then above the threshold. Error message is displayed on the LCD and the light will be RED showing everything is in this stalled state.
	
 The last state is the running state. The fan motor is on and will run intil the temperature drops below the threshold. If the water becomes too low it will transition to the error state. The blue LED should be turned on when in this state.
 
``

## Components

Each of the components is connected to different pins to be able to read with the Arduino. Each different component has a different purpose to be on the board.
The least intuitive part is the fan motor, negative side to ground and the positive side to pin 30. This fan is to push air and keep cool.
The real-time clock has two output ports, SDA and SCL, connected to 20 and 21, respectively. The real time clock is to keep track of the time and output on the serial monitor when it changes states.
The humidity sensor is connected to pin 38. This sensor reads temperature and humidity, these are shown on the LCD screen. The temperature is also used to determine what state the circuit will go into either on or fan to cool.
The stepper motor control has 1N1-7, but it's only necessary to use 1N1-4, 1 to 40, 2 to 41, 3 to 42, and 4 to 43. The stepper motor connects straight into the control module and is used to ‘open’ and ‘close’ the vent for cooling air. 
The water sensor is connected to pin A0. This tells the circuit to go into error status or to continue whatever is going on. If the sensor drops below about half the red LED will turn on and stop everything else going on.
The LCD screen has VSS to a potentiometer output. This potentiometer gets power from the 5V and then outputs to V0 as well. VDD to power. RS to 11. RW to GND. E to 11. D4 to 2. D5 to 3. D6 to 4. D7 to 5. A goes to a 330 resistor and power. K goes to GND. The LCD screen shows temperature and himidity and when the water sensor is too low then the LCD will show “Water level too low.”
The four buttons on the breadboard have specific tasks to them each. The first button on the left connects to pin 18. This button is for turning on the circuit.  The second button connects to pin 8 and is for turning off the circuit to idle. The next is the reset button to pin 9. The last button is for the stepper motor connected to pin 10. This button is for spinning the stepper motor to open the ‘vent’.
All of the LEDs have 330 resistors to the anode, and the cathode is connected to Arduino pins. The green LED shows the temperature under threshold and is connected to pin 53. The yellow LED shows when the circuit is idle and that is connected to pin 50. The blue LED shows when the fan is on and spinning to bring the temperature down. This is connected to the pin 51. The last LED red is to show when the water is below threshold and is the error status. This one is connected to pin 52.

## System Overview
For the idle state, the temperature had to be below 27 degrees Celsius. For the fan to activate and switch to the running state (blue LED), the temperature needed to be above 27 degrees Celsius. There was no additional power supply needed.


