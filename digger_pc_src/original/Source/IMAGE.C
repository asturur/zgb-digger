
#include "digger.h"

int	bug_count[6], bug_inc[6];


/* sprites used for coins
	sprite_cn = coin image
	sprite_nc = coin erase block
*/

#include <spritecn.spr>
#include <spritenc.spr>



/* sprites used to show lives left 
   sprite s0 = man for player 0
   sprite s1 = man for player 1
   sprite s2 = blank
*/

#include <sprites0.spr>
#include <sprites1.spr>
#include <sprites2.spr>




/* sprites ( 1 - 7 ) used for stationary money bags 
   sprite b4 is a left tilted money bag
   sprite b5 is a right tilted money bag

   sprite b6 is a falling money bag

   sprite b7 is the first broken money bag
   sprite b8 is the second broken money bag
   sprite b9 is the third broken money bag
*/
   
#include <sprite1.spr>
#include <spriteb4.spr>
#include <spriteb5.spr>
#include <spriteb6.spr>
#include <spriteb7.spr>
#include <spriteb8.spr>
#include <spriteb9.spr>




char	bkgnd_2[X_WID_1*Y_WID_1];
char	bkgnd_3[X_WID_1*Y_WID_1];
char	bkgnd_4[X_WID_1*Y_WID_1];
char	bkgnd_5[X_WID_1*Y_WID_1];
char	bkgnd_6[X_WID_1*Y_WID_1];
char	bkgnd_7[X_WID_1*Y_WID_1];



/* sprites used for enemy creatures

   sprite m0 = first creature normal type
   sprite m1 = second creature normal type
   sprite m2 = third creature normal type
   sprite m3 = falling dead bug normal type

   sprite c0 = first creature chasing you in right direction
   sprite c1 = second creature chasing you in right direction
   sprite c2 = third creature chasing you in right direction
   sprite c3 = falling dead creature chasing you in right direction

   sprite a0 = first creature chasing you in left direction
   sprite a1 = second creature chasing you in left direction
   sprite a2 = third creature chasing you in left direction
   sprite a3 = falling dead creature chasing you in left direction
*/

#include <spritem0.spr>
#include <spritem1.spr>
#include <spritem2.spr>
#include <spritem3.spr>
#include <spritec0.spr>
#include <spritec1.spr>
#include <spritec2.spr>
#include <spritec3.spr>
#include <spritea0.spr>
#include <spritea1.spr>
#include <spritea2.spr>
#include <spritea3.spr>




char	bkgnd_8[X_WID_c0*Y_WID_c0];
char	bkgnd_9[X_WID_c0*Y_WID_c0];
char	bkgnd_10[X_WID_c0*Y_WID_c0];
char	bkgnd_11[X_WID_c0*Y_WID_c0];
char	bkgnd_12[X_WID_c0*Y_WID_c0];
char	bkgnd_13[X_WID_c0*Y_WID_c0];


init_images()
{
	int	loop;

	sp_init(1,image_1,outln_1,bkgnd_1,X_WID_1,Y_WID_1,0,0);
	sp_init(2,image_1,outln_1,bkgnd_2,X_WID_1,Y_WID_1,0,0);
	sp_init(3,image_1,outln_1,bkgnd_3,X_WID_1,Y_WID_1,0,0);
	sp_init(4,image_1,outln_1,bkgnd_4,X_WID_1,Y_WID_1,0,0);
	sp_init(5,image_1,outln_1,bkgnd_5,X_WID_1,Y_WID_1,0,0);
	sp_init(6,image_1,outln_1,bkgnd_6,X_WID_1,Y_WID_1,0,0);
	sp_init(7,image_1,outln_1,bkgnd_7,X_WID_1,Y_WID_1,0,0);

	sp_init(8,image_m0,outln_m0,bkgnd_8,X_WID_m0,Y_WID_m0,0,0);
	sp_init(9,image_m0,outln_m0,bkgnd_9,X_WID_m0,Y_WID_m0,0,0);
	sp_init(10,image_m0,outln_m0,bkgnd_10,X_WID_m0,Y_WID_m0,0,0);
	sp_init(11,image_m0,outln_m0,bkgnd_11,X_WID_m0,Y_WID_m0,0,0);
	sp_init(12,image_m0,outln_m0,bkgnd_12,X_WID_m0,Y_WID_m0,0,0);
	sp_init(13,image_m0,outln_m0,bkgnd_13,X_WID_m0,Y_WID_m0,0,0);

	init_b_images();

	for(loop=0; loop<6; loop++) {
		bug_count[loop]=0; bug_inc[loop]=1;
	}

}


