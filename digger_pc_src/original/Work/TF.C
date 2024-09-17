/*
*****************************************************************************************************
*****************************************************************************************************
*****                                                                                           *****
*****                  Task F   -   Score & Screen Updating                                     *****
*****                                                                                           *****
*****                                                                                           *****
*****************************************************************************************************
*****************************************************************************************************
*/

#include	"digger.h"

extern	char	diskdata[];	/* external disk transfer buffer */
extern	char	text[];

long	score_p, score_0, score_1, bonus_0, bonus_1;

char	htemp[10], highscore[40];

long	score[11];

char	name[11][4]	= { "...", "...", "...", "...", "...", "...", "...", "...", "...", "...", "..." };


int	timeout;
int	filenum;



/* ================= score values ===================

	bug	= 250
	coin	= 25
	money	= 500

	bonus for 8 sucessive coins = 250

	bonus	= 1000
	bugs after bonus:
	1	= 200
	2	= 400
	3	= 800
	ext.

*/


init_tf()	/* reset scores related to the present game - not high scores */
{
	add_score(0);	/* show present players score on screen in color red */
}


                             

init_hi_score()
{	/* reset high score */
	int	temp, count, loop, loop2;


#ifndef	FREEDEMO

	getbuf();	/* get track 39, sector #1 of drive 0 */ 
#endif
/*
#ifdef	FREEDEMO

	filenum=open("digger.scr",0x8002);
	if(filenum>0) {
		read(filenum,diskdata,512);
		close(filenum);
	}
#endif
*/



	count=1;	/* recover high scores from disk buffer */
	for(loop=1; loop<11; loop++) {

		for(loop2=0; loop2<3; loop2++) {	/* get 3 character name */
			name[loop][loop2]=diskdata[count];
			count++;
		}

		count+=2;	/* disgard padding of 2 spaces */

		for(loop2=0; loop2<6; loop2++) {	/* get 6 digit score */
			htemp[loop2]=diskdata[count];
			count++;
		}

		strnum(htemp,&score[loop]);	/* convert 6 digit score string to la long int */

	}

	if(diskdata[0]!='s') { /* reset all scores if read was not good */
		for(temp=0; temp<11; temp++) {
			score[temp]=0;
			strcpy(name[temp],"...");
		}
	}


}



rst_score()	/* reset scores for this game */
{
	score_p=0; score_0=0; score_1=0; bonus_0=20000; bonus_1=20000;

}



task_f()	/* update fill multiplyer */
{
	int	a;

	a=a;

}


flash_score(color)
int	color;
{
	if(get_player()==0) {
		println(score_0,0,0,6,color);
	}
	else {
		if(score_1<100000)
			println(score_1,236,0,6,color);
		else
			println(score_1,248,0,6,color);
	}

}




add_score(val)	/* called to increase score */
int	val;
{


	if(get_player()==0) {
		score_0+=val;	/* add new score */
		if(score_0>999999)
			score_0=0;
		println(score_0,0,0,6,GREEN);
		if(score_0>=bonus_0) {	/* new life if posible */
			if(get_lives(1)<5) {
				add_life(1);	/* give player 1 a new life */
				print_lives();	/* update display */
			}
			bonus_0+=20000;	/* set up for next new life */
		}
	}
	else {
		score_1+=val;	/* add new score */
		if(score_1>999999)
			score_1=0;
		if(score_1<100000)
			println(score_1,236,0,6,GREEN);
		else
			println(score_1,248,0,6,GREEN);

		if(score_1>bonus_1) {	/* new life if posible */
			if(get_lives(2)<5) {
				add_life(2);	/* give player 2 a new life */
				print_lives();	/* update display */
			}
			bonus_1+=20000;	/* set up for next new life */
		}

	}

	inc_plot();	/* report screen printing */
	inc_plot();
	inc_plot();

}




