/*
***************************************************************************************************
***************************************************************************************************
**                                                                                               **
**                              D a t a    D e f i n i t i o n s                                 **
**                                                                                               **
**                                                                                               **
***************************************************************************************************
***************************************************************************************************
*/

#include "digger.h"


/* ************************************************************************** */
/* Matrix definitions for all levels of play */
/* ************************************************************************** */

char	mat_1[H_MAX][W_MAX] = {	{ 'S',' ',' ',' ','B',' ',' ',' ',' ',' ','H','H','H','H','S'},
				{ 'V',' ',' ','C','C',' ',' ','C',' ',' ','V',' ','B',' ',' '},
				{ 'V','B',' ','C','C',' ',' ','C',' ',' ','V',' ',' ',' ',' '},
				{ 'V',' ',' ','C','C','B',' ','C','B',' ','V',' ','C','C','C'},
				{ 'V',' ',' ','C','C',' ',' ','C',' ',' ','V',' ','C','C','C'},
				{ 'H','H',' ','C','C',' ',' ','C',' ',' ','V',' ','C','C','C'},
				{ ' ','V',' ',' ',' ',' ','B',' ','B',' ','V',' ',' ',' ',' '},
				{ ' ','H','H','H','H',' ',' ',' ',' ',' ','V',' ',' ',' ',' '},
				{ 'C',' ',' ',' ','V',' ',' ',' ',' ',' ','V',' ',' ',' ','C'},
				{ 'C','C',' ',' ','H','H','H','H','H','H','H',' ',' ','C','C'}
			};


char	mat_2[H_MAX][W_MAX] = {	{ 'S','H','H','H','H','H',' ',' ','B',' ','B',' ',' ','H','S'},
				{ ' ','C','C',' ',' ','V',' ',' ',' ',' ',' ',' ',' ','V',' '},
				{ ' ','C','C',' ',' ','V',' ','C','C','C','C','C',' ','V',' '},
				{ 'B','C','C','B',' ','V',' ','C','C','C','C','C',' ','V',' '},
				{ 'C','C','C','C',' ','V',' ',' ',' ',' ',' ',' ',' ','V',' '},
				{ 'C','C','C','C',' ','V',' ','B',' ',' ','H','H','H','H',' '},
				{ ' ','C','C',' ',' ','V',' ','C','C',' ','V',' ',' ',' ',' '},
				{ ' ','B','B',' ',' ','V','C','C','C','C','V',' ','C','C',' '},
				{ 'C',' ',' ',' ',' ','V',' ','C','C',' ','V',' ','C','C',' '},
				{ 'C','C',' ',' ',' ','H','H','H','H','H','H',' ',' ',' ',' '}
			};


char	mat_3[H_MAX][W_MAX] = {	{ 'S','H','H','H','H','B',' ','B',' ','B','H','H','H','H','S'},
				{ 'C','C',' ',' ','V',' ','C',' ','C',' ','V',' ','B','B',' '},
				{ 'C',' ',' ',' ','V',' ','C',' ','C',' ','V',' ','C','C',' '},
				{ ' ','B','B',' ','V',' ','C',' ','C',' ','V','C','C','C','C'},
				{ 'C','C','C','C','V',' ','C',' ','C',' ','V','C','C','C','C'},
				{ 'C','C','C','C','H','H','H','H','H','H','H',' ','C','C',' '},
				{ ' ','C','C',' ',' ','C',' ','V',' ','C',' ',' ','C','C',' '},
				{ ' ','C','C',' ',' ','C',' ','V',' ','C',' ',' ',' ',' ',' '},
				{ 'C',' ',' ',' ',' ','C',' ','V',' ','C',' ',' ',' ',' ','C'},
				{ 'C','C',' ',' ',' ','C',' ','H',' ','C',' ',' ',' ','C','C'}
			};


