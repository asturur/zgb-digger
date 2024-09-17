/*
*****************************************************************************************************
*****************************************************************************************************
*****                                                                                           *****
*****                  Task A   -   Bug Movement                                                *****
*****                                                                                           *****
*****                                                                                           *****
*****       The level of dificulty depends on the speed of travel, time between creation of     *****
*****       new bugs, the number of total bugs and the time in the advancing mode               *****
*****                                                                                           *****
*****************************************************************************************************
*****************************************************************************************************
*/

#include	"digger.h"


/* all man related variables are re-initialized for eack round & screen */

int	bug[6][20];


#define		BUG_ACTIVE	0
#define		BUG_X		1
#define		BUG_Y		2
#define		BUG_X_POS	3
#define		BUG_Y_POS	4
#define		BUG_X_REM	5
#define		BUG_Y_REM	6
#define		BUG_DIR		7
#define		BUG_OLD_DIR	8
#define		BUG_WAIT	9
#define		BUG_ALIVE	10
#define		BUG_TYPE	11
#define		BUG_STUCK	12
#define		DEAD_TYPE	13
#define		DEAD_NUM	14
#define		DEAD_WAIT	15
#define		BUG_BEGIN	16

/* the first element specifies the bug number (0-5)

   the second element specifies:
		BUG_ACTIVE		- set to TRUE if the bug is active on the screen 
		BUG_X      		- X screen location of bug 
		BUG_Y      		- Y screen location of bug 
		BUG_X_POS		- X matrix position og bug 
		BUG_Y_POS		- Y matrix position og bug 
		BUG_X_REM		- X remainder 
		BUG_Y_REM		- Y remainder
		BUG_DIR	 		- direction of travel
		BUG_OLD_DIR		- last direction of travel ( used to draw bug )
		BUG_WAIT   		- wait counter
		BUG_ALIVE  		- alive or dead status - TRUE = alive
		BUG_TYPE   		- Normal or advancing types of bugs
		BUG_STUCK  		- counter to turn normal bugs into advancing ext.
		DEAD_TYPE  		- type of dead body being displayed
		DEAD_NUM   		- the bag number that coused the bugs death
		DEAD_WAIT  		- wait between dead body type changes
        	BUG_BEGIN		- delay before moving new bug


*/

int	wait_flag;	/* set up by other modules to void bug_wait */

int	bugs_created, bugs_total, bugs_max, bugs_delay, start_delay;
int 	bonus_ready;

init_ta()
{	
	int	bug_num;

	for(bug_num=0; bug_num<6; bug_num++) {

		bug[bug_num][BUG_ACTIVE]=FALSE;

	}

	start_delay=(45-(get_dificulty()<<1));

	bugs_created=0;	/* counter of number of bugs that have been created on screen */
	bugs_total=5+get_dificulty();	/* total number of bugs to be in the round */
/*	bugs_total=1;	/* ********************* */  */

	switch(get_dificulty()) { /* maximum number of bugs to be on the screen at one time */
	case 1:
		bugs_max=3;	
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		bugs_max=4;
		break;
	case 8:
	case 9:
	case 10:
		bugs_max=5;
		break;
	}
/* 	bugs_max=1;	/* ******************* */  */

	bugs_delay=10;	/* delay brfore first bug is created */
	bonus_ready=TRUE;	/* show that a bonus may be created */
}





end_ta()
{
	int	bug_num;

	for(bug_num=0; bug_num<6; bug_num++) {
		if(bug[bug_num][BUG_ACTIVE])
			sp_erase(bug_num+8);	/* erase bug from screen */
	}
}