check_hi_score()
{	/* update score display & check for high score */
	int	loop, xloop, yloop;

	add_score(0);	/* update score display */
	
	if(get_player()==0)
		score_p=score_0;
	else
		score_p=score_1;

	if(score_p>score[10]) {	/* a new high score has been reached */
		clear(0);

		print_present_score();
		strcpy(text,"PLAYER ");	/* set up string */
		if(get_player()==0)
			strcat(text,"1");
		else
			strcat(text,"2");
		prints(text,108,0,RED);
		

		prints(" NEW HIGH SCORE ",64,40,RED);	/* write high score on screen */

		get_name();	/* get initiales for this player */

		sort_scores();	/* update high score list */

		save_scores();	/* save high scores on disk */
		
	}
	else {	/* display game over message if not a new high score */
		blank_top();	/* clear top line of screen */

		prints("GAME OVER",104,0,WHITE);

		sn2_off();
		set_tone(40);
		sn2_stop();	/* turn off second sound source to stabalize interupts */
		for(xloop=0; xloop<20; xloop++) {
			for(loop=0; loop<8; loop++) {
				for(yloop=0; yloop<111; yloop++)
					yloop=yloop;
			}			

			for(loop=0; loop<8; loop++) {
				for(yloop=0; yloop<111; yloop++)
					yloop=yloop;
			}			

		}
                sn2_start();	/* restart sound interupts */

		prints("         ",104,0,WHITE);
	}
	
}



print_present_score()
{	/* display present score, number of lives left & high score if one has been set */
	println(score_0,0,0,6,3);	/* print present scores at top of screen */
	if(players()==2) {	/* show second score if 2 players are active */
		if(score_1<100000)
			println(score_1,236,0,6,3);
		else
			println(score_1,248,0,6,3);
	}
	
}






print_lives()
{	/* draw men on the top line to show spare lives */
 	int	loop, temp;

	temp=get_lives(1)-1;	/* dont show present life */
	for(loop=1; loop<5; loop++) {	/* a maximum of 4 lives may be displayed */

		if(temp>0)	/* draw gun */
			put_man(0,60+loop*20,0);
		else
			put_man(2,60+loop*20,0);

		temp--;
	}

	if(players()==2) {
		temp=get_lives(2)-1;	/* dont show present life */
		for(loop=1; loop<5; loop++) {	/* a maximum of 4 lives may be displayed */

			if(temp>0)	/* draw gun */
				put_man(1,244-loop*20,0);
			else
				put_man(2,244-loop*20,0);

			temp--;
		}


	}
}



show_players()	/* display number of players message */
{

	if(players()==1) {
		prints("ONE",220,25,BROWN);
		prints(" PLAYER ",192,39,BROWN);
	}
	else {
		prints("TWO",220,25,BROWN);
		prints(" PLAYERS",184,39,BROWN);
	}

}

show_scores()	/* display high score list */
{
	int	loop, color;


	prints("HIGH SCORES",16,25,3);
	color=2;
	for(loop=1; loop<11; loop++) {
		strcpy(highscore,"");	/* empty string */
		strcat(highscore,name[loop]);
		strcat(highscore,"  ");
		strscr(htemp,score[loop],TRUE);	/* get ascii string for score value */
		strcat(highscore,htemp);
		prints(highscore,16,31+loop*13,color);
		color=1;
	}

}




save_scores()	/* save scores to disk buffer */
{
	int	loop, temp;

	strcpy(diskdata,"s");	/* clear buffer & show good data with s as the first character */


	for(loop=1; loop<11; loop++) {
		strcpy(highscore,"");	/* empty string */
		strcat(highscore,name[loop]);
		strcat(highscore,"  ");
		strscr(htemp,score[loop],TRUE);	/* get ascii string for score value */
		strcat(highscore,htemp);
		strcat(diskdata,highscore);
	}

#ifndef FREEDEMO

 	savbuf();	/* save to disk */  

#endif

/*
#ifdef	FREEDEMO

	filenum=creat("digger.scr",0x8002);
	if(filenum>0) {
		write(filenum,diskdata,512);
		close(filenum);
	}
#endif
*/

}




	

