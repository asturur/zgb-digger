/*
*****************************************************************************************************
*****************************************************************************************************
*****                                                                                           *****
*****                  Task Z   -   Task dispatcher                                             *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****************************************************************************************************
*****************************************************************************************************
*/



#include	"digger.h"


int	table[] = { 14,13,7,6,5,4,3,2,1,12,11,10,9,8,15,0 };

int	game[2][4];


#define	DEAD		0
#define	LIVES		1
#define	LEVEL		2
#define	DONE_LEVEL	3

char	text[30];	/* text string used for high score ext */
int	player, num_players, new_round, new_player;
int	plot_count;




main()
{
	int	flag, demo_time, loop, x;

	med_res();
	palette(0); backgrnd(0);
	init_hi_scores();
	kb_init(); init_joy();
	init_sound();
	num_players=1;

	clear(0);

	med_txt("For more information about",5,5,7);
	med_txt("other exciting entertainment",5,6,7);
	med_txt("products, write to us at:",5,7,7);


	med_txt("Windmill Software Inc.",8,9,7);
	med_txt("2209 Leominster Drive",8,10,7);
	med_txt("Burlington, Ontario",8,11,7);
	med_txt("CANADA   L7P 3W8",8,12,7);

	med_txt("DIGGER is available for all PC,",5,16,7);
	med_txt("XT, AT and compatibles.",5,17,7);
	med_txt("A special version is also ",5,18,7);
	med_txt("available for the IBM PCjr.",5,19,7);

	med_txt("Thank You for your support.",5,22,7);

	/* determine system clock speed */
	find_speed();

	med_txt("Press any key ...",21,24,7);
	flag=FALSE;
	while(!flag) {
		flag=start_wait();	/* exit if a key has been pressed */
	};

#ifdef	FREEDEMO

	clear(0);

	med_txt("This is a demonstration copy",5,1,7);
	med_txt("of DIGGER. The game is completely",5,2,7);
	med_txt("functional except that only the",5,3,7);
	med_txt("first two rounds are enabled and",5,4,7);
	med_txt("high scores are not saved.",5,5,7);

	med_txt("This demonstration program may",5,7,7);
	med_txt("be copied and passed on to your",5,8,7);
	med_txt("friends.  We hope that you and",5,9,7);
	med_txt("your friends will enjoy DIGGER.",5,10,7);

	med_txt("To purchase DIGGER along with a",5,12,7);
	med_txt("full color illustrated manual,",5,13,7);
	med_txt("please send $39.95 to:",5,14,7);

	med_txt("Windmill Software Inc.",8,17,7);
	med_txt("2209 Leominster Drive",8,18,7);
	med_txt("Burlington, Ontario",8,19,7);
	med_txt("CANADA   L7P 3W8",8,20,7);

	
	med_txt("Press any key ...",21,24,7);
	flag=FALSE;
	while(!flag) {
		flag=start_wait();	/* exit if a key has been pressed */
	};






	clear(0);

	med_txt("INSTRUCTIONS",15,1,7);

	med_txt("Before starting game, press 'ESC'",3,3,7);
	med_txt("to select number of players.",3,4,7);

	med_txt("Use the UP, DOWN, LEFT & RIGHT",3,6,7);
	med_txt("arrow keys to maneuvre your DIGGER",3,7,7);
	med_txt("Mobile through the mining site",3,8,7);
	med_txt("following the old mining shafts",3,9,7);
	med_txt("and tunnelling new paths.",3,10,7);

	med_txt("Press Function Key 'F1' to throw",3,12,7);
	med_txt("fireballs at the approaching",3,13,7);
	med_txt("Nobbins and Hobbins.",3,14,7);

	med_txt("'F7' controls background music.",3,16,7);
	med_txt("'F9' controls all sound.",3,17,7);
	med_txt("SPACE BAR suspends game.",3,18,7);
	med_txt("Ctrl left & right arrow keys",3,19,7);
	med_txt("reposition the screen.",3,20,7);

	med_txt("Press any key ...",21,24,7);
	flag=FALSE;
	while(!flag) {
		flag=start_wait();	/* exit if a key has been pressed */
	};




	clear(0);

	med_txt("SCORING",15,0,7);

	med_txt("During normal play, you destroy",3,2,7);
	med_txt("Nobbins & Hobbins with your fireball",3,3,7);
	med_txt("or by pushing the gold sacks down",3,4,7);
	med_txt("a mine shaft.  Don't forget to scoop",3,5,7);
	med_txt("up the gold when the sack breaks.",3,6,7);

	med_txt("After all the Nobbins & Hobbins have",3,8,7);
	med_txt("emerged, cherries appear. Scooping up",3,9,7);
	med_txt("the cherries begins a bonus round.",3,10,7);

	med_txt("Nobbins & Hobbins .....  250",5,12,7);
	med_txt("Emeralds ..............   25",5,13,7);
	med_txt("Gold Nuggets ..........  500",5,14,7);

	med_txt("Bonus points:",3,16,7);
	med_txt("8 Emeralds in a row ...  250",5,17,7);
	med_txt("Cherries .............. 1000",5,18,7);
	med_txt("Nobbins & Hobbins ... 200, 400, 800",5,19,7);

	med_txt("With every 20,000 points, you earn",3,21,7);
	med_txt("an extra life.",3,22,7);

	med_txt("Press any key ...",21,24,7);
	flag=FALSE;
	while(!flag) {
		flag=start_wait();	/* exit if a key has been pressed */
	};









#endif

	while(TRUE) {	/* continuous loop for game */
		init_th();	/* turn off sounds */
		sp_table(table);
		init_images();
		init_joy();
/* demo - title page display */
		clear(0);
		gentitle();  
		line(280,199,319,199,0);
		prints("D I G G E R",100,0,3);
		show_players();
		show_scores();
		demo_time=0;
		endgame=FALSE;		
		flag=FALSE;
		while(!flag) {

			flag=start_wait();	/* exit demo if a key has been pressed */
			if(endgame) {	/* if ESC is pressed then toggle between 1 & 2 player game */
				toggle_players();
				show_players();
				keychr=0;
				endgame=FALSE;
			}

			/* clear demo area of screen */
			if(demo_time==0) {
				for(loop=54; loop<174; loop+=12)
					prints("            ",164,loop,0);
			}

			/* bring on first bug */
			if(demo_time==50) {
				sp_place(8,292,63);	/* place normal bug */
				x=292; 	/* set up starting position */	
			}
			if(demo_time>50 && demo_time<=77) {
				x-=4;
				draw_bug(0,NORMAL,LEFT,x,63);
			}
			if(demo_time>77)
				draw_bug(0,NORMAL,RIGHT,184,63);
			if(demo_time==83)
				prints("NOBBIN",216,64,RED);


			/* bring on second bug */
			if(demo_time==90) {
				sp_place(9,292,82);
				draw_bug(1,ADVANCING,LEFT,292,82);
				x=292; 	/* set up starting position */	
			}
			if(demo_time>90 && demo_time<=117) {
				x-=4;
				draw_bug(1,ADVANCING,LEFT,x,82);
			}
			if(demo_time>117) 
				draw_bug(1,ADVANCING,RIGHT,184,82);

			if(demo_time==123)
				prints("HOBBIN",216,83,RED);

			/* bring on digger */
			if(demo_time==130) {
				sp_place(0,292,101);
				draw_man(LEFT,292,101,TRUE);
				x=292; 	/* set up starting position */	
			}
			if(demo_time>130 && demo_time<=157) {
				x-=4;
				draw_man(LEFT,x,101,TRUE);
			}
			if(demo_time>157)
				draw_man(RIGHT,184,101,TRUE);
			if(demo_time==163)
				prints("DIGGER",216,102,RED);

			/* start bag display */
			if(demo_time==178) {
				sp_place(1,184,120);
				draw_bag(1,BAG_0,184,120);
			}
			if(demo_time==183) {
				prints("GOLD",216,121,RED);
			}

			/* start emerald display */
			if(demo_time==198) {
				put_coin(184,141);
			}
			if(demo_time==203) {
				prints("EMERALD",216,140,RED);
			}

			/* start bonus display */
			if(demo_time==218) {
				draw_bonus(184,158);
			}
			if(demo_time==223) {
				prints("BONUS",216,159,RED);
			}

			/* increment counter with wrap around */
			demo_time++;
			if(demo_time>250)
				demo_time=0;

			
			for(loop=0; loop<45; loop++) {
				sleep(1);
				if(keychr && !(keychr & 0x80)) {
					break;
				}
			}

		}


/* set up player 0 variables */
		game[0][LEVEL]=1; game[0][LIVES]=3;

/* set up player 1 variables if being used */
		if(num_players==2) {
			game[1][LEVEL]=1; game[1][LIVES]=3;
		}
		else
			game[1][LIVES]=0;

/* initialize all tasks for boath players */
		clear(0);	/* clear screen */

		player=0;	
		init_level();
		player=1;
		init_level();

		rst_scores();	/* reset scores for this round */

		new_round=TRUE;	/* screen is redrawn if new_round=TRUE */
		if(num_players==2)
			new_player=TRUE;	/* player up message at top of screen is displayed if new_player=TRUE */
		player=0;	/* start with first player */

		while(game[0][LIVES] || game[1][LIVES]) {	/* loop untill boath players are dead */
	
			game[player][DEAD]=FALSE;

			while(!game[player][DEAD] && game[player][LIVES]) {
				rst_images();	/* restore origional images */
				play_round();	/* do game untill player dies */
       			}


			if(player==0) {
				if(game[1][LIVES]) {	/* check if other player is still alive */
					new_player=TRUE;
					new_round=TRUE;
					player=1;
				}
			}
			else {
				if(game[0][LIVES]) {	/* check if player 0 is still alive */
					new_player=TRUE;
					new_round=TRUE;
					player=0;
				}
			}

		}

	}

}


