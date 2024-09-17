/*
*****************************************************************************************************
*****************************************************************************************************
*****                                                                                           *****
*****                  Task B   -   Money Bag  routines                                         *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****************************************************************************************************
*****************************************************************************************************
*/

#include	"digger.h"

#define		DROP_WAIT	15


#define		BAG_X		0
#define		BAG_Y		1
#define		BAG_X_POS   	2
#define		BAG_Y_POS   	3
#define		BAG_X_REM  	4
#define		BAG_Y_REM	5
#define		BAG_DIR		6
#define		BAG_LOOSE	7
#define		BAG_COUNT	8
#define		BAG_FIRST	9
#define		BAG_ACTIVE	10
#define		BAG_BROKEN	11
#define		BAG_BRAKABLE	12

int	bags_0[8][20];
int	bags_1[8][20];
int	bags[8][20];

/* the first value specifies the bag number for current round (1-7)
   
	   The second is the information for that bag
		0 = true X value (0-319)
		1 = true Y value (0-199)
		2 = X matrix position (0-14)
		3 = Y matrix position (0-9)
		4 = 0 if bag is directly within an X matrix location
		5 = 0 if bag is directly within an Y matrix location
		6 = Direction of bag movement
		7 = TRUE when bag is ready to fall - after digging under it
		8 = Counter for bag falling - decremented toward zero if above if TRUE
		9 = TRUE when bag is fall for the first time - the background is not erased for the first move
		10 = TRUE when bag is presently being used on screen
		11 = >0 when bag has fallen more than one level and has broken
		12 = TRUE when bag has fallen more than one level and needs to be broken
*/


int	bag_wait;	/* for convenience, the same variable is used for boath players */
int	broke_max;

init_tb()	/* create money bags at corrct positions for the level */
{	
        int	bag_num, x, y, x_pos, y_pos, loop;

	bag_wait=0;
	broke_max=150-get_dificulty()*10;	/* time before money pile disapears */

	for(bag_num=1; bag_num<8; bag_num++) {
		bags[bag_num][BAG_ACTIVE]=FALSE;
	}

	bag_num=1;
	for(x_pos=0; x_pos<W_MAX; x_pos++) {
		for(y_pos=0; y_pos<H_MAX; y_pos++) {
			if(get_mat(x_pos,y_pos,get_screen())=='B' && bag_num<8) {
				bags[bag_num][BAG_ACTIVE]=TRUE;		/* show that the bag is now on screen */
				bags[bag_num][BAG_BROKEN]=0;		/* show that the bag is not broken */
				bags[bag_num][BAG_BRAKABLE]=0;		/* show that the bag may not be broken */
				bags[bag_num][BAG_DIR]=STOP;	/* start bag with no movement */
				bags[bag_num][BAG_LOOSE]=FALSE;	/* set loose bag flag */
				bags[bag_num][BAG_COUNT]=DROP_WAIT;		/* set loose bag counter */
				bags[bag_num][BAG_FIRST]=TRUE;		/* set flag to show that bag has not started falling */
				x=x_pos*20+X_OFFSET;	/* find X coordinate of screen image */
				y=y_pos*18+Y_OFFSET;	/* find Y coordinate of screen image */
				bags[bag_num][BAG_X]=x;	/* save X coordinate of screen image */
				bags[bag_num][BAG_Y]=y;	/* save X coordinate of screen image */
				bags[bag_num][BAG_X_POS]=x_pos;	/* save X matrix coordinate of the image */
				bags[bag_num][BAG_Y_POS]=y_pos;	/* save Y matrix coordinate of the image */
				bags[bag_num][BAG_X_REM]=0;	/* set flag to show that bag is on an even x matrix location */
				bags[bag_num][BAG_Y_REM]=0;	/* set flag to show that bag is on an even Y matrix location */
				bag_num++;	/* set up for next bag */
			}
		}
	}
			
	for(bag_num=1; bag_num<8; bag_num++) {
		for(loop=0; loop<20; loop++) {
			if(get_player()==0)
				bags_0[bag_num][loop]=bags[bag_num][loop];
			else
				bags_1[bag_num][loop]=bags[bag_num][loop];
		}
	}

}


