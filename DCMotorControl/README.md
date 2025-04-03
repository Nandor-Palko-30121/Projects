# DC Motor Control 
## This project implements a control system for a DC motor equipped with an encoder, using an Atmega32 microcontroller mounted on an Arduino interface. The primary objective is to enable the motor to perform a specified number of rotations in one direction and then return to its initial position through a reverse rotation. Additionally, the user can adjust motor speed in real-time via a GUI built with the Processing development environment.
## Features
* Bidirectional Control: The motor can rotate clockwise and counterclockwise using a motor driver (TB6612FNG).
* Encoder Feedback: Utilizes a GA12-N20 DC motor with integrated encoder, providing 413 pulses per full rotation.
* PID Controller: Implements a PID controller for precise positioning and speed control during reverse rotation.
* User Interface: A GUI facilitates user interaction, providing real-time monitoring and control of motor speed and position.
* Software Implementation: Developed using Arduino IDE for microcontroller programming and Processing for GUI development.
## Possible improovements
* Implementing more robust noise filtering for speed calculation.
* Optimizing the PID controller using established tuning methods (e.g., Ziegler-Nichols).
* Improving GUI speed control by handling data transmission more efficiently.
