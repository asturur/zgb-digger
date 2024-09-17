/*
*****************************************************************************************************
*****************************************************************************************************
*****                                                                                           *****
*****                  Task G   -   Sreen Background Maintenence                                *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****************************************************************************************************
*****************************************************************************************************
*/

#include	"digger.h"

int	back_0[H_MAX][W_MAX];
int	back_1[H_MAX][W_MAX];

int	background[H_MAX][W_MAX]; /* used to store info for present screen background 
				     the integer is split up to indicate the following:
				    	Bit number  Value   Meaning
				  		0     	0     	The first horizontal bite from the right has been removed
				  		1	0	The second     	"		"		"	
				  		2	0	The third	"		"		"	
                	                        3	0	The fourth	"		"		"
				  		4	0	The last	"		"		"

				  		6	0	The first vertical bite from the top has been removed
				  		7	0	The second	"		"		"
				  		8	0	The third	"		"		"
				  		9	0	The fourth	"		"		"
				  		10	0	The fifth	"		"		"
				  		11	0	The last	"		"		"

				  		13	0	The entire background square is removed
				  */		
								

init_tg()	/* create background and paths at corrct positions for the level */
{	
        int	temp, x_pos, y_pos;


	for(x_pos=0; x_pos<W_MAX; x_pos++) {
		for(y_pos=0; y_pos<H_MAX; y_pos++) {
			background[y_pos][x_pos]=0xffff;	/* show an un-eaten square */

			temp=get_mat(x_pos,y_pos,get_screen()); 	/* get type of square */
			if(temp=='S' || temp=='V') 	/* erase a vertical path */
					background[y_pos][x_pos]&=0xd03f;	/* set up matrix for this player */
			if(temp=='S' || temp=='H') 	/* erase a horizontal path */
					background[y_pos][x_pos]&=0xdfe0;	/* set up matrix for this player */

			if(get_player()==0)
				back_0[y_pos][x_pos]=background[y_pos][x_pos];
			else
				back_1[y_pos][x_pos]=background[y_pos][x_pos];

		}
	}

}



start_tg()
{
	int	x, y;

	for(x=0; x<W_MAX; x++) {
		for(y=0; y<H_MAX; y++) {
			if(get_player()==0)
				background[y][x]=back_0[y][x];
			else
				background[y][x]=back_1[y][x];

		}
	}
	palette(0);
	draw_background(get_screen());	/* create background pattern */
	era_path();

}


end_tg()
{
	int	x, y;

	for(x=0; x<W_MAX; x++) {
		for(y=0; y<H_MAX; y++) {
			if(get_player()==0)
				back_0[y][x]=background[y][x];
			else
				back_1[y][x]=background[y][x];

		}
	}

}



era_path()
{
        int	x_pos, y_pos, x, y;


	for(x_pos=0; x_pos<W_MAX; x_pos++) {
		for(y_pos=0; y_pos<H_MAX; y_pos++) {
			if((background[y_pos][x_pos] & 0x2000)==0) {	/* path is in this block */
				x=x_pos*20+X_OFFSET;
				y=y_pos*18+Y_OFFSET;
				if((background[y_pos][x_pos] & 0xfc0)!=0xfc0) {	/* erase a vertical path */
					background[y_pos][x_pos]&=0xd03f;	/* set up matrix for this player */
					era_down(x,y-15);	/* erase screen */
					era_down(x,y-12);
					era_down(x,y-9);
					era_down(x,y-6);
					era_down(x,y-3);
					era_up(x,y+3);

				}
				if((background[y_pos][x_pos] & 0x1f)!=0x1f) {
					background[y_pos][x_pos]&=0xdfe0;	/* set up matrix for this player */
					era_right(x-16,y);	/* erase screen */
					era_right(x-12,y);
					era_right(x-8,y);
					era_right(x-4,y);
					era_left(x+4,y);					                                	
				}					
				if(x_pos<(W_MAX-1)) {
					if((background[y_pos][x_pos+1] & 0xfdf)!=0xfdf)
						era_right(x,y); 
				}
				if(y_pos<(H_MAX-1)) {
					if((background[y_pos+1][x_pos] & 0xfdf)!=0xfdf)	/* if any are erased */
						era_down(x,y); 
				}

			}
		}
	}


}



