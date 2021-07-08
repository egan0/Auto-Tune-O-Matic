#define SPLIT 1.059463094359
#define NUM_NOTES 88
#define NOTENAME(n) (name[((n)+9)%12])
#define FREQUENCY(n) ( pow( SPLIT, (n)-49. ) * 440. + .5)
String name[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

//sets the window of precision of the tuning. default is 0.4%. smaller % = more accurate but harder to tune and can be jumpy if there are frequency fluctuations.
float margin = 0.4/100; //margin of accuracy 

//set pins used for 9 segment display and leds
int ledPins[11] = {2,3,4,5,9,8,7,6,18,19,17};

//timer variables
byte newData = 0;
byte prevData = 0;
unsigned int timer = 0;
unsigned int period;
float frequency;

ISR(ADC_vect) {
  prevData = newData;
  newData = ADCH;
  if (prevData < 150 && newData >=150){
    period = (period + timer) / 2;
    timer = 0;
  }
  timer++;
}

void setDisplay(String note, int noteOutBy) {   //Decide what to show on the led and cathode
   String displaySet = "00000000000";
   if (note == "C")  displaySet = "11000110";
   if (note == "C#") displaySet = "11010110";
   if (note == "D")  displaySet = "11101001";
   if (note == "D#") displaySet = "11111001";
   if (note == "E")  displaySet = "11000111";
   if (note == "F")  displaySet = "10000111";
   if (note == "F#") displaySet = "10010111";
   if (note == "G")  displaySet = "01101111";
   if (note == "G#") displaySet = "01111111";
   if (note == "A")  displaySet = "10101111";
   if (note == "A#") displaySet = "10111111";
   if (note == "B")  displaySet = "11101111";
   if (noteOutBy < 0)  displaySet = displaySet + "100";
   if (noteOutBy > 0)  displaySet = displaySet + "001";
   if (noteOutBy == 0) displaySet = displaySet + "010";
   for (int n=0;n<11;n++) { digitalWrite(ledPins[n],  displaySet[n] == '0' ? LOW : HIGH); }
}

void getNote() {     //Calculate the note for the current frequency
  float noteFrequ1;    //note before the one detected
  float noteFrequ2;    //note detected
  float noteFrequ3;    //note after the one detected
  float noteOutBy;     //the number of hz that the note is off tune by

  for (int n = 0; n < NUM_NOTES; n++) {
    //get frequency ranges for comparing to current frequency
    noteFrequ1 = FREQUENCY(n);
    noteFrequ2 = FREQUENCY(n+1);
    noteFrequ3 = FREQUENCY(n+2);
   
    if ((frequency > noteFrequ2 - ((noteFrequ2 - noteFrequ1)/2)) && (frequency <= noteFrequ3 - ((noteFrequ3 - noteFrequ2)/2))) {       
      noteOutBy = frequency - noteFrequ2;      
      if ((noteOutBy < 0  &&  noteOutBy + ((noteFrequ2*margin)+0.5) >= 0) || (noteOutBy > 0   &&    noteOutBy - ((noteFrequ2*margin)+0.5) <= 0)) {  
          noteOutBy = 0;  //tuning is within the note tolerance
      }
      setDisplay(NOTENAME(n),(int)noteOutBy);
      break;  //exit loop once the correct note was found and processed
    }
  }
}

void setup(){
  Serial.begin(9600);   //start talking to the computer over serial for debugging
  for (int n=0;n<11;n++) { pinMode(ledPins[n],  OUTPUT); }  
  cli();                                  //doable interrupts
  //set up continuous sampling of analogue pin 0 at 38.5kHz
  ADCSRA = 0;                             //clear registers
  ADCSRB = 0;                             //clear registers   
  ADMUX |= (1 << REFS0);                  //set reference voltage
  ADMUX |= (1 << ADLAR);                  //left align the ADC value- so we can read highest 8 bits from ADCH register only  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0);  //set ADC clock with 32 pre scaler - 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE);                 //enable auto trigger
  ADCSRA |= (1 << ADIE);                  //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);                  //enable ADC    
  ADCSRA |= (1 << ADSC);                  //start ADC measurements
  sei();                                  //enable interrupts
}

void loop(){
  frequency = (38440./period) * pow(SPLIT, 24.) ; 
  getNote();
  //Serial.println(frequency);
  delay(70);
}
