/* ========================================================================== *\
 *	Author: Alexander Skafte
 *	License: None
\* ========================================================================== */

/* TODO
	- Use the faster option to digitalWrite (write to the ports directly).
	- Implement code for using a button to swtich between hour/minute/second
	  to day/month/year.
*/

#include <Time.h>
#include <Wire.h>


/* ========================================================================== *\
	MACROS AND CONSTANTS
\* ========================================================================== */

/* Take note! Using Serial.print will:
	- Make the lights shine less brightly.
	- Freeze when the ATmega328P-PU is not connected with serial to the
	  computer.
*/
#define DEBUG(var)				\
	do {					\
		Serial.print(#var " = ");	\
		Serial.println(var);		\
	} while (0);
#undef DEBUG
#define DEBUG(var) { }

/* Change to set the RTC module's internal time to the values below */
#if 0
	#define SET_RTC_DS3231_TIME
	#define SECOND		04
	#define MINUTE		41
	#define HOUR		4
	#define DAY_OF_WEEK	1
	#define DAY_OF_MONTH	28
	#define MONTH		9
	#define YEAR		15
#endif


/* Output pins used on the ATmega328P-PU */
#define A		9
#define B		10
#define C		11
#define D		12
#define E		13

#define NUM_PINS	5
#define NUM_ROWS	3
#define NUM_COLS	6

/* Define all possible HIGH/LOW combinations in the circuit. */
int connections[NUM_ROWS][NUM_COLS][2] = {
	{ {B, A}, {C, A}, {D, A}, {E, A}, {A, B}, {C, B}, },
	{ {D, B}, {E, B}, {A, C}, {B, C}, {D, C}, {E, C}, },
	{ {A, D}, {B, D}, {C, D}, {E, D}, {A, E}, {B, E}, },
};


char*		dec_to_bin	(char bin[6], int dec, int num_bits);
byte		to_bcd		(byte n);
byte		from_bcd	(byte n);

void		set_DS3231_time	(byte second, byte minute, byte hour,
				 byte day_of_week, byte day_of_month,
				 byte month, byte year);

void		read_DS3231_time(byte* second, byte* minute, byte* hour,
				 byte* day_of_week, byte* day_of_month,
				 byte* month, byte* year);

void		display_time	(int row, char bin[NUM_COLS], int time);
void		light		(int connection[2]);


/* ========================================================================== *\
	Setup and Loop Functions
\* ========================================================================== */

void setup()
{
	Serial.begin(9600);
	pinMode(A, INPUT);
	pinMode(B, INPUT);
	pinMode(C, INPUT);
	pinMode(D, INPUT);
	pinMode(E, INPUT);

	Wire.begin();

#ifdef SET_RTC_DS3231_TIME
	set_DS3231_time(SECOND, MINUTE, HOUR, DAY_OF_WEEK,
			DAY_OF_WEEK, MONTH, YEAR);
#endif
}

void loop()
{
	byte second, minute, hour, day_of_week, day_of_month, month, year;

	read_DS3231_time(&second,	&minute,	&hour,
			 &day_of_week,	&day_of_month,
			 &month,	&year);

	int h = hour;
	int m = minute;
	int s = second;

	/* Buffer to hold hour/minute/second in binary form */
	char bin[6];

	display_time(0, bin, h);	/* Display hours on LED 1-6	*/
	display_time(1, bin, m);	/* Display minutes on LED 7-12	*/
	display_time(2, bin, s);	/* Display seconds on LED 13-18	*/
}


/* ========================================================================== *\
	Utility Functions
\* ========================================================================== */

char* dec_to_bin(char bin[6], int dec, int num_bits)
{
	int i;
	bin[num_bits] = '\0';
	for (i = num_bits - 1; i >= 0; --i, dec >>= 1) {
		bin[i] = (dec & 1) + '0';
	}
	return bin;
}

byte to_bcd(byte n)
{
	return (16 * (n / 10)) + (n % 10);
}

byte from_bcd(byte n)
{
	return (10 * (n / 16)) + (n % 16);
}


/* ========================================================================== *\
	RTC Module (DS3231) Interfacing
\* ========================================================================== */

#define DS3231_I2C_ADDRESS	0x68

void set_DS3231_time(byte second, byte minute, byte hour,
		     byte day_of_week, byte day_of_month,
		     byte month, byte year)
{
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0);
	Wire.write(to_bcd(second));
	Wire.write(to_bcd(minute));
	Wire.write(to_bcd(hour));
	Wire.write(to_bcd(day_of_week));
	Wire.write(to_bcd(day_of_month));
	Wire.write(to_bcd(month));
	Wire.write(to_bcd(year));
	Wire.endTransmission();
}

void read_DS3231_time(byte* second, byte* minute, byte* hour,
		      byte* day_of_week, byte* day_of_month,
		      byte* month, byte* year)
{
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0);
	Wire.endTransmission();
	Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
	*second		= from_bcd(Wire.read() & 0x7f);
	*minute		= from_bcd(Wire.read());
	*hour		= from_bcd(Wire.read() & 0x3f);
	*day_of_week	= from_bcd(Wire.read());
	*day_of_month	= from_bcd(Wire.read());
	*month		= from_bcd(Wire.read());
	*year		= from_bcd(Wire.read());
}


/* ========================================================================== *\
	Circuit Control
\* ========================================================================== */

void display_time(int row, char bin[NUM_COLS], int time)
{
	dec_to_bin(bin, time, NUM_COLS);
	for (int col = 0; col < NUM_COLS; ++col) {
		if (bin[col] == '1') {
			light(connections[row][col]);
		}
	}
}

void light(int connection[2])
{
	pinMode(connection[0], OUTPUT);
	digitalWrite(connection[0], HIGH);
	pinMode(connection[1], OUTPUT);
	digitalWrite(connection[1], LOW);
//	delay(300);
	pinMode(connection[0], INPUT);
	pinMode(connection[1], INPUT);
}