start_tb()
{
        int	bag_num, loop;


	for(bag_num=1; bag_num<8; bag_num++) {
		for(loop=0; loop<20; loop++) {
			if(get_player()==0)
				bags[bag_num][loop]=bags_0[bag_num][loop];
			else
				bags[bag_num][loop]=bags_1[bag_num][loop];

		}
		if(bags[bag_num][BAG_ACTIVE]) {
			sp_place(bag_num,bags[bag_num][BAG_X],bags[bag_num][BAG_Y]);	/* draw bag on screen */
		}
	}


}




end_tb()
{
	int	bag_num, loop;

	rst_fall();	/* stop any sound for falling bags */

	for(bag_num=1; bag_num<8; bag_num++) {

		if( (bags[bag_num][BAG_X_POS]==M_X_START && bags[bag_num][BAG_Y_POS]==M_Y_START)
		    || (bags[bag_num][BAG_X_REM] || bags[bag_num][BAG_Y_REM])
		    || (bags[bag_num][BAG_BROKEN] || bags[bag_num][BAG_BRAKABLE] || bags[bag_num][BAG_LOOSE]) ) {
			if(bags[bag_num][BAG_ACTIVE]) {
				bags[bag_num][BAG_ACTIVE]=FALSE;
				sp_erase(bag_num);
			}
		}

		for(loop=0; loop<20; loop++) {
			if(get_player()==0)
				bags_0[bag_num][loop]=bags[bag_num][loop];
			else
				bags_1[bag_num][loop]=bags[bag_num][loop];

		}

	}

}

task_b()
{
	int	bag_num, x_pos, y_pos, flagf, flagl;


	for(bag_num=1; bag_num<8; bag_num++) {

		if(bags[bag_num][BAG_ACTIVE]) {	/* check only active bags */
			if(bags[bag_num][BAG_BROKEN]) {

/* redraw bag for falling/breaking effect */
				if(bags[bag_num][BAG_BROKEN]==1) {
					snd_break();	/* start broken bag sound */
					draw_bag(bag_num,BAG_7,bags[bag_num][BAG_X],bags[bag_num][BAG_Y]);
					inc_plot();	/* report a screen plot */
				}
				if(bags[bag_num][BAG_BROKEN]==3) {
					draw_bag(bag_num,BAG_8,bags[bag_num][BAG_X],bags[bag_num][BAG_Y]);
					inc_plot();	/* report a screen plot */
				}
				if(bags[bag_num][BAG_BROKEN]==5) {
					draw_bag(bag_num,BAG_9,bags[bag_num][BAG_X],bags[bag_num][BAG_Y]);
					inc_plot();	/* report a screen plot */
				}

/* increment broken bag timer */
				bags[bag_num][BAG_BROKEN]++;

/* check if bag needs to be removed */
				if(bags[bag_num][BAG_BROKEN]==broke_max)	/* erase bag after time limit */
					era_bag(bag_num);
                                else {	/* if area below the bag has been dug out then increment timer for quick removal */
					if(bags[bag_num][BAG_Y_POS]<(H_MAX-1) && bags[bag_num][BAG_BROKEN]<(broke_max-10)) {
						x_pos=bags[bag_num][BAG_X_POS];
						y_pos=bags[bag_num][BAG_Y_POS];
						if((get_background(x_pos,y_pos+1) & 0x2000)==0)
							bags[bag_num][BAG_BROKEN]=broke_max-10;
					}
				}
			}
			else
/* process any active unbroken bags */
				check_bag(bag_num);
      		}
	}

/* check if falling or loose bag sound is finished */
	flagf=TRUE;	/* check for falling bags - if none then turn off falling bag sound */
	flagl=TRUE;	/* check for loose bags - if none then turn off loose bag sound */
	for(bag_num=1; bag_num<8; bag_num++) {
		if(bags[bag_num][BAG_DIR]==DOWN && bags[bag_num][BAG_ACTIVE])
			flagf=FALSE;
		if(bags[bag_num][BAG_LOOSE] && bags[bag_num][BAG_DIR]!=DOWN && bags[bag_num][BAG_ACTIVE])
			flagl=FALSE;
	}	
	if(flagf)
		rst_fall();	/* stop falling bag sound */
	if(flagl)
		rst_loose();	/* stop loose bag sound */

}


