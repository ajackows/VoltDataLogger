#include <avr/io.h>
#include <avr/interrupt.h>


#define VERBOSE
//#define DEBUG

#define VOLTREAD A0
#define ADCMAXLVL 5.102
#define ADCRES 1023


//fucntion declarations
int IntrSet();
void ReadADC();

//globals
int numCnt;
uint16_t tot = 0;


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
      case 'a':
        Serial.println("boop");
        break;
      case 'b': 
        Serial.println("supdawg");
        break;
      default :
        break;
    }
  }
}

void ReadADC(uint16_t movTot)
{
  //variable declarations
  uint16_t adcRaw;
  double volt;
  double movAvg;
  //read voltage from ADC
  adcRaw = analogRead(VOLTREAD);

  movAvg = movTot/10;
  
  //calculate volatage from ADC value using overvoltage correction
  volt = ((double)adcRaw*ADCMAXLVL)/ADCRES; 
  //print raw ADC value (for debug purposes)
  //Serial.print(adcRaw);
  //Serial.print("\t");

  //print calculated voltage

  
  Serial.println(volt,3);  

}


uint16_t WindowAvg(uint16_t movgAvg)
{
  uint16_t adcRaw;
  //read from ADC
  adcRaw = analogRead(VOLTREAD);

  
  #ifdef DEBUG
  Serial.println(movgAvg);
  
  Serial.print(adcRaw);
  Serial.print("\t");
  #endif
  
  if(movgAvg == 0)
  {
    return adcRaw;
  }else{
    return (adcRaw + movgAvg);
  }
  
}

int IntrSet()
{
   //set timer control register A for normal operation
   TCCR1A = 0x00;
   //set timer control register B to run clock with prescale 
   TCCR1B =  (1 << CS12); // set 1024 prescaler

   TIMSK1 = (1 << TOIE1); // enable timer1 overflow interrupt
   TCNT1 = 0;
  
   sei(); // enable interrupts
}



ISR(TIMER1_OVF_vect)
{
  numCnt++;
  #ifdef DEBUG
  Serial.println(numCnt);
  #endif


  if(numCnt >= 20)
  { 
    tot = WindowAvg(tot);

    #ifdef DEBUG
    Serial.print("TOTAL: ");
    Serial.println(tot);
    #endif
  }
  if(numCnt >= 30)
  {
    numCnt = 0;
    ReadADC(tot);
    tot = 0;
  }

}

