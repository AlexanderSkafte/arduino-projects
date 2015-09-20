
/*
	Author: Alexander Skafte
	License: None
*/

#include <Time.h>

// #define     fast dig write
//PORT{letter} |= _BV(P{letter}{number});
//PORT{letter} &= ~_BV(P{letter}{number});

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

void setup()
{
	Serial.begin(9600);
	pinMode(A, INPUT);
	pinMode(B, INPUT);
	pinMode(C, INPUT);
	pinMode(D, INPUT);
	pinMode(E, INPUT);
}

struct Time {
	int	hour;
	int	minute;
	int	second;
};

struct Time get_time(unsigned long s)
{

	struct Time	t;
	t.second	= s % 60;
	t.minute	= (s / 60) % 60;
	t.hour		= (s / (60 * 60)) % 24;
	return t;
}

unsigned long hmstos(unsigned long h,
		     unsigned long m,
		     unsigned long s)
{
	return (h * 60 * 60 + m * 60 + s);
}

#define NOW_H	23
#define NOW_M	59
#define NOW_S	55

void loop()   
{
//	time_t	t = now();
//	int	h = (00 + hour(t))   % 24;
//	int	m = (00 + minute(t)) % 60;
//	int	s = (32 + 16 + 8 + second(t)) % 60;

	struct Time time =
		get_time( hmstos(NOW_H, NOW_M, NOW_S) + second(now()) );

	int	s = time.second;
	int	m = time.minute;
	int	h = time.hour;

	/* Buffer to hold hour/minute/second in binary form */
	char bin[6];

	/* Display hour */
	dec_to_bin(bin, h, 6);
	for (int i = 0; i < NUM_COLS; ++i) {
		if (bin[i] == '1') {
			light(connections[0][i]);
		}
	}

	/* Display minute */
	dec_to_bin(bin, m, 6);
	for (int i = 0; i < NUM_COLS; ++i) {
		if (bin[i] == '1') {
			light(connections[1][i]);
		}
	}

	/* Display second */
	dec_to_bin(bin, s, 6);
	for (int i = 0; i < NUM_COLS; ++i) {
		if (bin[i] == '1') {
			light(connections[2][i]);
		}
	}
}



//void show_number(int n, int* row)
//{
//	char bin[6];
//	dec_to_bin(bin, n, 6);
//
//	for (int led = 0; led < 6; ++led) {
//		light(row[led]);
//	}
//}

//int connections[5][4][2] = {
//	{ {A,B}, {A,C}, {A,D}, {A,E} },
//	{ {B,A}, {B,C}, {B,D}, {B,E} },
//	{ {C,A}, {A,B}, {C,D}, {C,E} },
//	{ {D,A}, {D,B}, {D,C}, {D,E} },
//	{ {E,A}, {E,B}, {E,C}, {E,D} }
//};

//int connections[5][4][2] = {
//	{ {B, A}, {C, A}, {D, A}, {E, A} },	// A
//	{ {A, B}, {C, B}, {D, B}, {E, B} },	// B
//	{ {A, C}, {B, C}, {D, C}, {E, C} },	// C
//	{ {A, D}, {B, D}, {C, D}, {E, D} },	// D
//	{ {A, E}, {B, E}, {C, E}, {D, E} },	// E
//};

