/*
-------------------------------------------------------------------------------

	==========  C Text Routine / Function Library  ==========

Last Changed:	July 17, 1983

		- rnd destroyed defseg


Description:

	This module contains routines for the placement of text and score.


Routines Supported:

----------  Print String ----------

	prints(string,x,y,color)
	int	x, y, color;
	char	*string;

		*string	- pointer to string to be printed
		x,y	- starting x,y coordinates
		color	- color of text (0 - 3)

Description:

	The text string passed will be placed on the screen starting at
	location x,y. Each character will be placed 12 pixels past the
	previous one. The text string will be plotted on the color
	specified. Valid characters are A - Z . _ and space.



----------  Print Character  ----------

	printc(chr,x,y,color)
	char	chr;
	int	x, y, color;

		chr	- ASCII character to print (A - Z)
		x,y	- starting x,y coordinates
		color	- color of character

Description:

	The character passed will be plotted at location x,y in the color
	specified. Only the letters A through Z will be printed.



----------  Print Integer Number  ----------

	printn(number,x,y,length,color)
	int	number, x, y, color, length;

		number	- integer to print
		x,y	- starting x,y coordinates
		length	- maximum number of digits
		color	- color of number (0 - 3)

Description:

	The specified number will be printed starting at location x,y. If
	the first digit is a 0, a space will be printed instead.



----------  Print Long Integer Number  ----------

	printnl(number,x,y,length,color)
	long	number;
	int	x, y, color, length;

		number	- long integer to print
		x,y	- starting x,y coordinates
		length	- maximum number of digits
		color	- color of number (0 - 3)

Description:

	The specifed long integer will be printed starting at location x,y.
	If the first digit is a 0, a space will be printed instead.



----------  Print Integer Digit  ----------

	printd(digit,x,y,color)
	int	digit, x, y, color;

		digit	- digit to print
		x,y	- location of digit
		color	- color of digit
Description:

	The number passed will be printed at location x,y.



----------  Return a Random Number  ----------

	n = rnd(range);
	int	range;

		range	- maximum number that rnd is to return (max 255)

Description:

	A random number between 0 and range will be returned. Range must
	not be above 255.



----------  Read Timer Counter  ----------

	n = timer();

Description:

	The counter timer location is read. It will return a value between
	0 and 255.


-------------------------------------------------------------------------------
*/

#define FALSE 0
#define TRUE  -1

#include <c:char__a.chr>	/*  include character image files  */
#include <c:char__b.chr>
#include <c:char__c.chr>
#include <c:char__d.chr>
#include <c:char__e.chr>
#include <c:char__f.chr>
#include <c:char__g.chr>
#include <c:char__h.chr>
#include <c:char__i.chr>
#include <c:char__j.chr>
#include <c:char__k.chr>
#include <c:char__l.chr>
#include <c:char__m.chr>
#include <c:char__n.chr>
#include <c:char__o.chr>
#include <c:char__p.chr>
#include <c:char__q.chr>
#include <c:char__r.chr>
#include <c:char__s.chr>
#include <c:char__t.chr>
#include <c:char__u.chr>
#include <c:char__v.chr>
#include <c:char__w.chr>
#include <c:char__x.chr>
#include <c:char__y.chr>
#include <c:char__z.chr>
#include <c:char__0.chr>
#include <c:char__1.chr>
#include <c:char__2.chr>
#include <c:char__3.chr>
#include <c:char__4.chr>
#include <c:char__5.chr>
#include <c:char__6.chr>
#include <c:char__7.chr>
#include <c:char__8.chr>
#include <c:char__9.chr>
#include <c:char__pe.chr>
#include <c:char__ul.chr>
#include <c:char__sp.chr>

/*  random number table  */

	char	rnd_table[] = {
		 74,130,197,120, 34,249,121,216,177,118, 72,228, 62,141,149,119,
		212,  3,106,123, 48, 21, 65,193,  9,138,  1,242,251,129, 60,160,
		 11, 40,152,128, 31,113, 43,131, 88,182,109,245,114,  2, 93,100,
		 42, 57,  7,219, 27, 54,214,194,236, 99, 18,237, 14, 20,132,155,
		181,124,252,148,168,201, 46,135, 76, 78,189, 47, 90, 81,195, 83,
		 68, 69, 24,183,157,198, 94, 26,192,244,200, 71, 41, 56,139,159,
		 73,227,241, 58, 17,  4,206,125,234, 82,188,215,247, 84,253,190,
		169,105,235, 75,136,204, 87,229, 23, 32, 67,117,175, 33,224,  6,
		185,150, 86, 91, 92,246,161,134,223,165, 13,153,104,103, 52,164,
		179,133,226,187,255,222,239,158, 28,202, 53, 61,112,145, 64, 19,
		180,207,140, 66,107,151,167,146, 38, 12, 49,170,250,143,166, 16,
		205, 51,176,174,231,210, 80, 96,243,163, 22,122,108,137, 30,240,
		 59, 79, 63,173, 50,101, 15,  8, 95, 25, 45,225, 39,218,116,221,
		 10,220,238, 77,186,217,144,154,147,  0,  5,111,230,203,102,184,
		162,208, 29,211, 70,156, 35,178,199, 85, 97,232,172,191, 89, 44,
		213, 36,142,254,248, 37,127,233,110,115,171,209, 55,126, 98,196	};

	int	rnd_seed;	/*  pointer to random number table  */

