/*
***************************************************************************************************
***************************************************************************************************
**                                                                                               **
**                                                                                               **
**                                  M r .    D i g g e r                                         **
**                                                                                               **
**         T a s k    H    -    S o u n d    G e n e r a t i o n                                 **
**                                                                                               **
**                                                                                               **
**                                                                                               **
***************************************************************************************************
***************************************************************************************************
*/

#include	"digger.h"
#include	"notes.h"

/* note length definitions */
#define		NS	1
#define		NE	2
#define		NQ	4
#define		NH	8
#define		NW	16

/* note shape definitions */
#define		ATTACK	0
#define		HOLD	1
#define		DECAY	2

#define TIMER_MODE	0x43
#define TIMER_PORT	0x42
#define INTERUPT_PORT	0x40
#define PORT_B		0x61
#define TIMER_ENABLE	0x01
#define SPEAKER_ENABLE	0x02
#define	ON		1
#define	OFF		0
#define	PULSE		1
#define	SQUARE		2


int	sound_freq, sound_on, sound2_on, sn2_stat, sn2_mode;
int	t_period, p_period, p_width, volume;

int	fall_period, fall_stat, fall_toggle, fall_num;
int	loose_stat, loose_type;
int	fire_stat, fire_freq, fire_pause;
int	chase_stat, chase_count, chase_toggle;
int	exp_stat, exp_period, exp_count;
int	break_stat, break_count, break_period;
int	coin_count, coin_stat;
int	coin_2_count, coin_2_stat, coin_2_period, coin_2_type;
int	bug_stat, bug_period, bug_c_1, bug_c_2;
int	dead_stat, dead_count, dead_period;
int	life_stat, life_count;
int	end_stat, end_count, end_loop;

int	money_stat, money_toggle, money_1, money_2, money_count;
int	m_pointer, m_type, m_count, m_note, m_stat, m_pause;
int	attack_level, attack_rate, hold_level, hold_rate, hold_count, decay_rate;
int	envelope, env_position;

int	time_reg=1;
int	timetemp;	/* used to equilize time on diferent speed CPU's */
int	time_mul;	/* used to equilize sound pulse width on diferent speed CPU's */


/*	tico-tico

	C5,NQ, AS4,NQ, C5,NQ, G4,NQ, DS4,NQ, G4,NQ, C4,NH,
	C5,NQ, AS4,NQ, C5,NQ, G4,NQ, DS4,NQ, G4,NQ, C4,NH,
	C5,NQ, D5,NQ, DS5,NQ, DS5,NQ, DS5,NQ, C5,NQ, D5,NQ, D5,NQ, D5,NQ,
	AS4,NQ, C5,NQ, C5,NQ, C5,NQ, B4,NQ, C5,12,

	C5,NQ, AS4,NQ, C5,NQ, G4,NQ, DS4,NQ, G4,NQ, C4,NH,
	C5,NQ, AS4,NQ, C5,NQ, G4,NQ, DS4,NQ, G4,NQ, C4,NH,
	C5,NQ, D5,NQ, DS5,NQ, DS5,NQ, DS5,NQ, C5,NQ, D5,NQ, D5,NQ, D5,NQ,
	AS4,NQ, C5,NQ, C5,NQ, C5,NQ, B4,NQ, C5,12,
*/