task_a()
{
	int	bug_num;
	
/* check if new bugs need creating */
	if(bugs_delay>0)
		bugs_delay--;
	else {
		if(bugs_created<bugs_total && bugs_count()<bugs_max && get_m_status() && (!get_bonus()) )
			start_bug();
		if(bonus_ready && bugs_created==bugs_total && bugs_delay==0 && get_m_status()) {
			bonus_ready=FALSE;	/* only create one bonus per round */
			begin_bonus(1);	/* initiate bonus sequence */
		}
	}


/* move all active bugs */
	for(bug_num=0; bug_num<6; bug_num++) {
		if(bug[bug_num][BUG_ACTIVE]) {
			if(bug[bug_num][BUG_STUCK]>(10-get_dificulty()) && bug[bug_num][BUG_TYPE]==NORMAL) {
				bug[bug_num][BUG_STUCK]=0;
				bug[bug_num][BUG_TYPE]=ADVANCING;
			}


			if(bug[bug_num][BUG_ALIVE]) {
				if(!bug[bug_num][BUG_WAIT]) {
					move_bug(bug_num);
					/* alow bug to move again to increase speed */
					if(bug[bug_num][BUG_ALIVE] && (rnd(15-get_dificulty())==0) 
					   && bug[bug_num][BUG_TYPE]!=ADVANCING)
						move_bug(bug_num);
				}
				else
					bug[bug_num][BUG_WAIT]--;
			}
			else
				move_dead_bug(bug_num);
		}
	}

}



start_bug()
{
	int	bug_num;

	for(bug_num=0; bug_num<6; bug_num++) {
		if(!bug[bug_num][BUG_ACTIVE]) {
			bug[bug_num][BUG_ACTIVE]=TRUE;
			bug[bug_num][BUG_ALIVE]=TRUE;
			bug[bug_num][BUG_WAIT]=0;
			bug[bug_num][BUG_TYPE]=NORMAL;
			bug[bug_num][BUG_STUCK]=0;

			bug[bug_num][BUG_X_POS]=14;
			bug[bug_num][BUG_Y_POS]=0;
			bug[bug_num][BUG_X]=14*20+X_OFFSET;
			bug[bug_num][BUG_Y]=Y_OFFSET;
			bug[bug_num][BUG_X_REM]=0;
			bug[bug_num][BUG_Y_REM]=0;
			bug[bug_num][BUG_DIR]=LEFT;
			bug[bug_num][BUG_OLD_DIR]=LEFT;

			bugs_created++;	/* increment count of bugs already created */
			bugs_delay=start_delay;	/* set delay before next bug creation */
			bug[bug_num][BUG_BEGIN]=5;	/* delay before new bug starts to move */
			sp_place(bug_num+8,bug[bug_num][BUG_X],bug[bug_num][BUG_Y]);
			break;	/* only place one bug */
		}
	}
			
}



