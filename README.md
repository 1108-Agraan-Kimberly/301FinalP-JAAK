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
	
 The last state is running state. The fan motor is on and will run intil the temperature drops below the threshold. If the water becomes too low it will transition to the error state. The blue LED should be turned on when in this state.
 
``