int	music0[] = { 	/* music for bonus round */
	C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, F4,NQ, G4,NQ, A4,NQ,
	C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, C4,NQ, F4,NE, A4,NE, G4,NQ, E4,NQ, C4,NQ,
	C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, F4,NQ, G4,NQ, A4,NQ,
	F4,NE, A4,NE, C5,10, AS4,NE, A4,NE, G4,NE, F4,NQ, A4,NQ, F4,NQ, 

	C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, F4,NQ, G4,NQ, A4,NQ,
	C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, C4,NQ, F4,NE, A4,NE, G4,NQ, E4,NQ, C4,NQ,
	C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, C4,NQ, C4,NE, C4,NE, F4,NQ, G4,NQ, A4,NQ,
	F4,NE, A4,NE, C5,10, AS4,NE, A4,NE, G4,NE, F4,NQ, A4,NQ, F4,NQ, 

	A4,NE, A4,NE, A4,NQ, A4,NE, A4,NE, A4,NQ, A4,NE, A4,NE, A4,NQ, 
	D5,NQ, A4,NQ, D5,NQ, A4,NQ, D5,NQ, A4,NQ, G4,NQ, F4,NQ, E4,NQ, D4,NQ,
	A4,NE, A4,NE, A4,NQ, A4,NE, A4,NE, A4,NQ, A4,NE, A4,NE, A4,NQ, 
	D5,NQ, A4,NQ, D5,NQ, A4,NQ, D5,NQ, C5,NQ, B4,NQ, C5,NQ, B4,NQ, C5,NQ, 

	A4,NE, A4,NE, A4,NQ, A4,NE, A4,NE, A4,NQ, A4,NE, A4,NE, A4,NQ, 
	D5,NQ, A4,NQ, D5,NQ, A4,NQ, D5,NQ, A4,NQ, G4,NQ, F4,NQ, E4,NQ, D4,NQ,
	A4,NE, A4,NE, A4,NQ, A4,NE, A4,NE, A4,NQ, A4,NE, A4,NE, A4,NQ, 
	D5,NQ, A4,NQ, D5,NQ, A4,NQ, D5,NQ, C5,NQ, B4,NQ, C5,NQ, B4,NQ, C5,NQ, 

		32100 };



int	music1[] = { 	/* music for normal movement of man */

	D4,NE, C4,NE, // 0 - 1
	D4,NE, A3,NE, F3,NE, A3,NE, D3,NQ, D4,NE, C4,NE, // 2 - 9
	D4,NE, A3,NE, F3,NE, A3,NE, D3,NQ, D4,NE, E4,NE, // 10 - 17
	F4,NE, E4,NE, F4,NE, D4,NE, E4,NE, D4,NE, E4,NE, C4,NE, // 18 - 25
	D4,NE, C4,NE, D4,NE, 
	
	AS3,NE, D4,NQ, D4,NE, C4,NE, // 26 - 33

	D4,NE, A3,NE, F3,NE, A3,NE, D3,NQ, D4,NE, C4,NE, // 34 - 41
	D4,NE, A3,NE, F3,NE, A3,NE, D3,NQ, D4,NE, E4,NE, // 42 - 49
	F4,NE, E4,NE, F4,NE, D4,NE, E4,NE, D4,NE, E4,NE, C4,NE, // 50 - 57
	D4,NE, C4,NE, D4,NE, 
	
	E4,NE, F4,NQ, // 58 - 63
	
    // page 2

	A4,NE, G4,NE, // 0 - 1
	
	A4,NE, F4,NE, C4,NE, F4,NE, A3,NQ, A4,NE, G4,NE, // 2- 9
	A4,NE, F4,NE, C4,NE, F4,NE, A3,NQ, A4,NE, B4,NE, // 10 - 17
	C5,NE, B4,NE, C5,NE, A4,NE, B4,NE, A4,NE, B4,NE, G4,NE, // 18 - 25
	A4,NE, G4,NE, A4,NE, F4,NE, A4,NQ, 
	
	A4,NE, G4,NE, // 26 - 33

	A4,NE, F4,NE, C4,NE, F4,NE, A3,NQ, A4,NE, G4,NE, // 34 - 41
	A4,NE, F4,NE, C4,NE, F4,NE, A3,NQ, A4,NE, B4,NE, // 42 - 49
	C5,NE, B4,NE, C5,NE, A4,NE, B4,NE, A4,NE, B4,NE, G4,NE, // 50 -57
	A4,NE, G4,NE, A4,NE, F4,NE, A4,NQ, // 51 - 63
	
    // page 3

	D5,NE, C5,NE, // 0 - 1

	A4,NE, F4,NE, C4,NE, F4,NE, A3,NQ, A4,NE, G4,NE, // 2- 9
	A4,NE, F4,NE, C4,NE, F4,NE, A3,NQ, A4,NE, B4,NE, // 10 - 17
	C5,NE, B4,NE, C5,NE, A4,NE, B4,NE, A4,NE, B4,NE, G4,NE, // 18 - 25
	A4,NE, G4,NE, F4,NE, G4,NE, A4,NQ,

		32100 };