move_bug(bug_num)
int	bug_num;
{
	int	colision, flag, bug_x_old, bug_y_old, dir, dir_1, dir_2, dir_3, dir_4, temp;

	bug_x_old=bug[bug_num][BUG_X];
	bug_y_old=bug[bug_num][BUG_Y];

/* check for change of direction if on an even intersection */
	if(bug[bug_num][BUG_X_REM]==0 && bug[bug_num][BUG_Y_REM]==0) {


/* check if an advancing bug should be changed back to normal */
		if(bug[bug_num][BUG_STUCK]>(30+get_dificulty()*2) && bug[bug_num][BUG_TYPE]==ADVANCING ) {
			bug[bug_num][BUG_STUCK]=0;
			bug[bug_num][BUG_TYPE]=NORMAL;
		}



/* find order of best directions */
		if(abs(get_m_y()-bug[bug_num][BUG_Y]) > abs(get_m_x()-bug[bug_num][BUG_X]) ) {		
			/* if Y distance is greater then give priority to Y travel */
			if(get_m_y()<bug[bug_num][BUG_Y]) {
				dir_1=UP;
				dir_4=DOWN;
			}
			else {
				dir_1=DOWN;
				dir_4=UP;
			}
			if(get_m_x()<bug[bug_num][BUG_X]) {
				dir_2=LEFT;
				dir_3=RIGHT;
			}
			else {
				dir_2=RIGHT;
				dir_3=LEFT;
			}
		}
		else {
			if(get_m_x()<bug[bug_num][BUG_X]) {
				dir_1=LEFT;
				dir_4=RIGHT;
			}
			else {
				dir_1=RIGHT;
				dir_4=LEFT;
			}
			if(get_m_y()<bug[bug_num][BUG_Y]) {
				dir_2=UP;
				dir_3=DOWN;
			}
			else {
				dir_2=DOWN;
				dir_3=UP;
			}

		}



/* if bugs are running away then change order of directions */
		if(get_bonus() ) {
			temp=dir_1;
			dir_1=dir_4;
			dir_4=temp;
			temp=dir_2;
			dir_2=dir_3;
			dir_3=temp;
		}


/* make backtracking the last choice */

		dir=get_rev(bug[bug_num][BUG_DIR]);

		if(dir==dir_1) {
			dir_1=dir_2;
			dir_2=dir_3;
			dir_3=dir_4;
			dir_4=dir;
		}
		if(dir==dir_2) {
			dir_2=dir_3;
			dir_3=dir_4;
			dir_4=dir;
		}
		if(dir==dir_3) {
			dir_3=dir_4;
			dir_4=dir;
		}

/* add randmness to movement */
		if((rnd(5+get_dificulty())==1) && get_dificulty()<6) {
			temp=dir_1;
			dir_1=dir_3;
			dir_3=temp;
		}

/* choose best direction that is a clear path */
		while(TRUE) {
			if(back_check(dir_1,bug[bug_num][BUG_X_POS],bug[bug_num][BUG_Y_POS])) {
				dir=dir_1;
				break;
			}
			if(back_check(dir_2,bug[bug_num][BUG_X_POS],bug[bug_num][BUG_Y_POS])) {
				dir=dir_2;
				break;
			}
			if(back_check(dir_3,bug[bug_num][BUG_X_POS],bug[bug_num][BUG_Y_POS])) {
				dir=dir_3;
				break;
			}
			if(back_check(dir_4,bug[bug_num][BUG_X_POS],bug[bug_num][BUG_Y_POS])) {
				dir=dir_4;
				break;
			}
#ifdef	DEBUG
			cprintf("error in new direction of bug %d  %d  %d  %d \n\r",dir_1, dir_2, dir_3, dir_4);
#endif
			break;
		}			

		if(bug[bug_num][BUG_TYPE]==ADVANCING)	/* bug diggs a new path when advancing */
			dir=dir_1;	/* always go in the best direction */


/* delay bugs as they cross an even boundary when changing direction */
		if(bug[bug_num][BUG_DIR]!=dir)
			bug[bug_num][BUG_WAIT]++;


		bug[bug_num][BUG_DIR]=dir;			

	}



/* check if bug is going off screen */
	if( (bug[bug_num][BUG_X]==X_LAST && bug[bug_num][BUG_DIR]==RIGHT)
	 || (bug[bug_num][BUG_X]==X_FIRST && bug[bug_num][BUG_DIR]==LEFT)
	 || (bug[bug_num][BUG_Y]==Y_LAST && bug[bug_num][BUG_DIR]==DOWN)
	 || (bug[bug_num][BUG_Y]==Y_FIRST && bug[bug_num][BUG_DIR]==UP) )
		bug[bug_num][BUG_DIR]=STOP;

	if(bug[bug_num][BUG_DIR]==LEFT || bug[bug_num][BUG_DIR]==RIGHT)
		bug[bug_num][BUG_OLD_DIR]=bug[bug_num][BUG_DIR];

/* move position values */
	if(bug[bug_num][BUG_TYPE]==ADVANCING) 	/* bug is digging a new tunnel - update background array */
		era_background(bug[bug_num][BUG_X],bug[bug_num][BUG_Y],bug[bug_num][BUG_DIR]);

	switch(bug[bug_num][BUG_DIR]) {
	case RIGHT:
		if(bug[bug_num][BUG_TYPE]==ADVANCING)
			era_right(bug[bug_num][BUG_X],bug[bug_num][BUG_Y]);	/* erase a new path */
		bug[bug_num][BUG_X]+=4;
		break;
	case LEFT:
		if(bug[bug_num][BUG_TYPE]==ADVANCING)
			era_left(bug[bug_num][BUG_X],bug[bug_num][BUG_Y]);	/* erase a new path */
		bug[bug_num][BUG_X]-=4;
		break;
	case UP:
		if(bug[bug_num][BUG_TYPE]==ADVANCING)
			era_up(bug[bug_num][BUG_X],bug[bug_num][BUG_Y]);	/* erase a new path */
		bug[bug_num][BUG_Y]-=3;
		break;
	case DOWN:
		if(bug[bug_num][BUG_TYPE]==ADVANCING)
			era_down(bug[bug_num][BUG_X],bug[bug_num][BUG_Y]);	/* erase a new path */
		bug[bug_num][BUG_Y]+=3;
		break;
	}


	if(bug[bug_num][BUG_TYPE]==ADVANCING) {	/* bug is digging a new tunnel */
		/* remove any coin in the path */
		coin_check((bug[bug_num][BUG_X]-X_OFFSET)/20,(bug[bug_num][BUG_Y]-Y_OFFSET)/18,
			   (bug[bug_num][BUG_X]-X_OFFSET)%20,(bug[bug_num][BUG_Y]-Y_OFFSET)%18,bug[bug_num][BUG_DIR]);
	}

	if(!get_m_status() ) {	/* reset bug position if man is already dying */
		bug[bug_num][BUG_X]=bug_x_old;
		bug[bug_num][BUG_Y]=bug_y_old;
	}

	if(bug[bug_num][BUG_BEGIN]) {	/* delay before movement begins */
		bug[bug_num][BUG_BEGIN]--;
		bug[bug_num][BUG_X]=bug_x_old;
		bug[bug_num][BUG_Y]=bug_y_old;
	}

/* move bug on screen & check for colisions */

	if(bug[bug_num][BUG_TYPE]==ADVANCING && bug[bug_num][BUG_STUCK]<100)
		bug[bug_num][BUG_STUCK]++;	/* increment counter for digging monsters */

	flag=TRUE;	/* start showing a good move */
	colision=draw_bug(bug_num,bug[bug_num][BUG_TYPE],bug[bug_num][BUG_OLD_DIR],bug[bug_num][BUG_X],bug[bug_num][BUG_Y]);
	inc_plot();	/* report a screen plot */
	if(colision & 0x3f00) {	/* colision with other bugs */
		bug[bug_num][BUG_WAIT]++;
		dir_change(bug_num,colision);	/* change direction of coliding bugs */
		inc_extra(colision);	/* increment plot counter for colisions with other bugs */
	}

	if(colision & 0xfe & get_active_bags()) {
		bug[bug_num][BUG_WAIT]++;
		wait_flag=FALSE;	/* flag will be set if a money pile was hit */
		if(bug[bug_num][BUG_DIR]==LEFT || bug[bug_num][BUG_DIR]==RIGHT) {
			flag=move_all_bags(bug[bug_num][BUG_DIR],colision);	/* move bags touched bu bug */
			bug[bug_num][BUG_WAIT]++;	/* slow down bug */
		}
		else {
			if(!money_check(colision))	/* remove any broken bags touched bu bug */
				flag=FALSE;
		}
		if(wait_flag)
			bug[bug_num][BUG_WAIT]=0;	/* dont slow bug if getting money pile */
		
                if(bug[bug_num][BUG_TYPE]==ADVANCING && bug[bug_num][BUG_STUCK]>1) 
			break_bags(colision);	/* break bags that are blocking the way */


	}


 	/* bug is stuck if on top of another bug */
	if(bug[bug_num][BUG_TYPE]==NORMAL && (colision & 0x3f00) && get_m_ststus() )
		bug[bug_num][BUG_STUCK]++;



	if(!flag) {	/* move bug back to the origional position */
		bug[bug_num][BUG_X]=bug_x_old;
		bug[bug_num][BUG_Y]=bug_y_old;	/* dont allow bags to be moved up or down */
		draw_bug(bug_num,bug[bug_num][BUG_TYPE],bug[bug_num][BUG_OLD_DIR],bug[bug_num][BUG_X],bug[bug_num][BUG_Y]);
		inc_plot();	/* report a screen plot */
		if(bug[bug_num][BUG_TYPE]==NORMAL) 
			bug[bug_num][BUG_STUCK]++;

		/* dont let normal bug get stuck going up or down */
		if( (bug[bug_num][BUG_DIR]==UP || bug[bug_num][BUG_DIR]==DOWN) && bug[bug_num][BUG_TYPE]==NORMAL)
			bug[bug_num][BUG_DIR]=get_rev(bug[bug_num][BUG_DIR]);	/* change bugs direction */

	}

 	if((colision & 0x01) && get_m_status() ) {
		if(get_bonus()) {
			era_bug(bug_num);
			bonus_score();
			snd_bug();	/* sound for bonus bug eaten */
		}
		else
			kill_man(BUG,0);	/* if man is still alive then kill man if hit by bug */
	}


	bug[bug_num][BUG_X_POS]=(bug[bug_num][BUG_X]-X_OFFSET)/20;
	bug[bug_num][BUG_Y_POS]=(bug[bug_num][BUG_Y]-Y_OFFSET)/18;
	bug[bug_num][BUG_X_REM]=(bug[bug_num][BUG_X]-X_OFFSET)%20;
	bug[bug_num][BUG_Y_REM]=(bug[bug_num][BUG_Y]-Y_OFFSET)%18;


}



