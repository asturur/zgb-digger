/*
*****************************************************************************************************
*****************************************************************************************************
*****                                                                                           *****
*****                  Task C   -   Man Movement                                                *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****************************************************************************************************
*****************************************************************************************************
*/

#include	"digger.h"


/* all man related variables are re-initialized for eack round & screen */

int	man_x, man_y, m_x_pos, m_y_pos, m_x_rem, m_y_rem;
int	m_dir, new_m_dir, m_last_dir, m_wait, m_alive, m_active;

int	mis_ready, mis_wait, mis_x, mis_y, mis_dir, mis_explode;

int	dead_type, dead_num, dead_body, dead_wait, fall_count;

int	bonus_flash, bonus_time, bonus_active, bonus_on, bonus_count;

int	coin_time;

/*	dead_type indicates the present death sequence i.e. falling due to money bag ext.
	dead_num is used with money bag death to show which gag is killing man
	dead_body indicates the mans body for the death roll
	dead_wait is the wait between the final death roll segments
*/

init_tc()
{	
	m_dir=LEFT; new_m_dir=LEFT; m_last_dir=RIGHT; /* start with no movement */
	m_x_pos=M_X_START; m_y_pos=M_Y_START; 
	man_x=m_x_pos*20+X_OFFSET; man_y=m_y_pos*18+Y_OFFSET;
	m_x_rem=0; m_y_rem=0;
	m_alive=TRUE; dead_type=MONEY_BAG; m_active=TRUE;
	m_wait=0;	/* delay before next man move */
	fall_count=0;	/* counter used to spin man as he falls to death */
	sp_place(0,man_x,man_y);

	mis_ready=TRUE;	/* show that missle is ready to fire */
	mis_wait=0;	/* no wait to fire missle */
	mis_explode=0;	/* reset explotion counter */
	fires=FALSE;	/* clear fire key */

	bonus_active=FALSE;	/* start with bonus not active */
	bonus_on=FALSE;		/* bonus mode not on */

	coin_time=0;	/* timer to indicate sucessive coin eating */

}


end_tc()
{
	sp_erase(0);	/* erase man from screen */
	m_active=FALSE;	/* stop man from being moved */
}


task_c()
{

/* move missile */	
	if(mis_explode)
		explode_missile();
	else
		move_missile();

/* move man */
	if(m_active) {
		if(m_alive) {	/* do normal movement for live man */
			if(m_wait) {
				draw_man(m_dir,man_x,man_y,(mis_ready && !mis_wait) );
				inc_plot();	/* report a screen plot */
				m_wait--;
			}
			else
				move_man();
		}
		else {	/* move man through dying sequence */
			dead_move();
		}
	}

/* check bonus operation */
	if(bonus_on && m_alive) {
		if(bonus_time) {
			bonus_time--;
			if(bonus_flash || bonus_time<20) {	/* flash for start & end of bonus time */
				bonus_flash--;
				if(bonus_time & 1) {
					palette(0);	/* back to normal */
					init_chase();	/* sound for chase start */
				}
				else {
					palette(1);	/* change screen color */
					init_chase();	/* sound for chase start */
				}
				if(bonus_flash==0) {
					init_music(0);
					rst_chase();	/* end sound for chase change */
					palette(1);	/* change screen color */
				}
			}
		}
		else {
			end_bonus();
			rst_chase();	/* end sound for chase change */
			init_music(1);	/* restart regular music */
		}

	}

	if(bonus_on && (!m_alive)) {
		end_bonus();
		rst_chase();	/* end sound for chase change */
		init_music(1);	/* restart regular music */
	}

/* adjuct counter for coin eating */
	if(coin_time>0)
		coin_time--;

}



