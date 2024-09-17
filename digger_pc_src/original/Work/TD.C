/*
*****************************************************************************************************
*****************************************************************************************************
*****                                                                                           *****
*****                  Task D   -   Eat Objects routines                                        *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****                                                                                           *****
*****************************************************************************************************
*****************************************************************************************************
*/

#include	"digger.h"


char	coins[H_MAX][W_MAX];	/* used during each round of play 
			   if a coin is at the location, data & player==TRUE 
			   player=1 for player#1, 2 for player #2 */


int	p_mask;


init_td()	/* clear all locations that may contain a coin then insert coins at corrct positions for the level */
{	

        int	loopx, loopy;

	if(get_player()==0)
		p_mask=1;	/* set mask for player 0 */
	else
		p_mask=2;	/* set mask for player 1 */


	for(loopx=0; loopx<W_MAX; loopx++) {
		for(loopy=0; loopy<H_MAX; loopy++) {
			if(get_mat(loopx,loopy,get_screen())=='C')
				coins[loopy][loopx] |=p_mask;
			else
				coins[loopy][loopx] &=~p_mask;
		}
	}

}



start_td()
{	

        int	loopx, loopy;

	if(get_player()==0)
		p_mask=1;	/* set mask for player 0 */
	else
		p_mask=2;	/* set mask for player 1 */

	for(loopx=0; loopx<W_MAX; loopx++) {
		for(loopy=0; loopy<H_MAX; loopy++) {
			if(coins[loopy][loopx] & p_mask)
				put_coin(loopx*20+X_OFFSET,loopy*18+3+Y_OFFSET);
		}
	}

}



coin_check(x_pos,y_pos,x_rem,y_rem,dir)
int	x_pos, y_pos, x_rem, y_rem, dir;
{	/* check for coins near area that man is digging */

	int	stat;

	stat=FALSE;	/* set to TRUE if a coin is removed - used for scoring */

	switch(dir) {
	case RIGHT:
		if(x_rem)
			x_pos++;	/* round up to point to real location */

		if(coins[y_pos][x_pos] & p_mask) {	/* update screen if a coin is present */
			if(x_rem==8) {	/* redraw coin to cover bite taken by man */
				put_coin(x_pos*20+X_OFFSET,y_pos*18+3+Y_OFFSET);
				inc_plot();	/* report screen plot */
			}
			if(x_rem==12) {	/* erase coin */
				rem_coin(x_pos*20+X_OFFSET,y_pos*18+3+Y_OFFSET);
				inc_plot();	/* report screen plot */
				stat=TRUE;
				coins[y_pos][x_pos]&=~p_mask;	/* remove coin from array */
			}
		}
		break;
	case LEFT:
		if(coins[y_pos][x_pos] & p_mask) {	/* update screen if a coin is present */
			if(x_rem==16) {	/* redraw coin to cover bite taken by man */
				put_coin(x_pos*20+X_OFFSET,y_pos*18+3+Y_OFFSET);
				inc_plot();	/* report screen plot */
			}
			if(x_rem==12) {	/* erase coin */
				rem_coin(x_pos*20+X_OFFSET,y_pos*18+3+Y_OFFSET);
				inc_plot();	/* report screen plot */
				stat=TRUE;
				coins[y_pos][x_pos]&=~p_mask;	/* remove coin from array */
			}
		}
		break;
	case UP:
		if(coins[y_pos][x_pos] & p_mask) {	/* update screen if a coin is present */
			if(y_rem==12) {	/* redraw coin to cover bite taken by man */
				put_coin(x_pos*20+X_OFFSET,y_pos*18+3+Y_OFFSET);
				inc_plot();	/* report screen plot */
			}
			if(y_rem==9) {	/* erase coin */
				rem_coin(x_pos*20+X_OFFSET,y_pos*18+3+Y_OFFSET);
				inc_plot();	/* report screen plot */
				stat=TRUE;
				coins[y_pos][x_pos]&=~p_mask;	/* remove coin from array */
			}
		}
		break;
	case DOWN:
		if(y_rem)
			y_pos++;	/* round up to point to real location */

		if(coins[y_pos][x_pos] & p_mask) {	/* update screen if a coin is present */
			if(y_rem==6) {	/* redraw coin to cover bite taken by man */
				put_coin(x_pos*20+X_OFFSET,y_pos*18+3+Y_OFFSET);
				inc_plot();	/* report screen plot */
			}
			if(y_rem==9) {	/* erase coin */
				rem_coin(x_pos*20+X_OFFSET,y_pos*18+3+Y_OFFSET);
				inc_plot();	/* report screen plot */
				stat=TRUE;
				coins[y_pos][x_pos]&=~p_mask;	/* remove coin from array */
			}
		}
		break;
        }
	return(stat);
}



count_coins()
{
	int	x, y, temp;

	temp=0;		/* reset coin counter */
	for(x=0; x<W_MAX; x++) {
		for(y=0; y<H_MAX; y++) {
			if(coins[y][x] & p_mask)
				temp++;
		}
	}

	return(temp);	/* return number of active coins */

}



	

dest_coin(x_pos,y_pos)	/* d=remove coin from matrix - used if a falling money bag destroys a coin */
int	x_pos, y_pos;
{

	if(coins[y_pos+1][x_pos] & p_mask) {
		coins[y_pos+1][x_pos]&=~p_mask;
        	rem_coin(x_pos*20+X_OFFSET,(y_pos+1)*18+3+Y_OFFSET);
	}
}

