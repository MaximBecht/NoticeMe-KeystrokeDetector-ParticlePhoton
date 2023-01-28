int led1 = D7;
int button = D6;

//variables noise and typing for calibrated maximum sound
unsigned int noise = 0;
unsigned int typing = 0;
int threshhold = 0;

int measure_soundlevel(int);



//------event functions------
void setup() {
    pinMode(led1, OUTPUT);
    pinMode(button, INPUT_PULLDOWN);
    //here we need to get previously calibrated sound levels (else we need to calibrate after every shutdown)
}

void loop() {
    if(digitalRead(button)){
        calibrate();
    }
    else {
        //calculate Wpm for 1 second and publish to topic
        //50ms is the min. time per keystroke (at 200wpm)
        
        measure_keystrokes_s(1);
        
        int keystrokes = measure_keystrokes_s(1);
        
        //calculate wpm (with 1 word = 5 characters) and send if greater 0
        float wpm = keystrokes * 60 / 5;
        if(wpm>0){
            Particle.publish("current_wpm",String(wpm));
    }
}



//------calculating functions------
void calibrate(){
    led_s(1);
    
    //measure noise (talk now!)
    noise = measure_soundlevel(5);
    Particle.publish("SoundMax_Noise",String(noise));
    
    //led to indicate change of state
    led_s(1);
    
    //measure typing (doesn't  matter if your talking)
    typing = measure_soundlevel(5);
    Particle.publish("SoundMax_Typing",String(typing));

    threshhold = calculate_threshhold(noise, typing);
    
    //indicate finishing of calibration
    led_blink_s(3);
}

int measure_soundlevel(int sampleWindow){
    unsigned int signalMax = 0, signalMin = 4096, sample = 0;;
    unsigned int startMillis = millis();
    
    while (millis() - startMillis < sampleWindow*1000){
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
   
   return signalMax;
}

int calculate_threshhold(int noise_level, int max_volume){
    //constant modifier for detecting input calculation
    const float detection_modifier = 0.01;
    const int offset = 0;
    
    int threshhold = offset + noise_level + ((max_volume - noise_level)*detection_modifier);
    return threshhold;
}

/**
 * Measure the total amount of keystokes in the given time
 * @param s: int time in seconds we want to measure
 * @returns: int total amount of keystrokes
 */
int measure_keystrokes_s(int s){
    int start = millis();
    int keystrokes = 0;
    bool is_high = false;
    int cooldown = 40; //this cooldown is required to prevent the microphone to pick up noise/reverb after a keystroke
    
    int debug_measurements = 0;
    
    //start counting keystrokes
    while(millis()-start < s*1000){
        
        debug_measurements+=1;
        if(analogRead(0) > threshhold){
            is_high = true;
        } else if(is_high) {
            is_high = false;
            keystrokes += 1;
            delay(cooldown);
        }
    }
    
    return keystrokes;
}



//------led functions------
void led_s(int s){
    int led_start = millis();
    digitalWrite(led1, HIGH);
    delay(1000*s);
    digitalWrite(led1, LOW);
}

void led_blink_s(int s){
    int blink_start = millis();
    while(millis()-blink_start < s*1000){
        digitalWrite(led1, HIGH);
        delay(200);
        digitalWrite(led1, LOW);
        delay(200);
    }
    digitalWrite(led1, LOW);
}