move_missile()
{
	int	colision, mask, loop, point_stat;

	if(mis_ready) {
		if(mis_wait) {
			mis_wait--;	/* wait untill timer has gone to zero */
		}
		else {
			if(get_button() && get_m_status() ) {	/* fire if button has been pressed & man is alive */
				mis_wait=60+get_dificulty()*3;
				mis_ready=FALSE;	/* show missle is active */
				switch(m_last_dir) {
				case RIGHT:
					mis_x=man_x+8; mis_y=man_y+4;	/* set up initial missle position */
					break;
				case LEFT:
					mis_x=man_x; mis_y=man_y+4;	/* set up initial missle position */
					break;
				case UP:
					mis_x=man_x+4; mis_y=man_y;	/* set up initial missle position */
					break;
				case DOWN:
					mis_x=man_x+4; mis_y=man_y+8;	/* set up initial missle position */
					break;
				}
				mis_dir=m_last_dir; 	/* set up initial missle direction */
				sp_place(15,mis_x,mis_y);	/* place missle */
				init_fire();	/* start missle sound */
			}

		}
	}
	else {	/* move an active missle */
		switch(mis_dir) {
		case RIGHT:
			mis_x+=8;
			point_stat=ppoint(mis_x,mis_y+4);
			point_stat|=ppoint(mis_x+4,mis_y+4);
			break;
		case LEFT:
			mis_x-=8;
			point_stat=ppoint(mis_x,mis_y+4);
			point_stat|=ppoint(mis_x+4,mis_y+4);
			break;
		case UP:
			mis_y-=7;
			point_stat=ppoint(mis_x+4,mis_y) & 0xc0;
			point_stat|=ppoint(mis_x+4,mis_y+1) & 0xc0;
			point_stat|=ppoint(mis_x+4,mis_y+2) & 0xc0;
			point_stat|=ppoint(mis_x+4,mis_y+3) & 0xc0;
			point_stat|=ppoint(mis_x+4,mis_y+4) & 0xc0;
			point_stat|=ppoint(mis_x+4,mis_y+5) & 0xc0;
			point_stat|=ppoint(mis_x+4,mis_y+6) & 0xc0;
			break;
		case DOWN:
			mis_y+=7;
			point_stat=ppoint(mis_x,mis_y) & 3;
			point_stat|=ppoint(mis_x,mis_y+1) & 3;
			point_stat|=ppoint(mis_x,mis_y+2) & 3;
			point_stat|=ppoint(mis_x,mis_y+3) & 3;
			point_stat|=ppoint(mis_x,mis_y+4) & 3;
			point_stat|=ppoint(mis_x,mis_y+5) & 3;
			point_stat|=ppoint(mis_x,mis_y+6) & 3;
			break;
		}

		
		colision=draw_missile(mis_x,mis_y,0);
		inc_plot();	/* report a screen plot */

		if(colision & 0x3f00) {
			mask=0x100;
			for(loop=0; loop<6; loop++) {		
				if(colision & mask) {
					era_bug(loop);	/* remove bug */
					score_bug();	/* score for killing bug with missile */
					mis_explode=1;	/* start explosion */
				}
				mask<<=1;
			}
		}
		if(colision & 0x40fe)	/* hit bonus or money */
			mis_explode=1;	/* start explosion */
	
  /* check for off screen or end of tunnel conditions */
		switch(mis_dir) {
		case RIGHT:
			if(mis_x>(X_LAST+4))
				mis_explode=1;	/* start explosion */
			else {
				if(point_stat && !colision) {
					mis_explode=1;	/* start explosion */
					mis_x-=8;	/* move missile back to origional position */
					draw_missile(mis_x,mis_y,0);
				}
			}
			break;
		case LEFT:
			if(mis_x<(X_FIRST+4))
					mis_explode=1;
			else {
				if(point_stat && !colision) {
					mis_explode=1;	/* start explosion */
					mis_x+=8;	/* move missile back to origional position */
					draw_missile(mis_x,mis_y,0);
				}
			}
			break;
		case UP:
			if(mis_y<(Y_FIRST+3))
					mis_explode=1;
			else {
				if(point_stat && !colision) {
					mis_explode=1;	/* start explosion */
					mis_y+=7;	/* move missile back to origional position */
					draw_missile(mis_x,mis_y,0);
				}
			}
			break;
		case DOWN:
			if(mis_y>(Y_LAST+3))
				mis_explode=1;
			else {
				if(point_stat && !colision) {
					mis_explode=1;	/* start explosion */
					mis_y-=7;	/* move missile back to origional position */
					draw_missile(mis_x,mis_y,0);
				}
			}
			break;
		}


	}
}