check_bag(bag_num)
int	bag_num;
{
	int	x, x_pos, x_rem, y, y_pos, y_rem, temp;


	x=bags[bag_num][BAG_X];
	x_pos=bags[bag_num][BAG_X_POS];
	x_rem=bags[bag_num][BAG_X_REM];
	y=bags[bag_num][BAG_Y];
	y_pos=bags[bag_num][BAG_Y_POS];
	y_rem=bags[bag_num][BAG_Y_REM];





	switch(bags[bag_num][BAG_DIR]) {
	case STOP:
/* check if a man has burrowed under a bag 
   if not already at the bottom of the screen and it is on an even X matrix position */
		if(y<Y_LAST && x_rem==0) {
			if(bags[bag_num][BAG_LOOSE]) {	/* the bag is already loose & getting ready to fall */
				if(!bags[bag_num][BAG_COUNT]) {	/* if the count is over then move the bag down */
					bags[bag_num][BAG_DIR]=DOWN;
					init_fall();	/* start falling bag sound */
				}
				else {
					bags[bag_num][BAG_COUNT]--;
					temp=bags[bag_num][BAG_COUNT]%8;
					switch(temp) {
					case 0:
						draw_bag(bag_num,BAG_4,x,y);
						inc_plot();	/* report a screen plot */
						init_loose();	/* sound for rocking bag */
						break;
					case 2:
						draw_bag(bag_num,BAG_0,x,y);
						inc_plot();	/* report a screen plot */
						init_loose();	/* sound for rocking bag */
						break;
					case 4:
						draw_bag(bag_num,BAG_5,x,y);
						inc_plot();	/* report a screen plot */
						init_loose();	/* sound for rocking bag */
						break;
					case 6:
						draw_bag(bag_num,BAG_0,x,y);
						inc_plot();	/* report a screen plot */
						init_loose();	/* sound for rocking bag */
						break;
					}
				}
		        }
			else {
				if(((get_background(x_pos,y_pos+1) & 0xfdf)!=0xfdf) && (!get_man_block(x_pos,y_pos+1)) )
					bags[bag_num][BAG_LOOSE]=TRUE;
			}
        	}
		else {	/* if bag is not ready to fall then reset falling variables - needed if a bag is moved
		  	   while it is getting ready to fall */
			bags[bag_num][BAG_COUNT]=DROP_WAIT;
			bags[bag_num][BAG_LOOSE]=FALSE;
		}

		break;

	case LEFT:
	case RIGHT:
/* check if bag needs to fall after being pushed left or right by the man or another bag 
   or if the bag is moving left or right and is directly in an X matrix position & not at the 
   bottom of screen, then change its direction to down 
   or if bag is at an even matrix position and ground is intact underneath - stop the movement */
		if(x_rem==0 && y<Y_LAST) {
			/* if a hole or partial hole is under the bag then start it falling */	
			if((get_background(x_pos,y_pos+1) & 0xfdf)!=0xfdf) {
				bags[bag_num][BAG_DIR]=DOWN;
				bags[bag_num][BAG_COUNT]=0;	/* clear counter to stop bags from wobbling */
				init_fall(DOWN);	/* start falling sound */
			}
			else
				stop_bag(bag_num);
		}

		if(x_rem==0 && y>=Y_LAST)	/* special case for stopping right - left movement on the bottom row */
			stop_bag(bag_num);
		break;

	case DOWN:
/* check if bag has fallen one level - bag must be broken when bottom is reached 
   also check for conditions that stop the bag from continuing to fall */
		if(y_rem==0) {
			bags[bag_num][BAG_BRAKABLE]++;
		}

		/* check for conditions to stop bag falling */
		if(y>=Y_LAST) {	/* stop bag at bottom of screen */
			stop_bag(bag_num);
		}
		else {	/* stop bag if the next layer is completely intact */			
			if(((get_background(x_pos,y_pos+1) & 0xfdf)==0xfdf) && y_rem==0) {
				stop_bag(bag_num);
			}
		}

		save_bugs(bags[bag_num][BAG_X_POS]);	/* tell bugs that a bag is falling on a Y tunnel */

		break;

	}
		

/* move bag if required */

	if(bags[bag_num][BAG_DIR]!=STOP) {	/* check for delay in moving bags left and right */
		if(bags[bag_num][BAG_DIR]!=DOWN && bag_wait) {	/* never delay falling bags */
			bag_wait--;
		}
		else
			move_bag(bag_num,bags[bag_num][BAG_DIR]);
	}

}