med_txt(string,x,y,color)
int	x,y,color;
char	*string;
{

	int	n=0;

	ttyrst(y,x);

	while(string[n]) {
		ttywrt(string[n++],color);
	}
}



play_round()
{
	int	loop;


	if(new_round) {	/* redraw screen if players have changed or first play */
		new_round=FALSE;
		start_level();	/* redraw screen */
		if(new_player) {
			new_player=FALSE;
			strcpy(text,"PLAYER ");	/* set up string */
			if(player==0)
				strcat(text,"1");
			else
				strcat(text,"2");
			blank_top();	/* clear top of screen */

			for(loop=0; loop<15; loop++) {
				prints(text,108,0,1);
				flash_score(1);
				sleep(20);
				prints(text,108,0,2);
				flash_score(2);
				sleep(20);
				prints(text,108,0,3);
				flash_score(3);
				sleep(20);
			}
			print_present_score();	/* display boath players score in brown */
			add_score(0);
		}
	}
	else
		restart_level();

	keychr=0;	/* clear keyboard */ 
	prints("        ",108,0,3); 
	init_tf();	/* print scores */
	print_lives();	/* show lives left */
	init_music(1);	/* start regular music */
	joystk();	/* clear any old keyboard charaters before beginning */
	while(!game[player][DEAD] && !game[player][DONE_LEVEL]) {			

		plot_count=0;
		task_c();	/* move man */
		
		task_a();	/* move bugs */ 
		
		task_b();	/* check for money bag movement */
		
		if(plot_count<8) {	/* delay for constant speed */
			for(loop=(8-plot_count)*5; loop>0; loop--)
				sleep(1);
		}

		if(plot_count>8)	/* extra delay for very heavy movement times */
			delay_bugs(plot_count-8);

		exit_check();
                        	
		done_check();	/* see if all coins are eaten */
	}
	end_tc();	/* erase old man */
	rst_music();	/* end background music */

	loop=20;	/* allows some movement to finish sound effects */
	while(get_moving_bags() || loop) {
		if(loop)
			loop--;
		plot_count=0;
		task_b(); 	/* allow bags to fall */
		task_c();	/* move any missiles */		
		task_a();	/* keep bugs moving */
		
		if(plot_count<8) {	/* delay for constant speed */
			for(loop=(8-plot_count)*5; loop>0; loop--)
				sleep(1);
		}
	}


	init_th();	/* end all sounds */
	era_missile();	/* remove any missiles left on screen */

	era_bonus();	/* erase any bonus markers */
	end_tb();
	end_tg();
	end_ta();	/* erase bugs */

	if(game[player][DONE_LEVEL])
		snd_end();	/* sound for end of screen */

	if(count_coins()==0) {	/* initialize level now so that coins destroyed by mans death alse result
				   in advancing to the next level */
		game[player][LEVEL]++;	/* advance to next level */
		if(game[player][LEVEL]>1000)
			game[player][LEVEL]=1000;
		init_level();  
	}


#ifdef	FREEDEMO

	if(game[player][LEVEL]>2) {
		game[player][LIVES]=0;
		check_hi_score();
	}
#endif

		
	if(game[player][DEAD]) {

#ifdef	DEBUG	if(!fire)	/* ************************** */
#endif
		game[player][LIVES]--;
		print_lives();	/* update display */
		if(game[player][LIVES]<1)
			check_hi_score();

	}

	if(game[player][DONE_LEVEL]) {
		game[player][LEVEL]++;	/* advance to next level */
		if(game[player][LEVEL]>1000)
			game[player][LEVEL]=1000;
		init_level();  
	}

}