era_background(x,y,dir)
int	x, y, dir;
{
	int	x_pos, x_rem, y_pos, y_rem;

	x_pos=(x-X_OFFSET)/20; x_rem=((x-X_OFFSET)%20)/4;
	y_pos=(y-Y_OFFSET)/18; y_rem=((y-Y_OFFSET)%18)/3;

	inc_plot();	/* report screen plotting */
	switch(dir) {	/* correct coordinates for next bite in direction of travel */
	case RIGHT:
		x_pos++;	/* correct to next matrix position to right */
		switch(x_rem) {
		case 0:
			background[y_pos][x_pos]&=0xfffe;	/* set flag for bite removed from background */
			break;
		case 1:
			background[y_pos][x_pos]&=0xfffd;
			break;
		case 2:
			background[y_pos][x_pos]&=0xfffb;
			break;
		case 3:
			background[y_pos][x_pos]&=0xfff7;
			break;
		case 4:
			background[y_pos][x_pos]&=0xffef;
			break;
		}
		if((background[y_pos][x_pos]&0x1f)==0)
			background[y_pos][x_pos]&=0xdfff;	/* set flag for entire square eaten */
		break;
	case LEFT:
		x_rem--;
		if(x_rem<0) {	/* correct for next position to the left */
			x_rem+=5;
			x_pos--;
		}
		switch(x_rem) {
		case 0:
			background[y_pos][x_pos]&=0xfffe;	/* set flag for bite removed from background */
			break;
		case 1:
			background[y_pos][x_pos]&=0xfffd;
			break;
		case 2:
			background[y_pos][x_pos]&=0xfffb;
			break;
		case 3:
			background[y_pos][x_pos]&=0xfff7;
			break;
		case 4:
			background[y_pos][x_pos]&=0xffef;
			break;
		}
		if((background[y_pos][x_pos]&0x1f)==0)
			background[y_pos][x_pos]&=0xdfff;	/* set flag for entire square eaten */
		break;
	case UP:
		y_rem--;
		if(y_rem<0) {	/* correct for next position up */
			y_rem+=6;
			y_pos--;
		}
		switch(y_rem) {
		case 0:
			background[y_pos][x_pos]&=0xffbf;	/* set flag for bite removed from background */
			break;
		case 1:
			background[y_pos][x_pos]&=0xff7f;
			break;
		case 2:
			background[y_pos][x_pos]&=0xfeff;
			break;
		case 3:
			background[y_pos][x_pos]&=0xfdff;
			break;
		case 4:
			background[y_pos][x_pos]&=0xfbff;
			break;
		case 5:
			background[y_pos][x_pos]&=0xf7ff;
			break;
		}
		if((background[y_pos][x_pos]&0xfc0)==0)
			background[y_pos][x_pos]&=0xdfff;	/* set flag for entire square eaten */
		break;
	case DOWN:
		y_pos++;	/* correct to next Y matrix location */
		switch(y_rem) {
		case 0:
			background[y_pos][x_pos]&=0xffbf;	/* set flag for bite removed from background */
			break;
		case 1:
			background[y_pos][x_pos]&=0xff7f;
			break;
		case 2:
			background[y_pos][x_pos]&=0xfeff;
			break;
		case 3:
			background[y_pos][x_pos]&=0xfdff;
			break;
		case 4:
			background[y_pos][x_pos]&=0xfbff;
			break;
		case 5:
			background[y_pos][x_pos]&=0xf7ff;
			break;
		}
		if((background[y_pos][x_pos]&0xfc0)==0)
			background[y_pos][x_pos]&=0xdfff;	/* set flag for entire square eaten */
		break;
	}
/*
	for(y_pos=0; y_pos<H_MAX; y_pos++) {
		for(x_pos=0; x_pos<W_MAX; x_pos++) {
			cprintf("%04x ",background[y_pos][x_pos]);
		}
		cprintf("\n\r");
	}
	cprintf("\n\r");
*/
}



get_background(x,y)
int	x, y;
{	/* return background square information */
	return(background[y][x]);

}

