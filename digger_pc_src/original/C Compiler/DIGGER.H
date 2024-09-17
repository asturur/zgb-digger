/* The following values produce a screen with 10 horizontal and 15 vertical positions 
   x_last = 280+x_offset 
   x_first= x_offset
   y_last = 162+y_offset
   y_first= y_offset	*/
#define		X_LAST		292
#define		X_FIRST		12
#define		Y_LAST		180
#define		Y_FIRST		18
#define		X_OFFSET	12
#define		Y_OFFSET	18
#define		H_MAX		10
#define		W_MAX		15

/* direction definitions */
#define	LEFT		4
#define	RIGHT		0
#define	UP		2
#define	DOWN		6
#define	STOP		(0-1)
#define	FALL_1	10
#define	FALL_2	11
#define	FALL_3	12
#define	FALL_4	13
#define	FALL_5	14
#define	FALL_9	15

/* mans starting position */
#define	M_X_START	7
#define	M_Y_START	9

/* screen color values */
#define BLACK		0
#define GREEN		1
#define	RED		2
#define	BROWN		3
#define	CYAN		1
#define	MAGENTA		2
#define	WHITE		3

/* death type definitions */
#define	MONEY_BAG	1
#define	MAN		2
#define	BUG		3
#define	DEAD		4
#define	BOUNCE		5

/* sprite type definitions */
#define	BAG_0		0
#define	BAG_4		1
#define	BAG_5		2
#define	BAG_6		3
#define	BAG_7		4
#define	BAG_8		5
#define	BAG_9		6

/* bug sprite types */
#define	ADVANCING	0
#define	NORMAL		1


#define	FALSE		0
#define TRUE		(!0)

/* define to turn on IBM or Hyperion options */
/* #define	HYP		1 */
#define	IBM		1

/* turn on debug - developement routines */
/* #define	DEBUG		1 */

extern	char	keychr, tick;
extern 	int	pdir, ldir, keydir, lkdir, fire, fires, endgame, sound, music;
extern 	int	defseg, joyx, joyy, butt1, butt2, sound2, sn2on, per2;