toggle_players()
{

	if(num_players==1)
		num_players=2;
	else
		num_players=1;

}


init_level()
{
	game[player][DONE_LEVEL]=FALSE;
	init_tg();
	init_td();
	init_tb();
	new_round=TRUE;	/* screen must be redrawn */
}



start_level()
{
	init_images();	/* redefine all sprites as inactive with origional shapes */
	start_tg();	/* draw background */
	start_tb();	/* draw money bags */
	start_td();	/* draw coins */
	init_tc();	/* draw man in starting position */
	init_ta();	/* create first bug */
#ifdef	DEBUG
	cprintf(" level %d  screen %d \n\r",get_level(),get_screen());
#endif
}


restart_level()
{
	rst_images();	/* redefine sprites with origional shapes */
	init_tc();	/* draw man in starting position */
	init_ta();	/* create first bug */
#ifdef	DEBUG
	cprintf(" level %d  screen %d \n\r",get_level(),get_screen());
#endif
}



done_check()
{
/* check if coins are eaten or all bugs are dead & man is not in the process of dying */
	if((count_coins()==0 || bugs_left()==0) && get_m_status() )
		game[player][DONE_LEVEL]=TRUE;
	else
		game[player][DONE_LEVEL]=FALSE;

}


inc_plot()
{	/* counter for # of screen plotd per loop of game */
	plot_count++;
	
}




