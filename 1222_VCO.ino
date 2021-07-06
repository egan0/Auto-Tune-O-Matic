#define NUM_NOTES 88
#define NOTENAME(n) (name[((n)+5)%12])
#define FREQUENCY(n) ( pow( pow(2.,1./12.), (n)-49. ) * 440. + .5)
String name[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

//sets the accuracy of the tuning. default is 0.45%. smaller % = more accurate but harder to tune and can be jumpy due to frequency fluctuations.
float margin = 0.45/100; //margin of accuracy 

// LED OUTPUT PINS
int LED3 = 18;
int LED4 = 19;
int LED5 = 17;

// 9 segment display output pins;
int LEDE  = 2;
int LEDD  = 3;
int LEDC  = 4;
int LEDDP = 5;
int LEDB  = 9;
int LEDA  = 8;
int LEDF  = 7;
int LEDG  = 6;

//------------------------------------------------------------------------------------------------------------------------- 
//generalized wave freq detection with 38.5kHz sampling rate and interrupts
//by Amanda Ghassaei
//https://www.instructables.com/id/Arduino-Frequency-Detection/
//Sept 2012

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/

//data storage variables
byte newData = 0;
byte prevData = 0;
unsigned int time = 0;  //keeps time and sends vales to store in timer[] occasionally
int timer[10];          //storage for timing of events
int slope[10];          //storage fro slope of events
unsigned int totalTimer;//used to calculate period
unsigned int period;    //storage for period of wave
byte index = 0;         //current storage index
int frequency;          //storage for frequency calculations
int maxSlope = 0;       //used to calculate max slope as trigger point
int newSlope;           //storage for incoming slope data

//variables for decided whether you have a match
byte noMatch = 0;       //counts how many non-matches you've received to reset variables if it's been too long
byte slopeTol = 3;      //slope tolerance- adjust this if you need
int timerTol = 10;      //timer tolerance- adjust this if you need

ISR(ADC_vect) {         //when new ADC value ready
  
  prevData = newData;   //store previous value
  newData = ADCH;       //get value from A0
  if (prevData < 127 && newData >=127){   //if increasing and crossing midpoint
    newSlope = newData - prevData;        //calculate slope
    if (abs(newSlope-maxSlope)<slopeTol){ //if slopes are ==
      //record new data and reset time
      slope[index] = newSlope;
      timer[index] = time;
      time = 0;
      if (index == 0){    //new max slope just reset
        noMatch = 0;
        index++;          //increment index
      }
      else if (abs(timer[0]-timer[index])<timerTol && abs(slope[0]-newSlope)<slopeTol) { //if timer duration and slopes match
        //sum timer values
        totalTimer = 0;
        for (byte i=0;i<index;i++){ totalTimer+=timer[i]; }
        period = totalTimer;    //set period
        //reset new zero index values to compare with
        timer[0] = timer[index];
        slope[0] = slope[index];
        index = 1;    //set index to 1
        noMatch = 0;
      }
      else {         //crossing midpoint but not match
        index++;    //increment index
        if (index > 9) { reset(); }
      }
    }
    else if (newSlope>maxSlope){  //if new slope is much larger than max slope
      maxSlope = newSlope;
      time = 0;         //reset clock
      noMatch = 0;
      index = 0;        //reset index
    }
    else{               //slope not steep enough
      noMatch++;        //increment no match counter
      if (noMatch>9){
        reset();
      }
    }
  }
  time++;//increment timer at rate of 38.5kHz
}

void reset(){   //clear out some variables
  index = 0;    //reset index
  noMatch = 0;  //reset match counter
  maxSlope = 0; //reset slope
}

//------------------------------------------------------------------------------------------------------------------------- 

void setNote(String segments) {
    // Decode cathode
    digitalWrite(LEDE,  segments[0] == '0' ? LOW : HIGH);
    digitalWrite(LEDD,  segments[1] == '0' ? LOW : HIGH);
    digitalWrite(LEDC,  segments[2] == '0' ? LOW : HIGH);
    digitalWrite(LEDDP, segments[3] == '0' ? LOW : HIGH);
    digitalWrite(LEDB,  segments[4] == '0' ? LOW : HIGH);
    digitalWrite(LEDA,  segments[5] == '0' ? LOW : HIGH);
    digitalWrite(LEDF,  segments[6] == '0' ? LOW : HIGH);
    digitalWrite(LEDG,  segments[7] == '0' ? LOW : HIGH);
}

void setLed(String segments) {
    // Decode led pattern
    digitalWrite(LED3, segments[0] == '0' ? LOW : HIGH);
    digitalWrite(LED4, segments[1] == '0' ? LOW : HIGH);
    digitalWrite(LED5, segments[2] == '0' ? LOW : HIGH);
}

void setDisplay(String note, int noteOutBy) {
   //Decide what to show on the led and cathode
   if (note == "C")  setNote("11000110");
   if (note == "C#") setNote("11010110");
   if (note == "D")  setNote("11101001");
   if (note == "D#") setNote("11111001");
   if (note == "E")  setNote("11000111");
   if (note == "F")  setNote("10000111");
   if (note == "F#") setNote("10010111");
   if (note == "G")  setNote("01101111");
   if (note == "G#") setNote("01111111");
   if (note == "A")  setNote("10101111");
   if (note == "A#") setNote("10111111");
   if (note == "B")  setNote("11101111");
   if (noteOutBy < 0)  setLed("100");
   if (noteOutBy > 0)  setLed("001");
   if (noteOutBy == 0) setLed("010");
}

void getNote() {
  //Calculate the note for the current frequency
  int noteFrequ1;       //note before the one detected
  int noteFrequ2;       //note detected
  int noteFrequ3;       //note after the one detected
  int noteOutBy;       //the number of hz that the note is off tune by

  for (int n = 0; n < NUM_NOTES; n++) {
    
    //get frequency ranges for comparing to current frequency
    noteFrequ1 = (int)FREQUENCY(n);
    noteFrequ2 = (int)FREQUENCY(n+1);
    noteFrequ3 = (int)FREQUENCY(n+2);
   
    if ((frequency > noteFrequ2 - ((noteFrequ2 - noteFrequ1)/2)) && (frequency <= noteFrequ3 - ((noteFrequ3 - noteFrequ2)/2))) {       
      noteOutBy = frequency - noteFrequ2;
      if (  ( (noteOutBy < 0) && (noteOutBy + (noteFrequ2*margin) < 0) ) || ( (noteOutBy > 0) && (noteOutBy - (noteFrequ2*margin) > 0) )  ) {  
        //tuning is required
      }
      else {
        noteOutBy = 0;  //tuning is within the note tolerance
      }
      setDisplay(NOTENAME(n),noteOutBy);
      break;  //exit loop
    }
  }
}

void setup(){
  Serial.begin(9600);
  pinMode(LED3,  OUTPUT);
  pinMode(LED4,  OUTPUT);
  pinMode(LED5,  OUTPUT);
  pinMode(LEDE,  OUTPUT);
  pinMode(LEDD,  OUTPUT);
  pinMode(LEDC,  OUTPUT);
  pinMode(LEDDP, OUTPUT);
  pinMode(LEDB,  OUTPUT);
  pinMode(LEDA,  OUTPUT);
  pinMode(LEDF,  OUTPUT);
  pinMode(LEDG,  OUTPUT);
    
  cli();  //doable interrupts
  //set up continuous sampling of analogue pin 0 at 38.5kHz
  ADCSRA = 0;             //clear registers
  ADCSRB = 0;             //clear registers   
  ADMUX |= (1 << REFS0);  //set reference voltage
  ADMUX |= (1 << ADLAR);  //left align the ADC value- so we can read highest 8 bits from ADCH register only  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 pre scaler - 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enable auto trigger
  ADCSRA |= (1 << ADIE);  //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  //enable ADC
  ADCSRA |= (1 << ADSC);  //start ADC measurements
  sei();  //enable interrupts
}
 
void loop(){
  //frequency = (38462*10)/period;  //ORIGINAL frequency timer rate/period
  frequency = (38745*10)/period;  //found this to be more accurate when when calibrating with external tuner
  
  getNote();
 
  Serial.println(frequency);
  delay(70);
}
