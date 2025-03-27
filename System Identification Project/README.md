# System Identification Project
## Objective
### This project aims for identifying a mathematical model for a given physical system based on the resonance phenomenon.
## Context
### Having a physical represented by a circuit, one can apply at the input of the system a sinusoidal signal with a frequency range from 1 Hz to 1 kHz. 
### The purpose of this input signal is to find the resonance frequency and determine through experimental data collection the specific mathematical model of the system. 
## Implementation steps
* Collecting the input/output data after applying the signal
* Finding the mathematical model of the system using non-parametrical methods (based on manual measurements on the collected data)
* Finding the mathematical model of the system using parametrical system identification methods 
* Compare and decide which model describes the real behaviour of the system the best.
## Validation of the model
* The validation procces consists of comparing the collected data with the frequency response of the identified model.
* For the non-parametrical method, validation sould be obtained through NMSE (Normalised Mean Square Error). The error should be less than 10% for the identified system to be validated.  
* For the parametrical approach, 2 methods were used for autocorrelation validation and 2 methods were used for intercorrelation validation, thus obtaining 4 different models.