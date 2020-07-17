/*
 * Project4.c
 *
 * Created: 6/2/2020 12:32:41 PM
 * Author : MrTrashCan
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr.h"
#include "lcd.h"

#define MAX_VOLT 5.0
#define MAX_RES 1024.0
#define INT_MAX 1024
#define INT_MIN -1

typedef struct {
	int current;
	int min;
	int max;
	long total;
	int count;
} readings;

int get_sample();
void update_reading(readings *r);
void display(readings *r);
void resetReading(readings *r);
void displayCurrent(readings *r);
int get_key();
int is_pressed(int r, int c);

int main(void)
{
    /* Replace with your application code */
	lcd_init();
	ADMUX = 0b01000000;
	ADCSRA = 0b10000000;
	readings reading = {0,INT_MAX,INT_MIN,0,0};
	char characters[] = "X123A456B789C*0#D";
	int running = 0;
	avr_wait(10);
	resetReading(&reading);
    while (1) 
    {
		int key = get_key();
		if(characters[key] == 'A')
			running = 1;
		else if(characters[key] == 'B')
		{
			resetReading(&reading);
			running = 0;
		}
		reading.current = get_sample();
		if(running)
		{
			update_reading(&reading);
			display(&reading);
			avr_wait(500);
		}
		else
		{
			displayCurrent(&reading);
			avr_wait(500);
		}
    }
}

int get_sample()
{
	//ADMUX = 0b01000000;
	//ADCSRA = 0b11000000;
	SET_BIT(ADCSRA,ADSC);
	while(!GET_BIT(ADCSRA,ADSC));
	return ADC;
}

void update_reading(readings *r)
{
	int s = r->current;
	if(r->max < s)
		r->max = s;
	if(r->min > s)
		r->min = s;
	r->total += s;
	r->count++;
}

void display(readings *r)
{
	char line[17];
	sprintf(line, "CUR:%4.2fAVR:%4.2f", (float)r->current / MAX_RES * MAX_VOLT, ((float)r->total/r->count) / MAX_RES * MAX_VOLT);
	//sprintf(line, "CUR:%04dAVR:%04d", r->current, r->total/r->count);
	lcd_pos(0,0);
	lcd_puts(line);
	sprintf(line, "MAX:%4.2fMin:%4.2f", (float)r->max / MAX_RES * MAX_VOLT, (float)r->min / MAX_RES * MAX_VOLT);
	//sprintf(line, "MAX:%04dMin:%04d", r->max, r->min);
	lcd_pos(1,0);
	lcd_puts(line);
}

void resetReading(readings *r)
{
	lcd_pos(0,0);
	lcd_puts("CUR:----AVR:----");
	lcd_pos(1,0);
	lcd_puts("MAX:----Min:----");
	r->count = 0;
	r->max = INT_MIN;
	r->min = INT_MAX;
	r->total = 0;
}

void displayCurrent(readings *r)
{
	char line[9];
	sprintf(line, "CUR:%4.2f", (float)r->current / MAX_RES * MAX_VOLT);
	lcd_pos(0,0);
	lcd_puts(line);
}

int get_key()
{
	for(int r = 0; r < 4; ++r)
	for(int c = 0; c < 4; ++c)
	if(is_pressed(r,c+4))
		return r*4+c+1;
	return 0;
}

int is_pressed(int r, int c)
{
	DDRC = 0b00000000;
	PORTC = 0b00000000;
	SET_BIT(DDRC, r);
	SET_BIT(PORTC, c);
	avr_wait(1);
	if (!GET_BIT(PINC, c))
		return 1;
	return 0;
}