explode_missile()
{
	switch(mis_explode) {
	case 1:
		snd_explode();	/* start explode sound */
		draw_missile(mis_x,mis_y,1);
		inc_plot();	/* report a screen plot */
		mis_explode=2;
		break;
	case 2:
		draw_missile(mis_x,mis_y,2);
		inc_plot();	/* report a screen plot */
		mis_explode=3;
		break;
	case 3:
		draw_missile(mis_x,mis_y,3);
		inc_plot();	/* report a screen plot */
		mis_explode=4;
		break;
	default:
		era_missile();
		mis_explode=0;
		break;
	}
}



era_missile()
{
	if(!mis_ready) {
		mis_ready=TRUE;
		sp_erase(15);
		rst_fire();	/* stop missle sound */
	}

}


move_man()
{
	int	m_key_dir, colision, m_x_old, m_y_old, flag, temp;
	
/* update keyboard & joystick variables */
	joystk();
	m_key_dir=check_turn();


/* set up the next direction of travel - do not change present direction */
	switch(m_key_dir) {
	case UP:
		new_m_dir=UP;
		break;
	case DOWN:
		new_m_dir=DOWN;
		break;
	case RIGHT:
		new_m_dir=RIGHT;
		break;  	
	case LEFT:
		new_m_dir=LEFT;
		break;
	default:
		new_m_dir=STOP;
	}


/* check for a change in Y direction */
	if(m_x_rem==0 && (new_m_dir==UP || new_m_dir==DOWN)) {
		m_dir=new_m_dir;
		m_last_dir=new_m_dir;
	}

/* check for a change in X direction */
	if(m_y_rem==0 && (new_m_dir==LEFT || new_m_dir==RIGHT)) {
		m_dir=new_m_dir;
		m_last_dir=new_m_dir;
	}


/* if no direction is requested then stop, else continue in the old direction */
	if(m_key_dir==STOP) {
		m_dir=STOP;
	}
	else
		m_dir=m_last_dir;


/* check if man is going to go past screen edges */
	if((man_x==X_LAST && m_dir==RIGHT) || (man_x==X_FIRST && m_dir==LEFT)
	    || (man_y==Y_LAST && m_dir==DOWN) || (man_y==Y_FIRST && m_dir==UP))
		m_dir=STOP;


/* erase background in front of man then move man */
	m_x_old=man_x; m_y_old=man_y;	/* store old location incase mans move is bad */

        if(m_dir!=STOP)
		era_background(man_x,man_y,m_dir);	/* update background matrix */
	switch(m_dir) {
	case RIGHT:
		era_right(man_x,man_y);
		man_x+=4;
		break;  	
	case LEFT:
		era_left(man_x,man_y);
		man_x-=4;
		break;
	case UP:
		era_up(man_x,man_y);
		man_y-=3;
		break;
	case DOWN:
		era_down(man_x,man_y);
		man_y+=3;
		break;
	}

/* remove or redraw coins in mans path */
	colision=coin_check((man_x-X_OFFSET)/20,(man_y-Y_OFFSET)/18,(man_x-X_OFFSET)%20,(man_y-Y_OFFSET)%18,m_dir);
	if(colision) {	/* man has eaten coin */
		score_coin();
		snd_coin();	/* sound for coin being eaten */
		snd_2_coin(coin_time);
		coin_time=9;	/* timer to indicate sucessive coin eating */
	}

/* check for colision */
	colision=draw_man(m_last_dir,man_x,man_y,(mis_ready && !mis_wait) );	/* redraw stopped man in new position on the screen */
	inc_plot();	/* report a screen plot */

	if(colision & 0xfe & get_active_bags()) {	/* colision detection for money bags */
		if(m_dir==RIGHT || m_dir==LEFT) {	/* move bags if going left or right */
			flag=move_all_bags(m_dir,colision);	/* move bags touched by the man */
			m_wait++;	/* make man miss a move if hitting money bags */
		}
		else {
			if(!money_check(colision))	/* check if any broken money bags are touched */
				flag=FALSE;	/* dont allow bags to be moved up or down if bag is not broken */
		}


		if(!flag) {	/* if bags would not move - return man to origional position */

			man_x=m_x_old; man_y=m_y_old;	/* restore old coordinates */
			draw_man(m_dir,man_x,man_y,(mis_ready && !mis_wait) );	/* redraw man in new position on the screen */
			inc_plot();	/* report a screen plot */

			m_last_dir=get_rev(m_dir);	/* reverse mans direction to prevent him from becoming stuck */

		}			
	}

	/* colision between bug & man is ONLY checked in ta.  This prevents dead bugs from killing man */
	if(colision & 0x3f00) {
		if(get_bonus() ) {	/* bonus is on */
			temp=rem_bugs(colision);
			while(temp) {	/* score for each bug removed */
				snd_bug();	/* start sound for bonus bug eaten */
				bonus_score();
				temp--;
			}
		}
	}

	if(colision & 0x4000) {
		score_extra();	/* score for getting bonus */
		start_bonus();	/* activate bonus operation */
	}

/* update variables for next time */
	m_x_pos=(man_x-X_OFFSET)/20; 
	m_x_rem=(man_x-X_OFFSET)%20;
	m_y_pos=(man_y-Y_OFFSET)/18;
	m_y_rem=(man_y-Y_OFFSET)%18;

}


