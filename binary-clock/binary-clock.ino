/*
 *	Author: Alexander Skafte
 *	License: None
 */

/* TODO
	- Buy and use the Real Time Clock module.
	- Use the faster option to digitalWrite (i.e. writing to
	  the ports directly).
*/

#include <Time.h>

/* Take note! Using Serial.Print will make the light shine less brightly.*/
#define DEBUG(var)				\
	do {					\
		Serial.print(#var " = ");	\
		Serial.print(var);		\
		Serial.print("\n");		\
	} while (0);

#undef DEBUG
#define DEBUG(var) { }

#define NOW_H	22
#define NOW_M	9
#define NOW_S	3

#define A		(2)
#define B		(3)
#define C		(4)
#define D		(5)
#define E		(6)

#define NUM_PINS	(5)
#define NUM_ROWS	(3)
#define NUM_COLS	(6)

int connections[3][6][2] = {
	{ {B, A}, {C, A}, {D, A}, {E, A}, {A, B}, {C, B}, },
	{ {D, B}, {E, B}, {A, C}, {B, C}, {D, C}, {E, C}, },
	{ {A, D}, {B, D}, {C, D}, {E, D}, {A, E}, {B, E}, },
};

void light(int connection[2])
{
	pinMode(connection[0], OUTPUT);
	digitalWrite(connection[0], HIGH);
	pinMode(connection[1], OUTPUT);
	digitalWrite(connection[1], LOW);
//	delay(50);
	pinMode(connection[0], INPUT);
	pinMode(connection[1], INPUT);
}

char* dec_to_bin(char bin[6], int dec, int num_bits)
{
	int i;
	bin[num_bits] = '\0';
	for (i = num_bits - 1; i >= 0; --i, dec >>= 1) {
		bin[i] = (dec & 1) + '0';
	}
	return bin;
}

struct Time {
	int	hour;
	int	minute;
	int	second;
};

struct Time get_time(unsigned long s)
{
	struct Time	t;
	t.hour		= (s / (60 * 60)) % 24;
	t.minute	= (s / 60) % 60;
	t.second	= s % 60;
	return t;
}

unsigned long hmstos(unsigned long h,
		     unsigned long m,
		     unsigned long s)
{
	return h * 60 * 60 + m * 60 + s;
}

unsigned long current_time = hmstos(NOW_H, NOW_M, NOW_S);

void setup()
{
	Serial.begin(9600);
	pinMode(A, INPUT);
	pinMode(B, INPUT);
	pinMode(C, INPUT);
	pinMode(D, INPUT);
	pinMode(E, INPUT);
	DEBUG(current_time);
}

void display_time(int row, char bin[NUM_COLS], int time)
{
	dec_to_bin(bin, time, NUM_COLS);
	for (int col = 0; col < NUM_COLS; ++col) {
		if (bin[col] == '1') {
			light(connections[row][col]);
		}
	}
}

void loop()   
{
	struct Time	time	= get_time(current_time + now());
	int		h	= time.hour;
	int		m	= time.minute;
	int		s	= time.second;

	/* Buffer to hold hour/minute/second in binary form */
	char bin[6];

	display_time(0, bin, h);	/* Display hours on LED 1-6	*/
	display_time(1, bin, m);	/* Display minutes on LED 7-12	*/
	display_time(2, bin, s);	/* Display seconds on LED 13-18	*/
}