get_name()	/* get players initials from the keyboard */
{
	int	y, temp, count, loop, yloop;

	prints("ENTER YOUR",100,70,3);
	prints(" INITIALS",100,90,3);
	prints("_ _ _",128,130,3);
	strcpy(name[0],"...");

	sn2_off();
	set_tone(40);
	sn2_stop();	/* turn off second sound source to stabalize interupts */


	timeout=FALSE;
	for(count=0; count<3; count++) {
		temp=0;
		while(temp==0 && !timeout) {
			temp=getkey(128+count*24,130);
			if(count && temp==0xe)
				count--;
			temp=conkey(temp);
		}

		if(temp!=0) {
			printc(temp,128+count*24,130,3);
			name[0][count]=temp;	/* store ascii in string */
		}

	}
	keychr=0;
	for(temp=0; temp<20; temp++)
		flash_palettes(15);
	sn2_start();	/* restart sound interupts */
	clear(0);
	palette(0);
}


flash_palettes(loop_max)
int	loop_max;
{
	int	loop, loop1, yloop;
	char	a;

	a=19;
	for(loop=0; loop<loop_max; loop++) {
		palette(1);
		for(yloop=0; yloop<a; yloop++) {
			yloop=yloop;
		}
		palette(0);
		for(yloop=0; yloop<a; yloop++) {
			yloop=yloop;
		}
	}

}


getkey(x,y)	/* get a letter from the keybaord & flash cursor */
int	x, y;
{
	int	count, loop;

	keychr=0;

	printc('_',x,y,3);
	for(loop=0; loop<50; loop++) {
		for(count=0; count<40; count++) {
			if(!(keychr & 0x80) && keychr)
				return(keychr);
			flash_palettes(15);			
		}
		for(count=0; count<40; count++) {
			if(!(keychr & 0x80) && keychr) {
				printc('_',x,y,3);
				return(keychr);
			}
			flash_palettes(15);			
		}
	}
	timeout=TRUE;	/* dont wait for ever */
	return(0);
}



sort_scores()	/* sort new score into the top 10 list */
{
        int	loop, count;


	count=10;
	while(count>1)  {	/* locate position of new score */
		if(score_p>=score[count-1])
			count--;
		else {
			break;
		}
	}

	for(loop=10; loop>count; loop--) {
		score[loop]=score[loop-1];
		strcpy(name[loop],name[loop-1]);
	}

	score[count]=score_p;
	strcpy(name[count],name[0]);


}



score_bug()
{	/* score for killing bug */

	add_score(250);

}



score_coin()
{	/* score for getting coin */

	add_score(25);
}



score_2_coin()
{	/* extra score for getting 8 sucessive coins */

	add_score(250);
}



score_money()
{	/* score for grabing money */

	add_score(500);

}



score_extra()
{	/* score for getting bonus */

	add_score(1000);

}


strscr(out,amount,blanks)	/* convert a long integer to a string  */
char	out[];
long	amount;
int	blanks;	/* if set to true a blank is inserted for leading zero's */
{

	int	chr, offset, flag;
	long	digit, testval;
	char	tstr[10];

	offset=0; digit=100000; flag=FALSE;
	strcpy(out,"");	/* initialize output string */

	while(digit>1) {	/* print all digits seperately exept the last */
		chr=amount/digit;
		amount %=digit;
		stcu_d(tstr,chr,10);	/* get ascii into tstr */
		if(chr || flag)	{ 	/* dont use leading zero's */
			strcat(out,tstr);	/* put new ascii digit on end of string */
			flag=TRUE;	/* show end of leading zero's */
		}
		else
			if(blanks)
				strcat(out," ");	/* replace leading zero's with a blank */

		digit /=10; offset++;
	}

	chr=amount;	/* print the last digit */
	stcu_d(tstr,chr,10);	/* get ascii into tstr */
	strcat(out,tstr);	/* put new ascii digit on end of string */


}


strnum(instr,amount)	/* convert a 6 digit string to a long integer  */
char	instr[];
long	*amount;
{

	int	chr, offset, flag;
	long	digit;

	offset=0; digit=100000; *amount=0;

	while(digit>0) {	/* print all digits seperately exept the last */
		chr=getval(instr[offset]);
		*amount =*amount+(digit*chr);
		digit /=10; offset++;
	}

}

