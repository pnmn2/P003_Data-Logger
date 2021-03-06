#include "TimerOne.h"

#define SAMPLING_FREQUENCY          100                                 // Sampling frequency in Hz.
#define SAMPLING_PERIOD             1000000 * 1 / SAMPLING_FREQUENCY    // Sampling period in microseconds.
#define SAMPLING_NUMBER_OF_SAMPLES  10                                  // Number of samples to be averaged.
#define SAMPLING_MARGIN             5                                   // A blink will be captured if a change in the sampling reading larger than this margin is detected.    
#define TIME_BETWEEN_BLINKS         1000                                // Minimum time between blinks captures in miliseconds.

#define SAMPLING_INPUT    A3  // ADC sampling input pin.
#define BUZZER_OUTPUT     8   // Buzzer output pin.

void ISR_Timer1 ();

int sampling_results_index {0};
int sampling_results_total {0};
int sampling_results_average {0};
int sampling_results_threshold {0};
int sampling_results[SAMPLING_NUMBER_OF_SAMPLES] {};

int state_eye_open {true};
unsigned long sampling_last_time_ms {0};

void setup()
{
  // Print program start message.
  Serial.begin(9600);
  //Serial.println("Successful, program start!\n");

  // Initialize digital pin for buzzer.
  pinMode(BUZZER_OUTPUT, OUTPUT);
  digitalWrite(BUZZER_OUTPUT, LOW);
  
  // Initialize timer one.
  Timer1.initialize();
  Timer1.attachInterrupt(ISR_Timer1, SAMPLING_PERIOD);
  delay(1000);
}

void loop()
{

}

void ISR_Timer1 ()
{
  //Serial.println(analogRead(SAMPLING_INPUT));

  int sampling_result = analogRead(SAMPLING_INPUT);
  sampling_results[sampling_results_index] = sampling_result;
  sampling_results_total += sampling_result;
  sampling_results_index++;
  if(sampling_results_index == SAMPLING_NUMBER_OF_SAMPLES) {
    sampling_results_index = 0;
  }
  sampling_results_total -= sampling_results[sampling_results_index];
  sampling_results_average = sampling_results_total / SAMPLING_NUMBER_OF_SAMPLES;

  switch (state_eye_open) {
    case true:
      if (sampling_result < sampling_results_threshold) {
        if (millis() - sampling_last_time_ms > TIME_BETWEEN_BLINKS) {
          Serial.println("Blink!!!!!");
          sampling_results_threshold = sampling_results_average + SAMPLING_MARGIN;
        }
        state_eye_open = false;
      }
      else {
        sampling_results_threshold = sampling_results_average - SAMPLING_MARGIN;
      }
      break;
    case false:
      if (sampling_result > sampling_results_threshold) {
        sampling_results_threshold = sampling_results_average - SAMPLING_MARGIN;
        sampling_last_time_ms = millis();
        state_eye_open = true;
      }
      else {
        sampling_results_threshold = sampling_results_average + SAMPLING_MARGIN;
      }
      break;
  }
}
