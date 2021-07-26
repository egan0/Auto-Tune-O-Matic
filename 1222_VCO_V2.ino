#define SPLIT 1.059463094359
#define NOTE_BIN(n) (ledbin[((n)+9)%12])
#define FREQ_NUM(n,y) ((log((n) / 440) / log(SPLIT)) + (y))

//sets the precision of the tuning. default is 0.08. smaller = more accurate but harder to tune and can be jumpy if there are frequency fluctuations.
float margin = 0.08;   //margin of precision

//set pins used for 9 segment display and leds
int ledPins[11] = {2,3,4,5,9,8,7,6,18,19,17};
String ledbin[12]={"11000110","11010110","11101001","11111001","11000111","10000111","10010111","01101111","01111111","10101111","10111111","11101111"};

//timer variables
byte newData = 0;
byte prevData = 0;
unsigned int timer = 0;
volatile unsigned int period;
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

void setDisplay(int notePointer, float noteOutBy) {   //Decide what to show on the led and cathode
   String displaySet = NOTE_BIN(notePointer);
   if (noteOutBy >= .5) displaySet = displaySet + "100";
   if (noteOutBy == .0) displaySet = displaySet + "010";
   else { if (noteOutBy < .5)  displaySet = displaySet + "001"; }
   for (int n=0;n<11;n++) { digitalWrite(ledPins[n],  displaySet[n] == '0' ? LOW : HIGH); }
}

void getNote() {     //Calculate the note for the current frequency
  float VCOFreqFloat = FREQ_NUM(frequency,49);         
  int VCOFreqInt = (int)FREQ_NUM(frequency,49);       
  float VCOFreqCent = VCOFreqFloat - VCOFreqInt;  
  if (VCOFreqCent < 0.5) VCOFreqInt = VCOFreqInt - 1;
  if ((VCOFreqCent < margin) || (VCOFreqCent > 1. - margin)) VCOFreqCent = .0;
  setDisplay(VCOFreqInt, VCOFreqCent);
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