rst_images()
{
	sp_chnge(1,image_1,outln_1,X_WID_1,Y_WID_1,0,0);
	sp_chnge(2,image_1,outln_1,X_WID_1,Y_WID_1,0,0);
	sp_chnge(3,image_1,outln_1,X_WID_1,Y_WID_1,0,0);
	sp_chnge(4,image_1,outln_1,X_WID_1,Y_WID_1,0,0);
	sp_chnge(5,image_1,outln_1,X_WID_1,Y_WID_1,0,0);
	sp_chnge(6,image_1,outln_1,X_WID_1,Y_WID_1,0,0);
	sp_chnge(7,image_1,outln_1,X_WID_1,Y_WID_1,0,0);


	sp_chnge(8,image_m0,outln_m0,X_WID_m0,Y_WID_m0,0,0);
	sp_chnge(9,image_m0,outln_m0,X_WID_m0,Y_WID_m0,0,0);
	sp_chnge(10,image_m0,outln_m0,X_WID_m0,Y_WID_m0,0,0);
	sp_chnge(11,image_m0,outln_m0,X_WID_m0,Y_WID_m0,0,0);
	sp_chnge(12,image_m0,outln_m0,X_WID_m0,Y_WID_m0,0,0);
	sp_chnge(13,image_m0,outln_m0,X_WID_m0,Y_WID_m0,0,0);

	rst_b_images();


}


draw_bug(bug_num,bug_type,bug_dir,x,y)
int	bug_num, bug_type, bug_dir, x, y;
{
	int	stat;

	bug_count[bug_num]+=bug_inc[bug_num];

	if(bug_count[bug_num]==2 || bug_count[bug_num]==0)
		bug_inc[bug_num]=-bug_inc[bug_num];

	if(bug_count[bug_num]>2)	/* saftey feature - should never be needed */
		bug_count[bug_num]=2;
	if(bug_count[bug_num]<0)
		bug_count[bug_num]=0;

	switch(bug_type) {
	case ADVANCING:
        	switch(bug_dir) {
		case RIGHT:
			switch(bug_count[bug_num]) {
			case 0:
				sp_chnge(bug_num+8,image_c0,outln_c0,X_WID_c0,Y_WID_c0,0,0);
				break;
			case 1:
				sp_chnge(bug_num+8,image_c1,outln_c1,X_WID_c1,Y_WID_c1,0,0);
				break;
			case 2:
				sp_chnge(bug_num+8,image_c2,outln_c2,X_WID_c2,Y_WID_c2,0,0);
				break;
			}
        		break;
		case LEFT:
			switch(bug_count[bug_num]) {
			case 0:
				sp_chnge(bug_num+8,image_a0,outln_a0,X_WID_a0,Y_WID_a0,0,0);
				break;
			case 1:
				sp_chnge(bug_num+8,image_a1,outln_a1,X_WID_a1,Y_WID_a1,0,0);
				break;
			case 2:
				sp_chnge(bug_num+8,image_a2,outln_a2,X_WID_a2,Y_WID_a2,0,0);
				break;
			}
        		break;
		}
	
		stat=sp_move(bug_num+8,x,y);	/* draw the new image & get colision info */

		break;


	case NORMAL:
		switch(bug_count[bug_num]) {
		case 0:
			sp_chnge(bug_num+8,image_m0,outln_m0,X_WID_m0,Y_WID_m0,0,0);
			break;
		case 1:
			sp_chnge(bug_num+8,image_m1,outln_m1,X_WID_m1,Y_WID_m1,0,0);
			break;
		case 2:
			sp_chnge(bug_num+8,image_m2,outln_m2,X_WID_m2,Y_WID_m2,0,0);
			break;
		}


		stat=sp_move(bug_num+8,x,y);	/* draw the new image & get colision info */

		break;

#ifdef	DEBUG
	default:
		cprintf("error in switch - draw bug \n\r");
		break;
#endif
	}

	return(stat);	/* return colision info */


}