// #define		NS	1
// #define		NE	2
// #define		NQ	4
// #define		NH	8
// #define		NW	16

int	music2[] = { 	/* music for death of man */
	REST,2, C4,6, C4,4, C4,2, C4,6, DS4,4, D4,2, D4,4, C4,2, C4,4, B3,2, C4,12,
	REST,16, REST,16, REST,16, REST,16, REST,16, REST,16, REST,16, REST,16, REST,16, REST,16, REST,16, REST,16, 
		32100 };


find_speed()
{
	int	flag;

	while (tick==0) {	/* wait for tick to be zero */
		flag=flag;
	}
	while (!(tick==0)) {	/* wait for tick to be not zero */
		flag=flag;
	}


	time_reg=0;
	timetemp=0;
	while (tick==timetemp) {	/* wait for tick to change */
		time_reg++;
	}

	time_mul=time_reg/550;	/* adjustment for pulse width music */
	if(time_mul<1)
		time_mul=1;

	time_reg-=550;	/* adjust for PC speed */
	if(time_reg<1)
		time_reg=1;


}





time()
{

	if(sound && (!sound_on) ) {	/* if keyboard module flags sound on & it is not on in the program
					   then turn on in program */
		sound_on=TRUE;
		music=0xffff;	/* TRUE must not be used */
	}

	if(!sound && sound_on) {	/* if keyboard module flags sound off & it is on in the program
					   then turn off in program */
		sound_on=FALSE;
		set_tone(40);	/* set tone to a high freq - turning speaker off causes a clic */
		sn2_off();	/* turn off all sound */
	}

	if(sound_on && (!m_pause) ) {
		p_period=32000;	/* default for no sound */  
		t_period=40;
		if(music)	/* only play music when toggle is enabled */
			chk_music();     

		chk_2_coin();	/* second sound for coins */
		chk_loose();	/* sound for loose bags */
		chk_dead();	/* sound for dead man */
		chk_break();	/* sound for breaking bag */
		chk_money();	/* sound for getting money */
		chk_coin();	/* sound for man eating coin */
		chk_explode();	/* sound for missle explotion */
		chk_fire();	/* sound for missile fire */
		chk_bug();	/* sound for bug being eaten during bonus round */
		chk_fall();	/* sound for falling bags */
		chk_life();	/* sound when new life is given */
		chk_chase();	/* sound during bonus round change-over */

		if(p_period==32000 || t_period!=40) 
			sn2_off();	/* turn off second sound source */
		else {
			sn2_on();	/* turn on second sound source */
			sn2_new();	/* update sound */ 
		}
		set_sound(t_period); 

	}

}


tdelay()
{

	/* equilize system clock speed */

	timetemp=0;
	while (timetemp<time_reg) {	
		timetemp++;
	}


}




init_th()	/* initialize sound */
{
	rst_fall();	/* turn of bag falling sound */	
	rst_loose();	/* turn off loose bag sound */
	rst_fire();	/* turn off missile sound */
	rst_music();	/* stop any background music */
	rst_chase();	/* stop sound for bonus round change-over */
	rst_explode();	/* sound for missle explotion */
	rst_break();	/* sound for bag breaking */
	rst_coin();	/* sound for coin being eaten */
	rst_2_coin();	/* second sound for coin being eaten */
	rst_money();	/* sound for money being grabbed */
	rst_bug();	/* sound for bug being eaten during bonus round */
	rst_dead();	/* sound for dead man */
	rst_life();	/* sound when new life is given */

}




rst_end()	/* sound for end of screen  */
{
	end_stat=FALSE;
	m_pause=FALSE;
}



