#include <SerialFlash.h>
#include <output_pwm.h>
#include <synth_waveform.h>
#include <output_i2s.h>
#include <output_spdif.h>
#include <synth_tonesweep.h>
#include <record_queue.h>
#include <play_memory.h>
#include <analyze_tonedetect.h>
#include <effect_bitcrusher.h>
#include <play_queue.h>
#include <effect_delay_ext.h>
#include <analyze_print.h>
#include <analyze_fft1024.h>
#include <effect_flange.h>
#include <filter_fir.h>
#include <output_dac.h>
#include <filter_variable.h>
#include <effect_envelope.h>
#include <control_sgtl5000.h>
#include <play_sd_wav.h>
#include <synth_dc.h>
#include <Audio.h>
#include <effect_fade.h>
#include <control_wm8731.h>
#include <mixer.h>
#include <filter_biquad.h>
#include <effect_delay.h>
#include <analyze_peak.h>
#include <spi_interrupt.h>
#include <input_i2s.h>
#include <play_serialflash_raw.h>
#include <effect_multiply.h>
#include <synth_whitenoise.h>
#include <synth_sine.h>
#include <synth_pinknoise.h>
#include <effect_chorus.h>
#include <input_adc.h>
#include <analyze_fft256.h>
#include <play_sd_raw.h>
#include <AudioControl.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioInputAnalog         adc2;           //xy=66,325
AudioInputAnalog         adc1;           //xy=70,277
AudioOutputPWM           pwm2;           //xy=265,326
AudioOutputPWM           pwm1;           //xy=273,274
AudioConnection          patchCord1(adc2, pwm2);
AudioConnection          patchCord2(adc1, pwm1);
// GUItool: end automatically generated code

void setup()
{
  AudioMemory(12);
}

void loop()
{
}



