#include <avr/io.h>
#include <avr/interrupt.h>
//#include "ADCdriver.c"


#define VERBOSE
//#define DEBUG

#define VOLTREAD A0
#define ADCMAXLVL 5.102 //needs to be measured from 5v line of arduino
#define ADCRES 1023


//fucntion declarations
int IntrSet();
void DisAvgpDat();

//globals

uint16_t tot = 0;
uint32_t timeStamp = 0; //time stamp in seconds
uint8_t itrFlg = 0;

uint8_t stat= 0x0;

double* datArr;
uint16_t indx = 0;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  IntrSet();
}

void loop() 
{
  
  if( Serial.available()) 
  {

    char cmd = Serial.read();

    //command case statement for logging upgrade
    switch(cmd)
    {

      //commands to implement:
      // - store data log
      // - print data log and clear
      // - switch to verbose mode (display measurements)
      case 'a':
        Serial.println("STARTING DATA LOG");

        
        break;
      case 'b': 
        Serial.println("ENDING DATA LOG");
        break;
      default :
        break;
    }
  }

  if(itrFlg == 1)
  {
    if((timeStamp%30) >= 20)
    { 
      //calculate total for moving average
      tot = WindowTot(tot);
  
      #ifdef DEBUG
      Serial.print("TOTAL: ");
      Serial.println(tot);
      #endif
    }
    if((timeStamp %30 == 0)&& timeStamp > 0)
    {
      
      //display data to port
      DisAvgDat(tot);
      tot = 0; // reset moving total
    }
    itrFlg = 0;
  }
}



//prints out voltage value to Serial port
void printVal(uint32_t tStamp, double adcVal)
{
  
  double volt;
  //calculate volatage from ADC value using overvoltage correction
  volt = ((double)adcVal*ADCMAXLVL)/ADCRES; 


  //print calculated voltage
  Serial.print("Time(s): \t");
  Serial.print (tStamp);
  Serial.print("\t Voltage(v): \t");
  Serial.println(volt,3); 
}

void DisAvgDat(uint16_t movTot)
{
  //variable declarations
  uint16_t adcRaw;
  double movAvg;

  //calcualte average ADC measurement from moving total
  movAvg = movTot/10;

  //TODO: add case for displaying voltage or logging based off of state

  //print out voltage value based off of moving average from ADC
  printVal(timeStamp, movAvg);

}

uint16_t WindowTot(uint16_t movgTot)
{
  uint16_t adcRaw;
  
  //read from ADC
  adcRaw = analogRead(VOLTREAD);
  
  
  #ifdef DEBUG
  Serial.print("raw Value:");
  Serial.print(adcRaw);
  Serial.print("\t");
  #endif
  
  //if this is the first value of the moving total replace the moving total with measurement else add to moving total
  if(movgTot == 0)
  {
    return adcRaw;
  }else
  {
    return (adcRaw + movgTot);
  }
  
}


int IntrSet()
{
   //set timer control register A for normal operation
   TCCR1A = 0x00;
   //set timer control register B to run clock with prescale 
   TCCR1B = (1<<CS10) | (1 << CS12); // set 256 prescaler

   TIMSK1 = (1 << TOIE1); // enable timer1 overflow interrupt
   TCNT1 = 49911;
        
   sei(); // enable interrupts
}


ISR(TIMER1_OVF_vect)
{
  //increment time stamp (1 sec)
  timeStamp++;

  //set interrupt flag for ADC operations
  itrFlg = 1;
  
  #ifdef DEBUG
  Serial.println(timeStamp % 30);
  #endif
  
  //reset timer counter value so it counts from this number (used to ensure 1sec interrupts)
  TCNT1 = 49911;
  
}