extern	int	defseg;		/*  data segment pointer for peek  */


/*	<<<<<<<<<<  Print Text String  >>>>>>>>>>	*/

prints(string,x,y,color)
int	x, y, color;
char	*string;
{
	int	n = 0;		/*  string pointer  */

	while (string[n]) {
		printc(string[n++],x,y,color);	/*  print character  */
		x += 12;			/* next character position  */
	}
}


/*	<<<<<<<<<<  Print Character  >>>>>>>>>>		*/

printc(chr,x,y,color)
char	chr;
int	x, y, color;
{
	switch (chr) {
		case 'A': put(x,y,char_a,color); break;
		case 'B': put(x,y,char_b,color); break;
		case 'C': put(x,y,char_c,color); break;
		case 'D': put(x,y,char_d,color); break;
		case 'E': put(x,y,char_e,color); break;
		case 'F': put(x,y,char_f,color); break;
		case 'G': put(x,y,char_g,color); break;
		case 'H': put(x,y,char_h,color); break;
		case 'I': put(x,y,char_i,color); break;
		case 'J': put(x,y,char_j,color); break;
		case 'K': put(x,y,char_k,color); break;
		case 'L': put(x,y,char_l,color); break;
		case 'M': put(x,y,char_m,color); break;
		case 'N': put(x,y,char_n,color); break;
		case 'O': put(x,y,char_o,color); break;
		case 'P': put(x,y,char_p,color); break;
		case 'Q': put(x,y,char_q,color); break;
		case 'R': put(x,y,char_r,color); break;
		case 'S': put(x,y,char_s,color); break;
		case 'T': put(x,y,char_t,color); break;
		case 'U': put(x,y,char_u,color); break;
		case 'V': put(x,y,char_v,color); break;
		case 'W': put(x,y,char_w,color); break;
		case 'X': put(x,y,char_x,color); break;
		case 'Y': put(x,y,char_y,color); break;
		case 'Z': put(x,y,char_z,color); break;
		case '0': put(x,y,char_0,color); break;
		case '1': put(x,y,char_1,color); break;
		case '2': put(x,y,char_2,color); break;
		case '3': put(x,y,char_3,color); break;
		case '4': put(x,y,char_4,color); break;
		case '5': put(x,y,char_5,color); break;
		case '6': put(x,y,char_6,color); break;
		case '7': put(x,y,char_7,color); break;
		case '8': put(x,y,char_8,color); break;
		case '9': put(x,y,char_9,color); break;
		case '.': put(x,y,char_pe,color); break;
		case '_': put(x,y,char_ul,color); break;
		case ' ': put(x,y,char_sp,color); break;

		default:  break;
	}
}


/*	<<<<<<<<<<  Print Integer Number  >>>>>>>>>>	*/

printn(number,x,y,length,color)
int	number, x, y, length, color;
{
	int	num;
	int	x_loc = (length-1) * 12 + x;		/*  location of last digit  */

	for (; length>0; length--, x_loc -= 12) {	/*  loop throught digits  */
		num = number % 10;			/*  get digit  */

		if ((length>1) || num)
			printd(num,x_loc,y,color);	/*  print digit  */

		number /= 10;			/*  shift number right one digit  */
	}
}


/*	<<<<<<<<<<  Print Long Integer Number  >>>>>>>>>>	*/

println(number,x,y,length,color)
long	number;
int	x, y, length, color;
{
	int	num;
	int	x_loc = (length-1) * 12 + x;		/*  location of last digit  */

	for (; length>0; length--, x_loc -= 12) {	/*  loop throught digits  */
		num = number % 10L;			/*  get digit  */

		if ((length>1) || num)
			printd(num,x_loc,y,color);	/*  print digit  */

		number /= 10L;			/*  shift number right one digit  */
	}
}


/*	<<<<<<<<<<  Print Integer Digit  >>>>>>>>>>	*/

printd(digit,x,y,color)
int	digit, x, y, color;
{
	switch (digit) {
		case 0:	put(x,y,char_0,color); break;
		case 1:	put(x,y,char_1,color); break;
		case 2:	put(x,y,char_2,color); break;
		case 3:	put(x,y,char_3,color); break;
		case 4:	put(x,y,char_4,color); break;
		case 5:	put(x,y,char_5,color); break;
		case 6:	put(x,y,char_6,color); break;
		case 7:	put(x,y,char_7,color); break;
		case 8:	put(x,y,char_8,color); break;
		case 9:	put(x,y,char_9,color); break;

		default: break;
	}
}


/*	<<<<<<<<<<  Return a Random Number  >>>>>>>>>>		*/

rnd(range)
int	range;
{
	/*  add timer count for auto seeding  */

	int	n = (++rnd_seed + timer()) & 255;

	return(((rnd_table[n] * ++range) >> 8) & 255);
}


/*	<<<<<<<<<<  Read Timer Counter  >>>>>>>>>>	*/

timer()
{
	int	a, temp;

	temp=defseg;	/* save old defseg value */
	defseg = 0x40;		/*  set data segment to bios data area  */
	a=peek(0x6c);
	defseg=temp;	/* restore origional defseg */
	return(a);
}
