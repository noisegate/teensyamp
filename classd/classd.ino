/* 
 *  It measures continuously the voltage on pin A2 & A3,
 *  converts the values to 4 PWM (left/right positive/negative for Hbridge)
 *  
 *  Serial interface
 *  Write v and press enter on the serial console to get the value
 *  Write c and press enter on the serial console to check that the conversion is taking place,
 *  Write t to check if the voltage agrees with the comparison in the setup()
 *  Write s to stop the conversion, you can restart it writing r.
 *
 *  https://forum.pjrc.com/threads/25532-ADC-library-update-now-with-support-for-Teensy-3-1
 *  simultaneous read on two adc for stereo...
 *  
 *
 */

#include <ADC.h>

// ARDUINOPATH/hardware/teensy/avr/cores/teensy3/pins_arduino.h 
const int readPin1 = A2; // ADC0
const int readPin2 = A3; // ADC1

//https://www.pjrc.com/teensy/td_pulse.html
#define OUTLP 23
#define OUTLN 22
#define OUTRP 21
#define OUTRN 20


ADC *adc = new ADC(); // adc object

void setup() {
    
    pinMode(readPin2, INPUT);
    pinMode(readPin1, INPUT);

    pinMode(OUTLP, OUTPUT);
    pinMode(OUTLN, OUTPUT);
    pinMode(OUTRP, OUTPUT);
    pinMode(OUTRN, OUTPUT);

    pinMode(LED_BUILTIN, OUTPUT);
    
    analogWriteFrequency(OUTLP, 35156.25);//pin 6 pwm 10 bit
    analogWriteResolution(10);
    analogWriteFrequency(OUTLN, 35156.25);//pin 6 pwm 10 bit
    analogWriteResolution(10);
    analogWriteFrequency(OUTRP, 35156.25);//pin 6 pwm 10 bit
    analogWriteResolution(10);
    analogWriteFrequency(OUTRN, 35156.25);//pin 6 pwm 10 bit
    analogWriteResolution(10);

    Serial.begin(9600);

    ///// ADC0 ////
    // reference can be ADC_REF_3V3, ADC_REF_1V2 (not for Teensy LC) or ADC_REF_EXT.
    adc->setReference(ADC_REF_3V3, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2
    adc->setReference(ADC_REF_3V3, ADC_1);
    
    adc->setAveraging(1); // set number of averages
    adc->setResolution(12); // set bits of resolution

    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADC_ADACK_2_4, ADC_ADACK_4_0, ADC_ADACK_5_2 and ADC_ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->setConversionSpeed(ADC_LOW_SPEED); // change the conversion speed
    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_LOW_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)

    ////// ADC1 /////
    #if defined(ADC_TEENSY_3_1)
    adc->setAveraging(1, ADC_1); // set number of averages
    adc->setResolution(10, ADC_1); // set bits of resolution
    adc->setConversionSpeed(ADC_VERY_LOW_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_VERY_LOW_SPEED, ADC_1); // change the sampling speed
    adc->setAveraging(1, ADC_0); // set number of averages
    adc->setResolution(10, ADC_0); // set bits of resolution
    adc->setConversionSpeed(ADC_VERY_LOW_SPEED, ADC_0); // change the conversion speed
    adc->setSamplingSpeed(ADC_VERY_LOW_SPEED, ADC_0); // change the sampling speed

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    adc->enableInterrupts(ADC_1);
    //adc->startContinuous(readPin2, ADC_1);
    adc->startSynchronizedContinuous(readPin1, readPin2);
    #endif

    Serial.print("Amp is awake...");
    delay(500);
}

int value = 0;
int value2 = 0;
char c=0;
ADC::Sync_result result;

void loop() {
    
    if (Serial.available()) {
        c = Serial.read();
        if(c=='c') { // conversion active?
            #if defined(ADC_TEENSY_3_1)
            Serial.print("Converting? ADC1: ");
            Serial.println(adc->isConverting(ADC_1));
            #endif
        } else if(c=='s') { // stop conversion
            //adc->stopContinuous(ADC_1);
            adc->stopSynchronizedContinuous();
            Serial.println("Stopped");
        } else if(c=='t') { // conversion successful?
            #if defined(ADC_TEENSY_3_1)
            Serial.print("Conversion successful? ADC1: ");
            Serial.println(adc->isComplete(ADC_1));
            #endif
        } else if(c=='r') { // restart conversion
            Serial.println("Restarting conversions ");
            //adc->startContinuous(readPin2, ADC_1);
            adc->startSynchronizedContinuous(readPin1, readPin2);
        } else if(c=='v') { // value
            //Serial.print("Value ADC0: ");
            //value = (uint16_t)adc->analogReadContinuous(ADC_0); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
            //Serial.println(value*3.3/adc->getMaxValue(ADC_0), DEC);
            
            Serial.print("Value ADC0: ");
            Serial.print(result.result_adc0*3.3/adc->getMaxValue(ADC_0),DEC);
            Serial.print(", Value ADC1: ");
            Serial.println(result.result_adc1*3.3/adc->getMaxValue(ADC_1),DEC);            
            #if defined(ADC_TEENSY_3_1)
            Serial.print("Value ADC1: ");
            value2 = (uint16_t)adc->analogReadContinuous(ADC_1); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
            Serial.println(value2*3.3/adc->getMaxValue(ADC_1), DEC);
            #endif
        } else if(c=='n') { // new single conversion on readPin3
            // this shows how even when both ADCs are busy with continuous measurements
            // you can still call analogRead, it will pause the conversion, get the value and resume the continuous conversion automatically.
            //Serial.print("Single read on readPin3: ");
            //Serial.println(adc->analogRead(readPin3)*3.3/adc->getMaxValue(ADC_0), DEC);
        }
    }

    /* fail_flag contains all possible errors,
        They are defined in  ADC_Module.h as

        ADC_ERROR_OTHER
        ADC_ERROR_CALIB
        ADC_ERROR_WRONG_PIN
        ADC_ERROR_ANALOG_READ
        ADC_ERROR_COMPARISON
        ADC_ERROR_ANALOG_DIFF_READ
        ADC_ERROR_CONT
        ADC_ERROR_CONT_DIFF
        ADC_ERROR_WRONG_ADC
        ADC_ERROR_SYNCH

        You can compare the value of the flag with those masks to know what's the error.
    */

    #if defined(ADC_TEENSY_3_1)
    if(adc->adc1->fail_flag) {
        //Serial.print("ADC1 error flags: 0x");
        //Serial.println(adc->adc1->fail_flag, HEX);
    }
    #endif

    

    delay(100);

}