dead_move()
{
	int	colision;

	switch(dead_type) {
	case MONEY_BAG:
		if(get_y_bag(dead_num)+6>man_y)
			man_y=get_y_bag(dead_num)+6;

		draw_man(FALL_9,man_x,man_y,FALSE);
		inc_plot();	/* report a screen plot */

		if(get_d_bag(dead_num)==STOP) {
			snd_dead();	/* start dead sound */
			dead_wait=5;
			dead_type=MAN;
			dead_body=0; 
			man_y-=6;
		}
		break;
	case MAN:
		if(dead_wait) {
			dead_wait--;
			break;
		}
		switch(dead_body) {
		case 0:
			init_music(2);	/* start music for death song */
			colision=draw_man(FALL_5,man_x,man_y,FALSE);
			inc_plot();	/* report a screen plot */
			if(colision & 0x3f00)
				rem_bugs(colision);	/* remove bugs hit by grave - prevents flashing */
			dead_body=1;
			dead_wait=2;
			break;
		case 1:
			draw_man(FALL_4,man_x,man_y,FALSE);
			inc_plot();	/* report a screen plot */
			dead_body=2;
			dead_wait=2;
			break;
		case 2:
			draw_man(FALL_3,man_x,man_y,FALSE);
			inc_plot();	/* report a screen plot */
			dead_body=3;
			dead_wait=2;
			break;
		case 3:
			draw_man(FALL_2,man_x,man_y,FALSE);
			inc_plot();	/* report a screen plot */
			dead_body=4;
			dead_wait=2;
			break;
		case 4:
			draw_man(FALL_1,man_x,man_y,FALSE);
			inc_plot();	/* report a screen plot */
			dead_type=DEAD;
			if(music)
				dead_wait=60;
			else
				dead_wait=10;
			break;
		}
		break;
	case BUG:

		dead_wait=0;
		dead_type=BOUNCE;
		dead_body=0;
		break;

	case BOUNCE:
		switch(dead_body) {
		case 0:
			draw_man(FALL_9,man_x,man_y-3,FALSE);
			inc_plot();	/* report a screen plot */
			dead_body=1;
			snd_dead();	/* start dead sound */
			break;
		case 1:
			draw_man(FALL_9,man_x,man_y-5,FALSE);
			inc_plot();	/* report a screen plot */
			dead_body=2;
			break;
		case 2:
			draw_man(FALL_9,man_x,man_y-6,FALSE);
			inc_plot();	/* report a screen plot */
			dead_body=3;
			break;
		case 3:
			draw_man(FALL_9,man_x,man_y-6,FALSE);
			inc_plot();	/* report a screen plot */
			dead_body=4;
			break;
		case 4:
			draw_man(FALL_9,man_x,man_y-5,FALSE);
			inc_plot();	/* report a screen plot */
			dead_body=5;
			break;
		case 5:
			draw_man(FALL_9,man_x,man_y-3,FALSE);
			inc_plot();	/* report a screen plot */
			dead_body=6;
			break;
		case 6:
			draw_man(FALL_9,man_x,man_y,FALSE);
			rst_music();
			inc_plot();	/* report a screen plot */
			dead_wait=5;
			dead_body=0;
			dead_type=MAN;
			break;
		}
		break;

	case DEAD:
		if(dead_wait) {
			dead_wait--;
			break;
		}
		make_dead(TRUE);	/* show tz that man is dead */
		break;
	}

}




