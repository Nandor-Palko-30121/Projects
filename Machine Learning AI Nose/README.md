# Machine Learning AI Nose
## Objective
### This project aims to develop a device capable of detecting different scents and recognizing them, provinding an output with the name of the scent.
### The approach for this project was using a neural network trained to specifically identify different organic materials based on the readings of a multichannel gas sensor to detect different scents eliminated by the materials.
### For the processing of the data in real time and for a user-friendly interface, a Wio Terminal will be used
## Implementation
### The implementation of this device consists in 4 steps:
* Designing a code for data collection
* Collecting datasets from the multichannel gas sensor and storing them in .csv files
* Designing and training a neural network using DSP techniques based on the collected data
* Developing a code to use the neural network in real-time
## Part 1: Designing a code for data collection
* The aim is to design a code in Arduino IDE for collecting datasets from the gas sensor readings
* Every dataset would have 10 collumns of data from exposure to a specific material.
* It is necessary to create a program which stores these datasets in .csv files. The program was made in Python
## Part 2: Collecting data from the gas sensor
* After the implementation of the data collection programs, the multichannel gas sensor is exposed to organic materials, such as cofee, mint tea, fruit tea and plain air.
* In the meantime, the data collection programs are let to run for 15-30 min, thus collecting measurements of Carbon monoxide (CO), Nitrogen dioxide (NO2), Ethyl alcohol(C2H5CH) and Volatile Organic Compounds (VOC).
* After collecting the data from one organic material, the sensor is blown with a fan to eliminate any residues before exposing it to another organic material.
## Part 3: Designing and training the neural network
* Using the Edge Impulse interface, it is possible to create a neural network with a machine learning algorithm based on the collected data.
* After creating and training the neural network model, Edge Impulse is capable of generating a library compatible with Arduino IDE for microcontroller implementation.
## Part 4: Developing a code to use the neural network
### The code consists of: 
* Reading the data in real-time from the sensor
* Storing the data in a buffer for future use
* Transforming the raw data from the buffer into a specific signal
* Running the obtained signal through the neural network
* Reading the output prediction provided by the neural network
* Printing the results on the Wio Terminal along with the trust index
* On the serial monitor, it will be printed the "trust index" of each material
* The material with the biggest trust index will be displayed 
* If the neural network is confused, it would be printed the word "Unknown" on the terminal 
## Applications
### This device has the potential to be used in medical applications, especially for people with olfactory problems
### It can be used in human-like robots to give them the sense of smell, thus contributing to the humanization of the machines
### Another application could be the necesity of detecting various gaseous substances without the need of using a great number of different specific sensor for each substance.