stop_bag(bag_num)	/* stop bag movement & set bag matrix information */
int	bag_num;
{
	int	loop, mask, colision;

	if(bags[bag_num][BAG_DIR]==DOWN && bags[bag_num][BAG_BRAKABLE]>1)
		bags[bag_num][BAG_BROKEN]=1;	/* start broken bag sequence */
	else
		bags[bag_num][BAG_BRAKABLE]=0;
		

	bags[bag_num][BAG_DIR]=STOP;
	bags[bag_num][BAG_COUNT]=DROP_WAIT;
	bags[bag_num][BAG_LOOSE]=FALSE;

	colision=draw_bag(bag_num,BAG_0,bags[bag_num][BAG_X],bags[bag_num][BAG_Y]);
	inc_plot();	/* report a screen plot */
	mask=0x2;	/* set mask for first bag */

	for(loop=1; loop<8; loop++) {
		if(mask & colision) 
			era_bag(loop);	/* erase any bags unbroken under the one just stopped */
		mask<<=1;	/* check the next bag */
	}	
}



move_bag(bag_num,dir)
int	bag_num, dir;
{	/* move a specific bag - if any other bags are hit then the move is aborted & a false is returned */
	int	x, y, x_pos, y_pos, x_rem, y_rem, old_x, old_y, colision, flag;

	flag=TRUE;	/* initialize flag to show a good move */
	x=bags[bag_num][BAG_X];
	x_pos=bags[bag_num][BAG_X_POS];
	x_rem=bags[bag_num][BAG_X_REM];
	y=bags[bag_num][BAG_Y];
	y_pos=bags[bag_num][BAG_Y_POS];
	y_rem=bags[bag_num][BAG_Y_REM];

	old_x=x; old_y=y;	/* save old location incase of a bad move */

/* check if bag is not active (broken on ground) */
	if(bags[bag_num][BAG_BROKEN]) {
		grab_money(bag_num);	/* check if man hits money pile */
		return(TRUE);	/* show a good move but don't actually move the bag */
	}



/* check if trying to move a falling bag left or right - ignore unless hitting man or bug */
	if(bags[bag_num][BAG_DIR]==DOWN && (dir==LEFT || dir==RIGHT) )  {
		colision=draw_bag(bag_num,BAG_6,x,y);	/* redraw the bag to see if it hits the man */
		inc_plot();	/* report a screen plot */
		if((colision & 0x01) && get_m_y()>=y) {	/* check for colisions with man lower on the screen */
			kill_man(MONEY_BAG,bag_num);	/* start man dead sequence */
		}		
		if(colision & 0x3f00) {	/* check for colision with bugs */
			bag_bugs(bag_num,colision);
        	}

		return(TRUE);	/* show a good move but don't actually move the bag */
	}


/* check if bag would go off screen */
	if( (x==X_LAST && dir==RIGHT) || (x==X_FIRST && dir==LEFT) ||
	    (y==Y_LAST && dir==DOWN) || (y==Y_FIRST && dir==UP) )
		flag=FALSE;	/* set flag for bad move */





	if(flag) {	/* find new bag position & erase background if bag is falling */
		switch(dir) {	/* move bag position */
		case RIGHT:
			x+=4;
			break;
		case LEFT:
			x-=4;
			break;
		case DOWN:
			if(bags[bag_num][BAG_FIRST]) {	/* don't erase top of passage when first starting to fall */
				bags[bag_num][BAG_FIRST]=FALSE;
				era_b_first(x,y);	/* erase first piece of background if bag is starting to fall */ 
				era_up(x,y+21);	/* erase above to create a hole as bugs will take this new route */

			}
			else
				era_b_down(x,y);	/* erase background if bag is falling */
			era_background(x,y,dir);	/* erase background array bits */
			dest_coin(x_pos,y_pos);	/* remove any coin in area to be destroyed - next block down */
			y+=6;
			break;
		}
		

/* move the bag - check & handle colisions */
		switch(dir) {
		case DOWN:
			colision=draw_bag(bag_num,BAG_6,x,y);	/* move a falling bag on screen */
			inc_plot();	/* report a screen plot */
			if((colision & 0x01) && get_m_y()>=y) {	/* check for colisions with man lower on the screen */
				kill_man(MONEY_BAG,bag_num);	/* start man dead sequence */
			}		
			if(colision & 0x3f00)	/* bugs hit by falling bag */
				bag_bugs(bag_num,colision);

			break;

		case LEFT:
		case RIGHT:
/* bags may not be loose if being pushed sideways */
			bags[bag_num][BAG_COUNT]=DROP_WAIT;
			bags[bag_num][BAG_LOOSE]=FALSE;

			colision=draw_bag(bag_num,BAG_0,x,y);	/* move normal bag on screen */
			inc_plot();	/* report a screen plot */
			bag_wait=1;	/* delay bag movement if a colision occures */
			if(colision & 0xfe) {	/* check for colisions with other bags */
				/* if other bags won't move then set flag & move back to the origional position */
				if(!move_all_bags(dir,colision)) {
					x=old_x; y=old_y;	/* restore origional coordinates */
					draw_bag(bag_num,BAG_0,x,y);		/* move bag */
					inc_plot();	/* report a screen plot */
					flag=FALSE;	/* show a bad move */
				}
			}

			if((colision & 0x1) || (colision & 0x3f00) ) {	/* check for colisions with man or bug */
				x=old_x; y=old_y;	/* restore origional coordinates */
				draw_bag(bag_num,BAG_0,x,y);		/* move bag */
				inc_plot();	/* report a screen plot */
				flag=FALSE;	/* show a bad move */
			}

			break;

		}



/* if move was ok then make bag keep moving else change direction */
		if(flag) 	
			bags[bag_num][BAG_DIR]=dir;
		else
			bags[bag_num][BAG_DIR]=get_rev(dir);


/* save temparary values in permenent array */ 	
		bags[bag_num][BAG_X]=x;	
		bags[bag_num][BAG_Y]=y;
		bags[bag_num][BAG_X_POS]=(x-X_OFFSET)/20;	/* indicate X matrix position */
		bags[bag_num][BAG_Y_POS]=(y-Y_OFFSET)/18;	/* indicate Y matrix position */
		bags[bag_num][BAG_X_REM]=(x-X_OFFSET)%20;	/* indicate if bag is on an even X matrix position */
		bags[bag_num][BAG_Y_REM]=(y-Y_OFFSET)%18;	/* indicate if bag is on an even Y matrix position */


	}
	
        return(flag);	/* return a true if move was sucessfull */

}