begin_bonus()
{	/* allows bonus to be activated */
	bonus_active=TRUE;
	draw_bonus(20*14+X_OFFSET,Y_OFFSET);	/* show bonus marker on screen */

}


start_bonus()
{	/* activates bonus mode */
	bonus_on=TRUE;
	era_bonus();	/* remove marker from screen */
	palette(1);	/* change screen color */
	bonus_time=250-get_dificulty()*20;
	bonus_flash=20;
	bonus_count=1;	/* score multiplyer */
}



end_bonus()
{	/* end bonus mode */
	bonus_on=FALSE;
	palette(0);	/* normal screen color */

}


era_bonus()
{
	if(bonus_active) {
		bonus_active=FALSE;
		sp_erase(14);
	}
	palette(0);	/* normal screen color */
}



bonus_score()
{
	add_score(bonus_count*200);
	bonus_count <<=1;	/* double score next time */
}


get_bonus()
{	/* return bonus indicator */
	return(bonus_on);

}



get_rev(dir)
int	dir;
{

	switch(dir) {	/* reverse direction */
	case RIGHT:
		dir=LEFT;
		break;
	case LEFT:
		dir=RIGHT;
		break;
	case UP:
		dir=DOWN;
		break;
	case DOWN:
		dir=UP;
		break;
	}

	return(dir);

}


get_man_block(x,y)
int	x, y;
{
	if((m_dir==UP || m_dir==DOWN) && ( man_x-X_OFFSET)/20==x && ( (man_y-Y_OFFSET)/18==y || (man_y-Y_OFFSET)/18+1==y ) ) 
		return(TRUE);
	else
		return(FALSE);

}


get_m_x()
{
	return(man_x);

}



get_m_y()
{
	return(man_y);

}


get_m_status()
{

	return(m_alive);

}


kill_man(d_type,num)
int	d_type, num;
{
	if(dead_type!=MAN && dead_type!=DEAD && dead_type!=BUG) {
		rst_music();	/* stop music */
		m_alive=FALSE;
		dead_type=d_type;
		dead_num=num;
	}
}

rst_mwait()
{
	m_wait=0;

}


ppoint(x,y)
{	/* find screen location color */
	int	ax, temp;

	ax=(y & 0xfe)*40;
	if(y & 1)
		ax+=0x2000;	

	ax+=(x>>2);

	defseg=0xb800;
	temp=peek(ax);

	return(temp);

}