snd_end()	/* sound for end of screen  - not interrupt driven */
{
	int	a;

	init_th();	/* turn off all other sounds */
	m_pause=TRUE;
	end_count=0;
	end_loop=20;
	end_stat=TRUE;
	while(end_stat) {
		if(tick!=a) {
			chk_end();
			a=tick;
		}
	}
}


chk_end()
{
	if(sound_on) {
		switch(end_count) {
		case 0:
			t_period=C5;
			break;
		case 1:
			t_period=E5;
			break;
		case 2:
			t_period=G5;
			break;
		case 3:
			t_period=D5;
			break;
		case 4:
			t_period=F5;
			break;
		case 5:
			t_period=A5;
			break;
		case 6:
			t_period=E5;
			break;
		case 7:
			t_period=G5;
			break;
		case 8:
			t_period=B5;
			break;
		case 9:
		case 10:
			t_period=C5/2;
			break;
		}


		p_period=t_period+35;
		p_width=50;

		sn2_on();
		sn2_new();
		set_sound(t_period);

		if(end_loop>0)
			end_loop--;
		else {
			end_loop=20;
			end_count++;
			if(end_count>10)
				rst_end();
		}
	}
	else {
		sleep(100);
		end_stat=FALSE;
	}
}


init_fall()
{
	fall_period=1000;
	fall_stat=TRUE;

}


rst_fall()
{
	fall_stat=FALSE;
	fall_num=0;
}



chk_fall()
{

	if(fall_stat) {	/* update if sound of fall is active */
		if(fall_num<1) {
			fall_num++;
			if(fall_toggle) {
				t_period=fall_period;
			}
		}
		else {
			fall_num=0;
			if(fall_toggle) {
				fall_period+=50;
				fall_toggle=FALSE;
			}
			else
				fall_toggle=TRUE;
		}

	}

}


snd_break()
{	/* sound for bag breaking */

	break_count=3;
	if(break_period<15000)
		break_period=15000;
	break_stat=TRUE;
}

rst_break()
{
	break_stat=FALSE;

}

chk_break()
{
	if(break_stat) {
		
		if(break_count) {
			break_count--;
			t_period=break_period;
		}
		else
			break_stat=FALSE;
	}
}



chk_loose()
{
	if(loose_stat) {
		loose_type++;
		if(loose_type>63)
			loose_type=0;

		switch(loose_type) {
		case 0:
			t_period=2000;
			break;
		case 16:
			t_period=2500;
			break;
		case 32:
			t_period=3000;
			break;
		case 48:
			t_period=2500;
			break;
		}

	}

}


init_loose()
{
	loose_stat=TRUE;

}

rst_loose()
{

	loose_stat=FALSE;
	loose_type=0;

}



init_fire()
{
	fire_freq=500;
	fire_stat=TRUE;

}

rst_fire()
{
	fire_stat=FALSE;
	fire_pause=0;
}

chk_fire()
{
	if(fire_stat) {
		if(fire_pause==1) {
			fire_pause=0;
			fire_freq+=fire_freq/55;
			t_period=fire_freq+rnd(fire_freq>>3);
			if(fire_freq>30000)
				rst_fire();
		}
		else
			fire_pause++;
	}
}

snd_explode()
{	/* sound for missle explotion */

	exp_period=1500;
	exp_count=10;
	exp_stat=TRUE;
	rst_fire();	/* end normal fire sound */
}

rst_explode()
{
	exp_stat=FALSE;

}

chk_explode()
{
	if(exp_stat) {
		if(exp_count) {
			exp_period-=(exp_period>>3);
			t_period=exp_period;
			exp_count--;
		}
		else
			exp_stat=FALSE;
	}

}




init_chase()
{

	chase_stat=TRUE;

}

rst_chase()
{
	chase_stat=FALSE;
	chase_count=0;
}

chk_chase()
{
	if(chase_stat) {
		chase_count++;
		if(chase_count>15)
			chase_count=0;

		switch(chase_count) {
		case 0:		
		case 1:		
		case 2:		
		case 3:		
		case 4:		
		case 5:		
			t_period=1230;
			break;
		case 8:		
		case 9:		
		case 10:		
		case 11:		
		case 12:		
		case 13:		
			t_period=1513;
			break;
		}

	}
}




