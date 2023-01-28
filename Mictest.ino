unsigned long startMillis= 0;               //start of sample window
unsigned int peakToPeak = 0;                //peak-to-peak level
const unsigned int sampleWindow = 2000;     //measurement window 100ms

unsigned int signalMax = 0;
unsigned int signalMin = 4095;

int sample = 0;
    
void setup(){
}

void loop(){ 
    
    signalMax = 0;
    signalMin = 4095;
    startMillis =  millis();
    //collect data for 50 mS
    while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);
      if (sample < 4095)            //toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;     //save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;     //save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;          //max - min = peak-peak amplitude
   double volts = (peakToPeak * 3.3) / 4095;    //convert to volts
   
   Serial.println(volts);
   Particle.publish("SoundMax",String(peakToPeak));
   delay(2000);
}