move_dead_bug(bug_num)
int	bug_num;
{

	switch(bug[bug_num][DEAD_TYPE]) {
	case MONEY_BAG:
		if(get_y_bag(bug[bug_num][DEAD_NUM])+6>bug[bug_num][BUG_Y])
			bug[bug_num][BUG_Y]=get_y_bag(bug[bug_num][DEAD_NUM]);

		draw_dead_bug(bug_num,bug[bug_num][BUG_TYPE],bug[bug_num][BUG_OLD_DIR],bug[bug_num][BUG_X],bug[bug_num][BUG_Y]);
		inc_plot();	/* report a screen plot */
		
		if(get_d_bag(bug[bug_num][DEAD_NUM])==STOP) {
			bug[bug_num][DEAD_WAIT]=1;
			bug[bug_num][DEAD_TYPE]=DEAD;
		}

		break;

	case DEAD:
		if(bug[bug_num][DEAD_WAIT]) {
			bug[bug_num][DEAD_WAIT]--;
			break;
		}
		era_bug(bug_num);	/* erase bug */
		score_bug();		/* score for killing bug with bag */
		break;
	}

}


/*
add_bug()
{	/* allows another bug to be created */
	bugs_total++;

}
*/



inc_extra(colision)
int	colision;
{	/* reports aditional delay when bugs plot over each other */
	int	loop, mask;

	mask=0x100;
	for(loop=0; loop<6; loop++) {
		if(colision & mask)
			inc_plot();	/* report aditional plots */
		mask<<=1;
	}

}