rst_coin()
{	/* sound for man eating coins */
	coin_stat=FALSE;
}

snd_coin()
{
	coin_stat=TRUE;
}

chk_coin()
{
	if(coin_stat) {
		t_period=1000;
		rst_coin();
	}

}




rst_2_coin()
{	/* second sound for man eating coins */
	coin_2_stat=FALSE;
}

snd_2_coin(new_flag)
int	new_flag;
{
	if(new_flag) {
		switch(coin_2_period) {
		case C5:
			coin_2_period=D5;
			break;
		case D5:
			coin_2_period=E5;
			break;
		case E5:
			coin_2_period=F5;
			break;
		case F5:
			coin_2_period=G5;
			break;
		case G5:
			coin_2_period=A5;
			break;
		case A5:
			coin_2_period=B5;
			break;
		case B5:
			coin_2_period=C6;
			score_2_coin();	/* extra bonus for getting 8 sucessive coins */
			break;
		case C6:
			coin_2_period=C5;
			break;
		}
	}
	else
		coin_2_period=C5;

	coin_2_count=7;
	coin_2_type=0;
	coin_2_stat=TRUE;

}

chk_2_coin()
{
	if(coin_2_stat) {
		if(coin_2_count) {
			switch(coin_2_type) {
			case 0:
			case 1:
				t_period=coin_2_period;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				break;
			}

			coin_2_type++;
			if(coin_2_type>7) {
				coin_2_type=0;
				coin_2_count--;
			}

		}
		else
			rst_2_coin();	

	}

}



rst_money()
{	/* sound for money being grabbed */

	money_stat=FALSE;

}


snd_money()
{

	money_1=500;
	money_2=4000;
	money_count=30;
	money_toggle=FALSE;
	money_stat=TRUE;

}

chk_money()
{
	if(money_stat) {
		if(money_count)
			money_count--;
		else
			money_stat=FALSE;

		if(money_toggle) {
			money_toggle=FALSE;
			t_period=money_1;
		}
		else {
			money_toggle=TRUE;
			t_period=money_2;
		}

		money_1+=(money_1>>4);
		money_2-=(money_2>>4);
	}
}



rst_bug()
{	/* sound for bug being eaten during bonus round */
	
	bug_stat=FALSE;

}

snd_bug()
{

	bug_c_1=20;
	bug_c_2=3;
	bug_period=2000;
	bug_stat=TRUE;

}

chk_bug()
{

	if(bug_stat) {

		if(bug_c_2) {	/* loop for 3 sets of tones */

			if(bug_c_1) {
				if((bug_c_1 %4)==1)
					t_period=bug_period;
				if((bug_c_1 %4)==3)
					t_period=bug_period-(bug_period>>4); 
				bug_c_1--;
				bug_period-=(bug_period>>4);
			}
			else {
				bug_c_1=20;
				bug_c_2--;
				bug_period=2000;
			}
		}
		else
			bug_stat=FALSE;
	}
}



rst_dead()	/* sound for dead man */
{

	dead_stat=FALSE;

}

snd_dead()
{

	dead_count=0;
	dead_period=20000;
	dead_stat=TRUE;

}

chk_dead()
{
	if(dead_stat) {
		dead_count++;

		switch(dead_count) {
		case 1:
			rst_music();
			dead_period=19000;
			break;
		case 2:
			dead_period=18000;
			break;
		case 3:
			dead_period=17000;
			break;
		case 4:
			dead_period=16000;
			break;
		case 5:
			dead_period=15000;
			break;
		case 6:
			dead_period=14000;
			break;
		case 7:
			dead_period=13000;
			break;
		case 8:
			dead_period=12000;
			break;
		case 9:
			dead_period=11000;
			break;
		case 10:
			dead_period=10000;
			break;
		}

		if(dead_count>10)
			dead_period+=500;


		if(dead_period>30000)
			rst_dead();
	
       		t_period=dead_period;

	}
}


rst_life()	/* sound when new life is given */
{
	life_stat=FALSE;
}