draw_dead_bug(bug_num,bug_type,bug_dir,x,y)
int	bug_num, bug_type, bug_dir, x, y;
{
	int	stat;

	switch(bug_type) {
	case ADVANCING:
        	switch(bug_dir) {
		case RIGHT:
			sp_chnge(bug_num+8,image_c3,outln_c3,X_WID_c3,Y_WID_c3,0,0);
        		break;
		case LEFT:
			sp_chnge(bug_num+8,image_a3,outln_a3,X_WID_a3,Y_WID_a3,0,0);
        		break;
		}
	
		stat=sp_move(bug_num+8,x,y);	/* draw the new image & get colision info */

		break;


	case NORMAL:
		sp_chnge(bug_num+8,image_m3,outln_m3,X_WID_m3,Y_WID_m3,0,0);

		stat=sp_move(bug_num+8,x,y);	/* draw the new image & get colision info */

		break;

#ifdef	DEBUG
	default:
		cprintf("error in switch - draw dead bug \n\r");
		break;
#endif
	}

	return(stat);	/* return colision info */


}



draw_bag(bag_num,bag_type,x,y)
int	bag_num, bag_type, x, y;
{
	int	colision;

	switch(bag_type) {
	case BAG_0:
		sp_chnge(bag_num,image_1,outln_1,X_WID_1,Y_WID_1,0,0);
		colision=sp_move(bag_num,x,y);
		break;
	case BAG_4:
		sp_chnge(bag_num,image_b4,outln_b4,X_WID_b4,Y_WID_b4,0,0);
		colision=sp_move(bag_num,x,y);
		break;
	case BAG_5:
		sp_chnge(bag_num,image_b5,outln_b5,X_WID_b5,Y_WID_b5,0,0);
		colision=sp_move(bag_num,x,y);
		break;
	case BAG_6:
		sp_chnge(bag_num,image_b6,outln_b6,X_WID_b6,Y_WID_b6,0,0);
		colision=sp_move(bag_num,x,y);
		break;
	case BAG_7:
		sp_chnge(bag_num,image_b7,outln_b7,X_WID_b7,Y_WID_b7,0,0);
		colision=sp_move(bag_num,x,y+(Y_WID_1-Y_WID_b7));	/* offset is added for size difference */
		break;
	case BAG_8:
		sp_chnge(bag_num,image_b8,outln_b8,X_WID_b8,Y_WID_b8,0,0);
		colision=sp_move(bag_num,x,y+(Y_WID_1-Y_WID_b8));	/* offset is added for size difference */
		break;
	case BAG_9:
		sp_chnge(bag_num,image_b9,outln_b9,X_WID_b9,Y_WID_b9,0,0);
		colision=sp_move(bag_num,x,y+(Y_WID_1-Y_WID_b9));	/* offset is added for size difference */
		break;
	}

	return(colision);

}




put_man(stat,x,y)
int	stat, x, y;
{	/* used to display lives left - 0 prints a man for player 0, 1 man for player 1, 2 = blank */

	switch(stat) { 
	case 0:
		sp_put(x,y,image_s0,outln_s0,X_WID_s0,Y_WID_s0);
		break;
	case 1:
		sp_put(x,y,image_s1,outln_s1,X_WID_s1,Y_WID_s1);
		break;
	case 2:
		sp_put(x,y,image_s2,outln_s2,X_WID_s2,Y_WID_s2);
		break;
	}

}




put_coin(x,y)
int	x,y;
{

	sp_blank(x,y,X_WID_cn,Y_WID_cn);
	sp_put(x,y,image_cn,outln_cn,X_WID_cn,Y_WID_cn);
	sp_disp();
}



rem_coin(x,y)
int	x,y;
{

	sp_blank(x,y,X_WID_nc,Y_WID_nc);
	sp_put(x,y,image_nc,outln_nc,X_WID_nc,Y_WID_nc);
	sp_disp();
}