dir_change(bug_num,colision)
int	bug_num, colision;
{
	int	loop, mask;

	mask=0x100;
	for(loop=0; loop<6; loop++) {
		if(colision & mask) {
			if(bug[bug_num][BUG_DIR]==bug[loop][BUG_DIR] && bug[loop][BUG_BEGIN]==0 && bug[bug_num][BUG_BEGIN]==0)
				bug[loop][BUG_DIR]=get_rev(bug[loop][BUG_DIR]);
		}
		mask<<=1;
	}

}



back_check(dir,x_pos,y_pos)
int	dir, x_pos, y_pos;
{
	int	flag;

	flag=FALSE;

	switch(dir) {
	case RIGHT:
		if(x_pos<W_MAX-1) {
			if( (get_background(x_pos+1,y_pos) & 0x2000)==0 
			    && ( (get_background(x_pos+1,y_pos) & 0x1)==0 || (get_background(x_pos,y_pos) & 0x10)==0) )
				flag=TRUE;
		}
		break;
	case LEFT:
		if(x_pos>0) {
			if( (get_background(x_pos-1,y_pos) & 0x2000)==0 
			    && ( (get_background(x_pos-1,y_pos) & 0x10)==0 || (get_background(x_pos,y_pos) & 0x1)==0) )
				flag=TRUE;
		}
		break;
	case UP:
		if(y_pos>0) {
			if( (get_background(x_pos,y_pos-1) & 0x2000)==0 
			    && ( (get_background(x_pos,y_pos-1) & 0x800)==0 || (get_background(x_pos,y_pos) & 0x40)==0) )
				flag=TRUE;
		}
		break;
	case  DOWN:
		if(y_pos<H_MAX-1) {
			if( (get_background(x_pos,y_pos+1) & 0x2000)==0 
			    && ( (get_background(x_pos,y_pos+1) & 0x40)==0 || (get_background(x_pos,y_pos) & 0x800)==0) )
				flag=TRUE;
		}
		break;
	}

	return(flag);

}