snd_life()
{

	life_count=96;
	life_stat=TRUE;

}

chk_life()
{
	if(life_stat) {

		if((life_count/3)%2)
			t_period=600+(life_count<<2);
		life_count--;
		if(life_count<1)
			life_stat=FALSE;
	}

}




rst_music()
{
	m_stat=FALSE;
	m_pointer=0;
	
}


init_music(x)
int	x;
{
	m_type=x;
	m_count=0;
	m_pointer=0;

	switch(m_type) {
	case 0:	/* chase music */
		attack_level=50;
		attack_rate=20;
		hold_level=20;
		hold_rate=10;
		decay_rate=4;
		break;
	case 1:	/* normal man movement music */
		attack_level=50;
		attack_rate=50;
		hold_level=8;
		hold_rate=15;
		decay_rate=1;
		break;
	case 2:	/* man death music */
		attack_level=50;
		attack_rate=50;
		hold_level=25;
		hold_rate=5;
		decay_rate=1;
		break;
	}

	m_stat=TRUE;

	if(x==2)	/* death march */
		rst_dead();	/* turn off dead sound */

}


chk_music()
{
	if(m_stat) {	/* execute if music is active */

		if(m_count) {
			m_count--;
		}
		else {

			envelope=ATTACK;	/* set pointer for start of note envelope */
			env_position=0;

			switch(m_type) {
			case 0:	/* chase music */
				m_count=music0[m_pointer+1]*3;
				m_note=music0[m_pointer];
				hold_count=m_count-3;
				m_pointer+=2;
				if(music0[m_pointer]==32100)
					m_pointer=0;
				break;
			case 1:	/* normal man movement music */
				m_count=music1[m_pointer+1]*6;
				m_note=music1[m_pointer];
				hold_count=12;
				m_pointer+=2;
				if(music1[m_pointer]==32100)
					m_pointer=0;
				break;
			case 2:	/* man death music */
				m_count=music2[m_pointer+1]*10;
				m_note=music2[m_pointer];
				hold_count=m_count-10;
				m_pointer+=2;
				if(music2[m_pointer]==32100)
					m_pointer=0;
				break;
			}
		}

		env_position++;	/* position within envelope */
		sn2_mode=PULSE;	/* initialize to pulse mode */

		p_period=m_note;	/* set up tone value */

		if(env_position>=hold_count)
			envelope=DECAY;

		switch(envelope) {
		case ATTACK:
			if( (p_width+attack_rate) >= attack_level) {
				p_width=attack_level;
				envelope=HOLD;	/* move to next part of envelope */
			}
			else
				p_width+=attack_rate;
			break;
		case HOLD:
			if( (p_width-hold_rate)<=hold_level) 
				p_width=hold_level;
			else
				p_width-=hold_rate;
			break;
		case DECAY:
			if( (p_width-decay_rate)<=1)
				p_width=1;
			else
				p_width-=decay_rate;
			break;
		}

		if(p_width==1)
			p_period=32000;

	}

}






init_pause()
{	/* set up a pause to stop background music during keyboard hold */

	m_pause=TRUE;

}



rst_pause()
{
	m_pause=FALSE;

}


rst_sound()
{	/* turn off speaker port at end of game - used during developement */
	int	temp;

	temp=inp(PORT_B); 
	temp =temp & 0xfc & ~SPEAKER_ENABLE & ~TIMER_ENABLE;
	outp(PORT_B,temp);	/* turn off timer */

	outp(INTERUPT_PORT,0xff);
	outp(INTERUPT_PORT,0xff);

}


