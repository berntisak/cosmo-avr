#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"

static uint16_t results[8];
static uint8_t dirty = 0;
ISR(ADC_vect) {
  static uint8_t adc = 0;
  results[(adc-1) & 7] = ADC;
  adc = (adc + 1) & 7;
  ADMUX = (1<<REFS0) | adc; // Current transaction have already started.
  dirty |= (1<<adc);
}

void adc_init(void) {
  DIDR0 = 0xff;
  ADMUX = (1<<REFS0);
  ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (1<<ADIE) | (7<<ADPS0);
}

uint16_t adc_read(uint8_t mux) {
  mux &= 7;
  uint8_t mask = 1<<mux;
  while((dirty & mask) == 0);
  uint16_t ret = results[mux & 7];
  cli();
  dirty &= ~mask;
  sei();
  return ret;
}