move_all_bags(dir,colision)
int	dir, colision;
{	/* move all bags that are indicated by the colision status */
	int	flag, bag_num, mask;

	flag=TRUE;	/* flag is reset to false if any bag will not move */
	mask=2;

	for(bag_num=1; bag_num<8; bag_num++) {
		if(colision & mask) {	/* try to move bag - if not possible then set flag to false */
			if(!move_bag(bag_num,dir))
				flag=FALSE;
		}
		mask<<=1;	/* set mask for next bit */
	}


	return(flag);	/* return a true if the move was completely sucessfull */

}



money_check(colision)
int	 colision;
{	/* check if any bags indicated by the colision status are broken money bags */
	int	flag, bag_num, mask;

	flag=TRUE;	/* flag is reset to false if any bag will not move */
	mask=2;

	for(bag_num=1; bag_num<8; bag_num++) {
		if(colision & mask) {	/* try to move bag - if not possible then set flag to false */
			if(bags[bag_num][BAG_BROKEN]) 
				grab_money(bag_num);	/* set flag to false if bag was not broken */
			else 
				flag=FALSE;
		}
		mask<<=1;	/* set mask for next bit */
	}
	return(flag);
}



era_bag(bag_num)
int	bag_num;
{

	if(bags[bag_num][BAG_ACTIVE]) {
		bags[bag_num][BAG_ACTIVE]=FALSE;	/* set non active flag */
		sp_erase(bag_num);	/* erase sprite from screen */
	}

}


