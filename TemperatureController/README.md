# Temperature controller
## This project aims for designing a controller to regulate the temperature using a temperature sensor and a DC motor fan
## Implementation steps
* The control would be made using an Arduino UNO interface
* Designing an electrical circuit based on a transistor to controll the DC motor voltage
* Developing a code that is capable of mapping the sensor readings with the PWM value for the transistor base
* Analizing and testing the final product
## Context
### The readings of the voltage from the temperature sensor (thermistor) is converted to Celsius temperature. This value is used to map the PWM value of the input voltage for the transistor base. 
### The DC motor voltage supply is 9V and the aim of the project is to proportionally regulate the speed of the motor based on the temperature readings