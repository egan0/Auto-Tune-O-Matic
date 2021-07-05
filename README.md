# Auto-Tune-O-Matic
Auto tuning code for the Look Mum No Computer - 1222 Tuner VCO project.


** This is just for proof of concept.  The auto tuning works fine when using the VCO as a standalone oscillator with only the onboard tuning, but not with any CV input. ** 


Have a look at LOOKMUMNOCOMPUTER's video's on building the 1112 performance oscillators here https://www.lookmumnocomputer.com/projects#/1222-performance-vco

This code is an adapted version of the code published by Jo Bouten (https://github.com/josbouten/Tune-O-Matic) and based on code published by by Amanda Ghassaei's Instructables and Sam's LookMumNoComputer. 

The code is designed to be used with physical modifications to the original design of the 1222 Tuner VCO project.  Please do not make these changes unless you know what you are doing and do so at your own risk.  

The code is designed to use pin D11 on the Arduino Nano which passes through a low pass filter and through a 2M resistor to pin 15 of the 3340.  

Please see https://lookmumnocomputer.discourse.group/t/1222-tuner-vco-module-fixes-improvements/2072/41 for more details on this mod.