get_y_bag(bag_num)
int	bag_num;
{
	return(bags[bag_num][BAG_Y]);

}

get_d_bag(bag_num)
int	bag_num;
{
	if(bags[bag_num][BAG_ACTIVE])
		return(bags[bag_num][BAG_DIR]);
	else
		return(STOP);	/* show bag stopped if not active */

}


get_active_bags()	/* return a mask to ignore all inactive bags */
{
	int	bag_num, mask, a;

	mask=2; a=0;
	for(bag_num=1; bag_num<8; bag_num++) {
		if(bags[bag_num][BAG_ACTIVE])
			a|=mask;	/* set apropriate colision mask bit ( sprites 1-7 ) */					
		mask<<=1;	/* set mask for the next bit */
	}

	return(a);

}


break_bags(colision)	/* remove all bags indicated by the colision data */
{
	int	bag_num, mask;

	mask=2;
	for(bag_num=1; bag_num<8; bag_num++) {
		if(bags[bag_num][BAG_ACTIVE] && (colision & mask) )
			era_bag(bag_num);	/* remove bag */

		mask<<=1;	/* set mask for the next bit */
	}

}


get_moving_bags()	/* return a mask to ignore all inactive bags */
{
	int	bag_num, mask, a;

	a=0;
	for(bag_num=1; bag_num<8; bag_num++) {
		if(bags[bag_num][BAG_ACTIVE] && bags[bag_num][BAG_BROKEN]<10 
		   && (bags[bag_num][BAG_BROKEN] || bags[bag_num][BAG_LOOSE]))
			a++;
	}

	return(a);

}


grab_money(bag_num)
int	bag_num;
{
	int	colision;

	colision=draw_bag(bag_num,BAG_9,bags[bag_num][BAG_X],bags[bag_num][BAG_Y]);	/* check if man hits a money pile */
	inc_plot();	/* report a screen plot */
	if(colision & 0x01) {
		score_money();	/* increase score for getting money */
		snd_money();	/* start sound for grabbing money */
		rst_mwait();	/* reset man wait so man is not delayed getting money */
	}		
	else
		rst_bwait();	/* reset bug wait so bug is not delayed getting money */

	era_bag(bag_num);	/* remove money pile */
}