init_sound()
{	/* turn on speaker & start up the external 2 tone sound routine */
	int	temp;

	outp(TIMER_MODE,0xb6);	/* set timer mode */
	outp(TIMER_PORT,0x20);	/* initialize timer to a high freq. */
	outp(TIMER_PORT,0x0);

	temp=inp(PORT_B); 
	temp =temp & 0xfc | SPEAKER_ENABLE | TIMER_ENABLE;
	outp(PORT_B,temp);	/* turn on speaker */


	outp(TIMER_MODE,0x36);	/* speed up timer */
	outp(INTERUPT_PORT,0);
	outp(INTERUPT_PORT,0x80);

	sound_on=TRUE;	/* set sound on flag */
	sound2_on=FALSE;	/* Reset sound2 flag */
	sn2on=0;
	sn2_mode=SQUARE;	/* initialize to square wave mode */
	p_period=12000;
	p_width=8;
	t_period=40;	
	sn2_stat=ON;	/* needed to insure that sn2_off will turn off sound */
	sn2_off();	/* turn off all sound to begin */
	
	init_th();
	sn2_start();	/* initialize interupt sound */
	sound2=0xffff;
	outp(INTERUPT_PORT,0x4000 & 0xff);
	outp(INTERUPT_PORT,0x4000 >> 8);	

}


/* ***************************************************************************
   	interupt pulse sound routines
   ***************************************************************************
*/


sn2_new()	/* set new frequency for boath tone generators */
{
	int	temp;


	if(sound_on) {	/* only update if the sound is turned on */

		set_tone(t_period); 	/* set first tone generator */
		if(p_period<1000 && sn2_mode==SQUARE)	/* check maximum freq of interupts */
			p_period=1000;
		if(p_period<1000 && sn2_mode==PULSE)
			p_period=1000;

		outp(INTERUPT_PORT,p_period & 0xff);
		outp(INTERUPT_PORT,p_period >> 8);	
		if(p_period<16000)
			sound2=(p_period<<2);	/* set new tick counter value */
		else
			sound2=0xffff;

		if(p_width<1)
			p_width=1;
		if(p_width>50)
			p_width=50;
		per2=p_width*time_mul;
		sn2_on();	/* insure that sound is on */ 
	}

}


sn2_off()	/* turn off second sound source */
{
	int	temp;

	if(sn2_stat==ON) {	/* sound 2 is now on */
		sn2_stat=OFF;
		sn2on=0;
		temp=inp(PORT_B); 
		temp =temp & 0xfc | SPEAKER_ENABLE | TIMER_ENABLE;
		outp(PORT_B,temp);	/* turn on speaker */
	}
}

sn2_on()	/* turn on second sound source */
{
	int	temp;

/* for pulse wave - sn2on = 1
   for square wave - sn2on = 2 */

	sn2on=sn2_mode;	/* turn sound on with desired mode */	

	if(sn2_stat==OFF && sound_on) {	/* sound 2 is now off */
		sn2_stat=ON; 
		temp=inp(PORT_B); 
		temp =temp & 0xfc | SPEAKER_ENABLE | TIMER_ENABLE;
		outp(PORT_B,temp);	/* turn on speaker */
	}

}

sn2_start()	/* start up interupt sound source if it is not already on */
{
	if(!sound2_on) {
		sn2_init();
		sound2=0xffff;
		outp(INTERUPT_PORT,0x4000 & 0xff);
		outp(INTERUPT_PORT,0x4000 >> 8);	
		sound2_on=TRUE;
	}

}


sn2_stop()	/* stop interupt sound source if it is already on & stop sound source 1 */  
{
	int	temp;

	if(sound2_on) {
		sn2_rst();
		sound2_on=FALSE;
	}

	set_sound(40);	/* set to a high tone for no sound */

	temp=inp(PORT_B); 
	temp =temp & 0xfc | SPEAKER_ENABLE | TIMER_ENABLE;
	outp(PORT_B,temp);	/* turn on speaker */


	outp(TIMER_MODE,0x36);	/* speed up timer */
	outp(INTERUPT_PORT,0);
	outp(INTERUPT_PORT,0x80);

}



set_sound(period)	
{	/* if sound is on then set the timer to a value - used by high score sound routine ext. */

	if(sound_on) {
		outp(TIMER_PORT,period & 0xff);
		outp(TIMER_PORT,period >> 8);	
	}

}


set_tone(freq)
{	/* always sets the sound port - used within this task */
	outp(TIMER_PORT,freq & 0xff);
	outp(TIMER_PORT,freq >> 8);	

}