char	mat_4[H_MAX][W_MAX] = {	{ 'S','H','B','C','C','C','C','B','C','C','C','C','B','H','S'},
				{ 'C','V',' ',' ','C','C','C','C','C','C','C',' ',' ','V','C'},
				{ 'C','H','H','H',' ','C','C','C','C','C',' ','H','H','H','C'},
				{ 'C',' ',' ','V',' ',' ','C','C','C',' ',' ','V',' ',' ','C'},
				{ ' ',' ',' ','H','H','H',' ','C',' ','H','H','H',' ',' ',' '},
				{ ' ',' ','B',' ',' ','V',' ','B',' ','V',' ',' ','B',' ',' '},
				{ ' ',' ','C',' ',' ','V','C','C','C','V',' ',' ','C',' ',' '},
				{ ' ','C','C','C',' ','H','H','H','H','H',' ','C','C','C',' '},
				{ 'C','C','C','C','C',' ','C','V','C',' ','C','C','C','C','C'},
				{ 'C','C','C','C','C',' ','C','H','C',' ','C','C','C','C','C'}
			};


char	mat_5[H_MAX][W_MAX] = {	{ 'S','H','H','H','H','H','H','H','H','H','H','H','H','H','S'},
				{ 'V','B','C','C','C','C','B','V','C','C','C','C','C','C','V'},
				{ 'V','C','C','C','C','C','C','V',' ','C','C','B','C',' ','V'},
				{ 'V',' ','C','C','C','C',' ','V','C','C','B','C','C','C','V'},
				{ 'V','C','C','C','C','C','C','V',' ','C','C','C','C',' ','V'},
				{ 'V',' ','C','C','C','C',' ','V','B','C','C','C','C','C','V'},
				{ 'V','C','C','B','C','C','C','V',' ','C','C','C','C',' ','V'},
				{ 'V',' ','C','C','B','C',' ','V','C','C','C','C','C','C','V'},
				{ 'V','C','C','C','C','C','C','V','C','C','C','C','C','C','V'},
				{ 'H','H','H','H','H','H','H','H','H','H','H','H','H','H','H'}
			};


char	mat_6[H_MAX][W_MAX] = {	{ 'S','H','H','H','H','H','H','H','H','H','H','H','H','H','S'},
				{ 'V','C','B','C','C','V',' ','V',' ','V','C','C','B','C','V'},
				{ 'V','C','C','C',' ','V','B','V','B','V',' ','C','C','C','V'},
				{ 'V','C','C','C','H','H',' ','V',' ','H','H','C','C','C','V'},
				{ 'V','C','C',' ','V',' ','C','V','C',' ','V',' ','C','C','V'},
				{ 'V','C','C','H','H',' ','C','V','C',' ','H','H','C','C','V'},
				{ 'V','C',' ','V',' ','C','C','V','C','C',' ','V',' ','C','V'},
				{ 'V','C','H','H','B','C','C','V','C','C','B','H','H','C','V'},
				{ 'V','C','V','C','C','C','C','V','C','C','C','C','V','C','V'},
				{ 'H','H','H','H','H','H','H','H','H','H','H','H','H','H','H'}
			};


char	mat_7[H_MAX][W_MAX] = {	{ 'S','H','C','C','C','C','C','V','C','C','C','C','C','H','S'},
				{ ' ','V','C','B','C','B','C','V','C','B','C','B','C','V',' '},
				{ 'B','V','C','C','C','C','C','V','C','C','C','C','C','V','B'},
				{ 'C','H','H','C','C','C','C','V','C','C','C','C','H','H','C'},
				{ 'C','C','V',' ','C','C','C','V','C','C','C',' ','V','C','C'},
				{ 'C','C','H','H','H','C','C','V','C','C','H','H','H','C','C'},
				{ 'C','C','C','C','V',' ','C','V','C',' ','V','C','C','C','C'},
				{ 'C','C','C','C','H','H',' ','V',' ','H','H','C','C','C','C'},
				{ 'C','C','C','C','C','V',' ','V',' ','V','C','C','C','C','C'},
				{ 'C','C','C','C','C','H','H','H','H','H','C','C','C','C','C'}
			};


char	mat_8[H_MAX][W_MAX] = {	{ 'H','H','H','H','H','H','H','H','H','H','H','H','H','H','S'},
				{ 'V',' ','C','C','B','C','C','C','C','C','B','C','C',' ','V'},
				{ 'H','H','H','C','C','C','C','B','C','C','C','C','H','H','H'},
				{ 'V','B','V',' ','C','C','C','C','C','C','C',' ','V','B','V'},
				{ 'V','C','H','H','H','C','C','C','C','C','H','H','H','C','V'},
				{ 'V','C','C','B','V',' ','C','C','C',' ','V','B','C','C','V'},
				{ 'V','C','C','C','H','H','H','C','H','H','H','C','C','C','V'},
				{ 'V','C','C','C','C',' ','V',' ','V',' ','C','C','C','C','V'},
				{ 'V','C','C','C','C','C','V',' ','V','C','C','C','C','C','V'},
				{ 'H','H','H','H','H','H','H','H','H','H','H','H','H','H','H'}
			};