save_bugs(x_pos)
int	x_pos;
{
	int	bug_num;

	for(bug_num=0; bug_num<6; bug_num++) {
		if(bug[bug_num][BUG_X_POS]==x_pos && bug[bug_num][BUG_DIR]==UP)
			bug[bug_num][BUG_DIR]=DOWN;	/* give bug intelegence to run away from falling bags */
	}

}



era_bug(bug_num)
int	bug_num;
{

	if(bug[bug_num][BUG_ACTIVE]) {	/* dont erase a dead image */
		bug[bug_num][BUG_ACTIVE]=FALSE;
		bug[bug_num][BUG_ALIVE]=FALSE;
		sp_erase(bug_num+8);
		if(get_bonus() )	/* create another bug if in bonus round */
			bugs_total++;		
	}

}
 


bag_bugs(bag_num,colision)
int	bag_num, colision;
{	/* kill all bugs indicated by the colision as they have been hit by a money bag */
	int	bug_num, mask;

	mask=0x100;
	for(bug_num=0; bug_num<6; bug_num++) {
		if((colision & mask) && bug[bug_num][BUG_Y]>=get_y_bag(bag_num)) {	/* this bug has been hit */
			kill_bug(bug_num,MONEY_BAG,bag_num);
		}
		mask<<=1;
	}

}



rem_bugs(colision)
int	 colision;
{	/* erase all bugs indicated by the colision word */
	int	bug_num, mask, stat;

	stat=0;	/* counter for number of bugs removed */
	mask=0x100;
	for(bug_num=0; bug_num<6; bug_num++) {
		if(colision & mask) {	/* this bug has been hit */
			era_bug(bug_num);
			stat++;
		}
		mask<<=1;
	}
	return(stat);
}



kill_bug(bug_num,d_type,num)
int	bug_num, d_type, num;
{

	bug[bug_num][BUG_ALIVE]=FALSE;
	bug[bug_num][DEAD_TYPE]=d_type;
	bug[bug_num][DEAD_NUM]=num;


}


bugs_left()
{	/* check if all bugs that should be created are dead */
	return(bugs_count()+bugs_total-bugs_created);

}



bugs_count()
{
	int	bug_num, a;

	a=0;	/* counter for active bugs */
	for(bug_num=0; bug_num<6; bug_num++) {
		if(bug[bug_num][BUG_ACTIVE])
			a++;
	}
	return(a);	/* return count of active bugs */

}	


delay_bugs(a)
int	a;
{	/* delay bugs to balance movement speed */

	int	loop;

	if(a>6)
		a=6;

	for(loop=0; loop<a; loop++) {
		if(loop!=0)	/* one bug always goes at full speed */
			bug[loop][BUG_WAIT]++;
	}

}



rst_bwait()
{
	wait_flag=TRUE;

}
	

