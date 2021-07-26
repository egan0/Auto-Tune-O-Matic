# Look Mum No Computer - 1222 Tuner VCO Project

Have a look at LOOKMUMNOCOMPUTER's video on building the 1112 performance oscillators here https://www.lookmumnocomputer.com/projects#/1222-performance-vco.

-----------------------------------------------------------------------------------

# Modified Arduino Code - '1222_VCO_V2.ino' - Version 2

-- UPDATE Version 2 --

Verison 2 does nothing different than the first version, but it eliminates the need to loop through each note for comaparing and finding the note being sent to the Arduino.  The new code directly calculates the note being played.  No real improvements, but less code to look at.

-----------------------------------------------------------------------------------

# Modified Arduino Code - '1222_VCO.ino' - Version 1

-- UPDATE --

The original code was increasing the frequency by multiplying it by 10, but this caused problems with matching the frequency to the note as each note is not a multiplication of 1 (semitone multiplied by the 12th root of 2, which is about 1.059463094359).  The frequency sampled is now shifted by 24 semitones to increase it to a value that is easier to work with and retain the original note.

The code used for sampling the frequency was great but could be simplified as the voltage sampled is always a clean wave.  The code has been simplified and the period calculation has been averaged to smooth out any fluctuations at the higher frequencies.

-----------------------------------------------------------------------------------

The '1222_VCO.ino' code is a modified version to use frequency-to-note conversion using equations rather than tables.  The equations allow for more accurate frequencies when calibrating the VCO using an external tuner. 

This code is an adapted version of the code written and published by Jo Bouten (https://github.com/josbouten/Tune-O-Matic), based on code published by Amanda Ghassaei's Instructables and Sam's LookMumNoComputer. 

# Auto Tuning Code - 'Auto-Tune-O-Matic.ino' 

** This is just for proof of concept.  The auto tuning works fine when using the VCO as a standalone oscillator with only the onboard tuning, but not with any CV input. ** 

This code is an adapted version of the code written and published by Jo Bouten (https://github.com/josbouten/Tune-O-Matic), based on code published by Amanda Ghassaei's Instructables and Sam's LookMumNoComputer. 

The auto tune code is designed to be used with physical modifications to the original design of the 1222 Tuner VCO project.  Please do not make these changes unless you know what you are doing and do so at your own risk.  

The code is designed to use pin D11 on the Arduino Nano which passes through a low pass filter and through a 2M resistor to pin 15 of the 3340.  

Please see https://lookmumnocomputer.discourse.group/t/1222-tuner-vco-module-fixes-improvements/2072/41 for more details on this mod.