blank_top()
{
	prints("                          ",0,0,3);
	prints(" ",308,0,3);

}


get_screen()
{	/* return a screen value 1 - 8  if level is greater than 8 then use levels 5-8 over */
	int	temp;

	temp=get_level();
	if(temp>8) {
		temp= temp%4;
		temp+=5;
	}

	return(temp);

}

get_dificulty()
{	/* levels of dificulty range from 1 - 10 */

	if(game[player][LEVEL]>10)
		return(10);
	else
		return(game[player][LEVEL]);

}


get_level()
{
	return(game[player][LEVEL]);
}



make_level(x)
{
	game[player][LEVEL]=x;

}


get_lives(num)
{
	if(num==1)
		return(game[0][LIVES]);
	else
		return(game[1][LIVES]);


}


add_life(num)
int	num;
{
	if(num==1)
		game[0][LIVES]++;
	else
		game[1][LIVES]++;

	snd_life();	/* sound for new life */
}



make_dead(status)
int	status;
{
	game[player][DEAD]=status;

}


get_player()
{
	return(player);

}

players()
{

	return(num_players);

}


exit_check()
{	/* check for a hold condition - also exit to dos during debug */
	int	a;

/*	if(endgame) {	/*	removed for sale vertion  */
		rst_sound();
		kb_rst();  
		sn2_stop();
		_exit();
	}
*/
	if(keychr==57 || keychr==185) {	/* check for a hold condition */
		init_pause();	/* turn off background music */
		set_sound(40);	/* turn off sound */ 
		sn2_off();
		blank_top();	/* erase top line */
		prints("PRESS ANY KEY",80,0,GREEN);	/* display message at top of screen */
		while(keychr==57)	/* hold game while pressing space key */
			a=a;
		while(keychr & 0x80)	/* hold game while last character was a key up */
			a=a;
		blank_top();
		print_present_score();	/* reprint top status line */
		add_score(0);	/* change color of present players score */
		print_lives();
		sleep(200);	/* delay for keyboard debounce */
		keychr=0;	/* reset key incase game was restarted with a space key */
	}
	else
		rst_pause();	/* turn on background music */
}


