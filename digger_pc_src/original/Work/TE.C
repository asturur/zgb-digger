
/*
*****************************************************************************************************
*****************************************************************************************************
*****                                                                                           *****
*****                  Task E   -   Keyboard and Joystick interfacing routines                 *****
*****                                                                                           *****
*****************************************************************************************************
*****************************************************************************************************
*/

#include	"digger.h"

int	last_key, joy_x_low, joy_y_low, joy_x_high, joy_y_high, joyuse, button, last_x, last_y;


init_joy()	/* Initialize joysticks */
{
	int	a, loop;

	keydir=STOP;
	lkdir=STOP;
	button=FALSE;

	joyuse=TRUE;	/* check if a joystick is atached */
	for(loop=0; loop<10; loop++) {	/* check 10 times in case of false noise */
		outp(0x201,0);
		if(!(inp(0x201) & 3))	/* check that timer is not already finished */
			joyuse=FALSE;

		a=0;	/* check that timer times out below limit */
		while((inp(0x201)& 3) && a<255)
			a++;
		if(a>250)
			joyuse=FALSE;

	}

}



joystk()
{	/* update variables related to joystick, buttons & keyboard */
	int	loop;

	last_key=lkdir;
	if(keydir!=STOP)
		last_key=keydir;

	lkdir=STOP;

	if(fire || fires)	/* reset button of keyboard F1 is not pressed */
		button=TRUE;
	else
		button=FALSE;
	fires=FALSE;

	if(joyuse) {
		inc_plot();	/* speed up program to compensare for joystick delay */
		inc_plot();

		last_x=0; last_y=0;	/* get average joystick values */
                for(loop=0; loop<4; loop++) {
			chkjoy();	/* update joystick & button status */
			last_x+=joyx;
			last_y+=joyy;
		}
		joyx=last_x>>2; joyy=last_y>>2;

		if(butt1)	/* update button variable for internal use */
			button=TRUE;
		else
			button=FALSE;
	}

}


start_wait()	/* check for the joystick button or a key to be pressed (other than ESC) */
{		/* the joystick MUST be in the centre position at this time */
		/* if no key or button then FALSE is returned */

	int	a, loop;

#ifdef	DEBUG
	if(joyuse)
		cprintf("press button #1 to start \n\r");
	else
		cprintf(" Press any key to begin \n\r");
#endif

	a=FALSE;	/* show no key or joystick button to be pressed */

	if(joyuse) {
		chkjoy();
		if(butt1)
			a=TRUE;
	}
#ifdef	IBM
	if(keychr!=0 && !(keychr&0x80) && keychr!=01) {
#endif
#ifdef	HYP
	if(keychr!=0 && !(keychr&0x80) && keychr!=13) {
#endif
		a=TRUE;
		joyuse=FALSE;
		keychr=0;
        
	}

	if(a==FALSE)
		return(FALSE);	/* return to wait loop */

	if(joyuse) {
		last_x=0; last_y=0;	/* get average joystick values */
                for(loop=0; loop<50; loop++) {
			chkjoy();	/* update joystick & button status */
			last_x+=joyx;
			last_y+=joyy;
		}
		joyx=last_x/50; joyy=last_y/50;

		joy_x_low=joyx-35;
		if(joy_x_low<0)
			joy_x_low=0;
		joy_x_low+=10;

		joy_y_low=joyy-35;
		if(joy_y_low<0)
			joy_y_low=0;
		joy_y_low+=10;

		joy_x_high=joyx+35;
		if(joy_x_high>255)
			joy_x_high=255;
		joy_x_high-=10;

		joy_y_high=joyy+35;
		if(joy_y_high>255)
			joy_y_high=255;
		joy_y_high-=10;

                last_x=joyx; last_y=joyy;
		
	}

	return(TRUE);	/* show that a key or button was pressed */

}



check_turn()
{	/* get direction from keyboard or joystick */
	int	a;

	a=last_key;

	if(joyuse) {

		a=STOP;

		if(joyx<joy_x_low) {
			a=4;
/*			if(joyy<joy_y_low)
				a=3;
			if(joyy>joy_y_high)
				a=5;
*/
		}

		if(joyx>joy_x_high){
			a=0;
/*			if(joyy<joy_y_low)
				a=1;
			if(joyy>joy_y_high)
				a=7;
*/
		}
		if(joyx>=joy_x_low && joyx<=joy_x_high) {
			if(joyy<joy_y_low)
				a=2;
			if(joyy>joy_y_high)
				a=6;
		}

	}

/*	cprintf(" x = %d  y = %d  dir = %d \n\r",joyx,joyy,a);  */
	return(a);	/* return direction value */
	
}
		


get_button()	/* return the button status */
{
	return(button);

}