#if defined(ADC_TEENSY_3_1)
void adc1_isr(void) {
    result =  adc->readSynchronizedContinuous();
    int left = result.result_adc0;
    int right= result.result_adc0;

    if (left<0x200){
      analogWrite(OUTLP, 0);
      analogWrite(OUTLN, 0x200 - left);
    } else{
      analogWrite(OUTLN, 0);
      analogWrite(OUTLP, left-0x200);
    }
    if (right<0x200){
      analogWrite(OUTRP, 0);
      analogWrite(OUTRN, 0x200 - right);
    } else{
      analogWrite(OUTRN, 0);
      analogWrite(OUTRP, right-0x200);
    }

    //test sample rate
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
    
}
#endif

// RESULTS OF THE TEST Teesny 3.x
// Measure continuously a voltage divider.
// Measurement pin A9 (23). Clock speed 96 Mhz, bus speed 48 MHz.

//
//  Using ADC_LOW_SPEED (same as ADC_VERY_LOW_SPEED) for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits            81 kHz                               1
//     12  bits            94 kHz                               1
//     10  bits            94 kHz                               1
//      8  bits           103 kHz                               1

//     16  bits             2.5 kHz                              32
//     12  bits             2.9 kHz                              32
//     10  bits             2.9 kHz                              32
//      8  bits             3.2 kHz                              32

//
//  Using ADC_MED_SPEED for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           193 kHz                               1
//     12  bits           231 kHz                               1
//     10  bits           231 kHz                               1
//      8  bits           261 kHz                               1

//     10  bits            58 kHz                               4 (default settings) corresponds to about 17.24 us

//
//  Using ADC_HIGH_SPEED (same as ADC_HIGH_SPEED_16BITS) for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           414 kHz                               1
//     12  bits           500 kHz                               1
//     10  bits           500 kHz                               1
//      8  bits           571 kHz                               1
//
//      8  bits           308 kHz                               1 ADC_VERY_LOW_SPEED sampling
//      8  bits           387 kHz                               1 ADC_LOW_SPEED sampling
//      8  bits           480 kHz                               1 ADC_MED_SPEED sampling
//      8  bits           632 kHz                               1 ADC_VERY_HIGH_SPEED sampling

//
//  Using ADC_VERY_HIGH_SPEED for sampling and conversion speeds
//  This conversion speed is over the limit of the specs! (speed=24MHz, limit = 18 MHz for res<16 and 12 for res=16)
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           888 kHz                               1
//     12  bits          1090 kHz                               1
//     10  bits          1090 kHz                               1
//      8  bits          1262 kHz                               1


// At 96 Mhz (bus at 48 MHz), 632 KHz is the fastest we can do within the specs, and only if the sample's impedance is low enough.


// RESULTS OF THE TEST Teensy LC
// Measure continuously a voltage divider.
// Measurement pin A9 (23). Clock speed 48 Mhz, bus speed 24 MHz.

//
//  Using ADC_VERY_LOW_SPEED for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits            33.3 kHz                               1
//     12  bits            37.5 kHz                               1
//     10  bits            37.5 kHz                               1
//      8  bits            40.5 kHz                               1

//     16  bits             1.04 kHz                             32
//     12  bits             1.2 kHz                              32
//     10  bits             1.2 kHz                              32
//      8  bits             1.3 kHz                              32

//
//  ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED and ADC_VERY_HIGH_SPEED are the same for Teensy 3.x and LC,
//  except for a very small ammount that depends on the bus speed and not on the ADC clock (which is the same for those speeds).
//  This difference corresponds to 5 bus clock cycles, which is about 0.1 us.
//
//  For 8 bits resolution, 1 average, ADC_MED_SPEED sampling speed the measurement frequencies for the different ADACK are:
//  ADC_ADACK_2_4       106.8 kHz
//  ADC_ADACK_4_0       162.6 kHz
//  ADC_ADACK_5_2       235.1 kHz
//  ADC_ADACK_6_2       263.3 kHz
//  For Teensy 3.x the results are similar but not identical for two reasons: the bus clock plays a small role in the total time and
//  the frequency of this ADACK clock is acually quite variable, the values are the typical ones, but in the electrical datasheet
//  it says that they can range from +-50% their values aproximately, so every Teensy can have different frequencies.