get_mat(x,y,lev)
int	x,y,lev;
{

	switch(lev) {
	case 1:
		return(mat_1[y][x]);
	case 2:
		return(mat_2[y][x]);
	case 3:
		return(mat_3[y][x]);
	case 4:
		return(mat_4[y][x]);
	case 5:
		return(mat_5[y][x]);
	case 6:
		return(mat_6[y][x]);
	case 7:
		return(mat_7[y][x]);
	case 8:
		return(mat_8[y][x]);
	}

}



/* *************************************************************************** */
/*	scan code to ascii & ascii to number convertions */
/* *************************************************************************** */

getval(chr)
char	chr;
{

	switch(chr) {
	case '1':
		return(1);
	case '2':
		return(2);
	case '3':
		return(3);
	case '4':
		return(4);
	case '5':
		return(5);
	case '6':
		return(6);
	case '7':
		return(7);
	case '8':
		return(8);
	case '9':
		return(9);
	default:
		return(0);
	}

}



conkey(chr)
char	chr;
{	/* convert the key scan code to the ascii equivelent */

	switch(chr) {
#ifdef	IBM
	case 0x1e:
		return('A');
	case 0x30:
		return('B');
	case 0x2e:
		return('C');
	case 0x20:
		return('D');
	case 0x12:
		return('E');
	case 0x21:
		return('F');
	case 0x22:
		return('G');
	case 0x23:
		return('H');
	case 0x17:
		return('I');
	case 0x24:
		return('J');
	case 0x25:
		return('K');
	case 0x26:
		return('L');
	case 0x32:
		return('M');
	case 0x31:
		return('N');
	case 0x18:
		return('O');
	case 0x19:
		return('P');
	case 0x10:
		return('Q');
	case 0x13:
		return('R');
	case 0x1f:
		return('S');
	case 0x14:
		return('T');
	case 0x16:
		return('U');
	case 0x2f:
		return('V');
	case 0x11:
		return('W');
	case 0x2d:
		return('X');
	case 0x15:
		return('Y');
	case 0x2c:
		return('Z');

	case 0x34:
		return('.');
	case 0x39:
		return(' ');

	case 0x0b:
		return('0');
	case 0x02:
		return('1');
	case 0x03:
		return('2');
	case 0x04:
		return('3');
	case 0x05:
		return('4');
	case 0x06:
		return('5');
	case 0x07:
		return('6');
	case 0x08:
		return('7');
	case 0x09:
		return('8');
	case 0x0a:
		return('9');
#endif
#ifdef	HYP
	case 46:
		return('A');
	case 65:
		return('B');
	case 63:
		return('C');
	case 48:
		return('D');
	case 33:
		return('E');
	case 49:
		return('F');
	case 50:
		return('G');
	case 51:
		return('H');
	case 38:
		return('I');
	case 52:
		return('J');
	case 53:
		return('K');
	case 54:
		return('L');
	case 67:
		return('M');
	case 66:
		return('N');
	case 39:
		return('O');
	case 40:
		return('P');
	case 31:
		return('Q');
	case 34:
		return('R');
	case 47:
		return('S');
	case 35:
		return('T');
	case 37:
		return('U');
	case 64:
		return('V');
	case 32:
		return('W');
	case 62:
		return('X');
	case 36:
		return('Y');
	case 61:
		return('Z');

	case 69:
		return('.');
	case 75:
		return(' ');

	case 24:
		return('0');
	case 15:
		return('1');
	case 16:
		return('2');
	case 17:
		return('3');
	case 18:
		return('4');
	case 19:
		return('5');
	case 20:
		return('6');
	case 21:
		return('7');
	case 22:
		return('8');
	case 23:
		return('9');
#endif
	default:
		return(0);
	}
}


