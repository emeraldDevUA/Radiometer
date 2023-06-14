#include <avr/delay.h>
#include <avr/interrupt.h>

// defining the clock of a chip as 8mHz
#define F_CPU 8000000UL

unsigned int numPattern[10] = {0x3f, 0x6,  0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x7,  0x7f, 0x6f},
digits[4] = {0, 0, 0, 0};

volatile unsigned int counterTemp = 0, counterConst = 0, time = 0, 
				      sensitivity = 250, result = 0, resultOld = 0,
					  limit = 45;
					  
					  
//External interrupts
ISR(INT0_vect) {
  counterConst++;
  counterTemp = 1;
}
//Timer overflow function
ISR(TIMER1_COMPA_vect) {
  if (time >= limit) {
    time = 0;
    counterTemp = 0;
    counterConst = 0;
    result = resultOld;
  } else if (time % 5 == 0 && time >= 15) {
    calculateDigits();
  }
  time++;
}

void drawDigits() {
  for (int i = 0; i < 4; i++) {
    PORTB |= numPattern[digits[i]];

    switch (i) {
      case 0:
        PORTD |= (1 << 5);
        _delay_ms(10);
        PORTD &= ~(1 << 5);
        break;
      case 1:
        PORTD |= (1 << 4);
        _delay_ms(10);
        PORTD &= ~(1 << 4);
        break;
      case 2:
        PORTD |= (1 << 3);
        _delay_ms(10);
        PORTD &= ~(1 << 3);
        break;
      case 3:
        PORTD |= (1 << 6);
        _delay_ms(10);
        PORTD &= ~(1 << 6);
        break;
      default:
        break;
    }
    PORTB &= 0;
  }
}
void beepAndBlink() {
  if (counterTemp == 1) {
    PORTD |= (1 << 1);
    _delay_ms(5);
    PORTD &= ~(1 << 1);
    counterTemp = 0;
  }
}
void calculateDigits() {
  if (time != 0) {
    result = ((counterConst * 3600) / time) / sensitivity;

    if (result >= (int)pow(10, 4)) {
      result /= 1000;
    }
    retValues();
    resultOld = result;
  }
}
void retValues() {
  for (int i = 3; i >= 0; i--) {
    digits[3 - i] = (result % (int)pow(10, i + 1)) / (int)pow(10, i);
  }
}

void setUp(){
  // setting up timer
  TCCR1B |= (1 << WGM12) | (1 << CS12);
  TIMSK |= (1 << OCIE1A);  
  // timer/counter 1 interrupt when overflown enabled
  OCR1AH = 0b01111010;
  OCR1AL = 0b00010010;
  // setting up analogue ports register
  DDRC = 0x00000000;

  // setting up digital registers
  DDRD = 0b11111011;
  DDRB = 0b11111111;
  PORTD = 0b00000000;
  PORTD = 0b00000000;
  
  // setting up interrupts
  
  GICR |= (1 << 6);
  MCUCR |= (1 << 7) | (1 << 6);
  
  //Interrupts enabled
  sei();
}


int main() {
	setUp();
  // 5 4 3 6
	while (1) {
		drawDigits();
		beepAndBlink();
  }
  
}
