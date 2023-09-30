/*
 * project.c
 *
 *  Created on: Sep 15, 2023
 *      Author: Amira
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define TOP 976 //to make sure that the calculated time is 1 second as we use prescaling 1024

unsigned char num0 = 0;
unsigned char num1 = 0;
unsigned char num2 = 0;
unsigned char num3 = 0;
unsigned char num4 = 0;
unsigned char num5 = 0;

unsigned char flag = 0;//for the timer

ISR(INT0_vect)
{
	num0 = 0;
	num1 = 0;
	num2 = 0;
	num3 = 0;
	num4 = 0;
	num5 = 0;
}
ISR(INT1_vect)
{
	TCCR1B &= ~(1 << CS10) & ~(1 << CS11) & ~(1 << CS12);
}

ISR(INT2_vect)
{
	TCCR1B |= (1 << CS10) | (1 << CS12);//clock is back
}


void Timer1_Init_CTC_Mode(void)
{
	TCNT1H = 0;
	TCNT1L = 0; //initial value for the timer
	OCR1A = TOP;

	TIMSK = (1 << OCIE1A); //Timer1 Interrupt Enable
	TCCR1A = (1 << FOC1A) | (1 << FOC1B);//for non-PWM modes
	TCCR1B = (1 << CS10) | (1 << WGM12) | (1 << CS12); //for prescaling /1024
}


void INT0_Init(void)
{
	MCUCR &= ~(1<<ISC00);
	MCUCR |= (1<<ISC01);//falling edge
	GICR  |= (1<<INT0);
}

void INT1_Init(void)
{
	MCUCR |= (1<<ISC10) | (1<<ISC11);//raising edge
	GICR  |= (1<<INT1);
}

void INT2_Init(void)
{
	MCUCSR &= ~(1 << ISC2);//falling edge
	GICR  |= (1<<INT2);
}

ISR(TIMER1_COMPA_vect)
{
	flag = 1;
}


int main(void)
{
	SREG  |= (1<<7);              // Enable global interrupts in MC.
	DDRC  |= 0x0F;               // Configure the first four pins in PORTC as output pins.
	DDRD  &= ~(1 << 2) & ~(1 << 3); //PD2 and PD3 are input pins(2 push buttons)
	DDRB  &= ~(1 << 2);  //PB2 is input pin(another push button)
	PORTB |= (1 << 2);//to activate the internal pull-up resistor for INT2.
	PORTD |= (1 << 2);//to activate the internal pull-up resistor for INT0.
	DDRA  |= 0x3F;     // Configure the first six pins in PORTA as output pins (the six seven segment enables).
	PORTA |= 0x3F;    //initialize the enables by 1.
	PORTC &= 0xF0;


	// activate external interrupt INT0
	INT0_Init();

	// activate external interrupt INT1
	INT1_Init();

	// activate external interrupt INT2
	INT2_Init();

	//activate the timer
	Timer1_Init_CTC_Mode();

	while (1)
	{
		if (flag == 1)
		{
			switch (num0)
			{
			case 9:
				num0 = 0;
				switch (num1)
				{
				case 5:
					num1 = 0;
					switch (num2)
					{
					case 9:
						num2 = 0;
						switch (num3)
						{
						case 5:
							num3 = 0;
							switch (num4)
							{
							case 9:
								num4 = 0;
								num5++;
								break;
							default:
								num4++;
								break;
							}
							break;
						default:
							num3++;
							break;
						}
						break;
					default:
						num2++;
						break;
					}
					break;
				default:
					num1++;
					break;
				}
				break;
			default:
				num0++;
				break;
			}
			flag = 0;
		}
		else
		{
				PORTA = (PORTA & 0xC0) | 0x01; // Enable the first digit, disable others
				PORTC = (PORTC & 0xF0) | (num0 & 0x0F);
				_delay_us(5);

				PORTA = (PORTA & 0xC0) | 0x02; // Enable the second digit, disable others
				PORTC = (PORTC & 0xF0) | (num1 & 0x0F);
				_delay_us(5);

				PORTA = (PORTA & 0xC0) | 0x04;
				PORTC = (PORTC & 0xF0) | (num2 & 0x0F);
				_delay_us(5);

				PORTA = (PORTA & 0xC0) | 0x08;
				PORTC = (PORTC & 0xF0) | (num3 & 0x0F);
				_delay_us(5);

				PORTA = (PORTA & 0xC0) | 0x10;
				PORTC = (PORTC & 0xF0) | (num4 & 0x0F);
				_delay_us(5);

				PORTA = (PORTA & 0xC0) | 0x20;
				PORTC = (PORTC & 0xF0) | (num5 & 0x0F);
				_delay_us(5);
		}
	}


}


