
#include "digger.h"

int	man_count, man_inc;
int	mis_num;	/* counter for missle types */


/* sprites used for background creation and removal 

   sprite rt = blanking block used during right movement of man 
   sprite lt = blanking block used during left movement of man 
   sprite up = blanking block used during up movement of man 
   sprite dn = blanking block used during down movement of man 

   sprite bf = first blanking block used for falling money bags
   sprite bd = normal blanking block used for falling money bags
   
   sprite p1 = background pattern for level 1
   sprite p2 = background pattern for level 2
   sprite p3 = background pattern for level 3
   sprite p4 = background pattern for level 4
   sprite p5 = background pattern for level 5
   sprite p6 = background pattern for level 6
   sprite p7 = background pattern for level 7
   sprite p8 = background pattern for level 8

*/

#include <spritert.spr>
#include <spritelt.spr>
#include <spriteup.spr>
#include <spritedn.spr>
#include <spritebd.spr>
#include <spritebf.spr>
#include <spritep1.spr>
#include <spritep2.spr>
#include <spritep3.spr>
#include <spritep4.spr>
#include <spritep5.spr>
#include <spritep6.spr>
#include <spritep7.spr>
#include <spritep8.spr>



/* sprites for missiles 
   sprite 15 = first traveling missile 
   sprite k1 = second traveling missile
   sprite k2 = second traveling missile

   sprite e1 = first exploding missile
   sprite e2 = second exploding missile
   sprite e3 = third exploding missile

*/

#include <sprite15.spr>
#include <spritek1.spr>
#include <spritek2.spr>
#include <spritee1.spr>
#include <spritee2.spr>
#include <spritee3.spr>

/* sprites used for bonus points 
   sprite z1 = first bonus object
*/

#include <spritez1.spr>


/* sprites used during movement of man 
   sprite 0  = blank square size of largest man

   sprite r0 = first man moving to the right
   sprite r1 = second man moving to the right
   sprite r2 = third man moving to the right
   sprite r4 = first man moving to the right with no cab
   sprite r5 = second man moving to the right with no cab
   sprite r6 = third man moving to the right with no cab
   sprite l0 = first man moving to the left
   sprite l1 = second man moving to the left
   sprite l2 = third man moving to the left
   sprite l4 = first man moving to the left with no cab
   sprite l5 = second man moving to the left with no cab
   sprite l6 = third man moving to the left with no cab
   sprite u0 = first man moving up
   sprite u1 = second man moving up
   sprite u2 = third man moving up
   sprite u4 = first man moving up with no cab
   sprite u5 = second man moving up with no cab
   sprite u6 = third man moving up with no cab
   sprite d0 = first man moving down
   sprite d0 = second man moving down
   sprite d2 = third man moving down
   sprite d4 = first man moving down with no cab
   sprite d5 = second man moving down with no cab
   sprite d6 = third man moving down with no cab

   sprite f1 = largest grave stone
   sprite f2 = smaller grave stone
   sprite f3 = smaller grave stone
   sprite f4 = smaller grave stone
   sprite f5 = smallest grave stone
   sprite f9 = truck turned over

*/

#include <sprite0.spr>
#include <spriter0.spr>
#include <spriter1.spr>
#include <spriter2.spr>
#include <spriter4.spr>
#include <spriter5.spr>
#include <spriter6.spr>
#include <spritel0.spr>
#include <spritel1.spr>
#include <spritel2.spr>
#include <spritel4.spr>
#include <spritel5.spr>
#include <spritel6.spr>
#include <spriteu0.spr>
#include <spriteu1.spr>
#include <spriteu2.spr>
#include <spriteu4.spr>
#include <spriteu5.spr>
#include <spriteu6.spr>
#include <sprited0.spr>
#include <sprited1.spr>
#include <sprited2.spr>
#include <sprited4.spr>
#include <sprited5.spr>
#include <sprited6.spr>
#include <spritef1.spr>
#include <spritef2.spr>
#include <spritef3.spr>
#include <spritef4.spr>
#include <spritef5.spr>
#include <spritef9.spr>



init_b_images()
{
	mis_num=0;	/* missle image counter */

	man_count=0; man_inc=1;

	sp_init(0,image_0,outln_0,bkgnd_0,X_WID_0,Y_WID_0,0,0);

	sp_init(14,image_z1,outln_z1,bkgnd_z1,X_WID_z1,Y_WID_z1,0,0);	/* bonus object */

	sp_init(15,image_15,outln_15,bkgnd_15,X_WID_15,Y_WID_15,0,0);	/* missile */

}


rst_b_images()
{
	mis_num=0;	/* missle image counter */

	man_count=0; man_inc=1;

	sp_chnge(0,image_0,outln_0,X_WID_0,Y_WID_0,0,0);

	sp_chnge(14,image_z1,outln_z1,X_WID_z1,Y_WID_z1,0,0);	/* bonus object */

	sp_chnge(15,image_15,outln_15,X_WID_15,Y_WID_15,0,0);	/* missile */

}



/* background erase routines */

era_right(x,y)	/* erase background in right direction */
int	x,y;
{
	sp_blank(x+16,y-1,X_WID_rt,Y_WID_rt);
	sp_put(x+16,y-1,image_rt,outln_rt,X_WID_rt,Y_WID_rt);
	sp_disp();

}



era_left(x,y)	/* erase background in left direction */
int	x,y;
{
	sp_blank(x-8,y-1,X_WID_lt,Y_WID_lt);
	sp_put(x-8,y-1,image_lt,outln_lt,X_WID_lt,Y_WID_lt);
	sp_disp();

}



era_up(x,y)	/* erase background in upward direction */
int	x,y;
{
	sp_blank(x-4,y-6,X_WID_up,Y_WID_up);
	sp_put(x-4,y-6,image_up,outln_up,X_WID_up,Y_WID_up);
	sp_disp();

}



era_down(x,y)	/* erase background in downward direction */
int	x,y;
{
	sp_blank(x-4,y+15,X_WID_dn,Y_WID_dn);
	sp_put(x-4,y+15,image_dn,outln_dn,X_WID_dn,Y_WID_dn);
	sp_disp();

}



era_b_down(x,y)	/* erase background in downward direction for falling money bags */
int	x,y;
{
	sp_blank(x-4,y+15,X_WID_bd,Y_WID_bd);
	sp_put(x-4,y+15,image_bd,outln_bd,X_WID_bd,Y_WID_bd);
	sp_disp();

}



era_b_first(x,y)	/* erase the first piece of background in downward direction for falling money bags */
int	x,y;
{
	sp_blank(x-4,y+17,X_WID_bf,Y_WID_bf);
	sp_put(x-4,y+17,image_bf,outln_bf,X_WID_bf,Y_WID_bf);
	sp_disp();

}



draw_background(lev)
int	lev;
{
	int	x, y;

	for(y=14; y<200; y+=4) {
		for(x=0; x<320; x+=20) {
			switch(lev) {
			case 1:
				sp_put(x,y,image_p1,outln_p1,X_WID_p1,Y_WID_p1);
				break;
			case 2:
				sp_put(x,y,image_p2,outln_p2,X_WID_p2,Y_WID_p2);
				break;
			case 3:
				sp_put(x,y,image_p3,outln_p3,X_WID_p3,Y_WID_p3);
				break;
			case 4:
				sp_put(x,y,image_p4,outln_p4,X_WID_p4,Y_WID_p4);
				break;
			case 5:
				sp_put(x,y,image_p5,outln_p5,X_WID_p5,Y_WID_p5);
				break;
			case 6:
				sp_put(x,y,image_p6,outln_p6,X_WID_p6,Y_WID_p6);
				break;
			case 7:
				sp_put(x,y,image_p7,outln_p7,X_WID_p7,Y_WID_p7);
				break;
			case 8:
				sp_put(x,y,image_p8,outln_p8,X_WID_p8,Y_WID_p8);
				break;
			}
		}
	}

}



draw_missile(x,y,mis_type)
int	x, y, mis_type;
{
	switch(mis_type) {
	case 0:
		mis_num++;
		if(mis_num>2)	/* get new image number */
			mis_num=0;
		switch(mis_num) {
		case 0:
			sp_chnge(15,image_15,outln_15,X_WID_15,Y_WID_15,0,0);
			break;
		case 1:
			sp_chnge(15,image_k1,outln_k1,X_WID_k1,Y_WID_k1,0,0);
			break;
		case 2:
			sp_chnge(15,image_k2,outln_k2,X_WID_k2,Y_WID_k2,0,0);
			break;
		}
		return(sp_move(15,x,y));
		break;
	case 1:
		sp_chnge(15,image_e1,outln_e1,X_WID_e1,Y_WID_e1,0,0);
		sp_move(15,x,y);
		break;
	case 2:
		sp_chnge(15,image_e2,outln_e2,X_WID_e2,Y_WID_e2,0,0);
		sp_move(15,x,y);
		break;
	case 3:
		sp_chnge(15,image_e3,outln_e3,X_WID_e3,Y_WID_e3,0,0);
		sp_move(15,x,y);
		break;
	}

}



draw_bonus(x,y)
int	x, y;
{

	sp_chnge(14,image_z1,outln_z1,X_WID_z1,Y_WID_z1,0,0);
	sp_place(14,x,y);

}



draw_man(dir,x,y,cab)
int	dir, x, y, cab;
{
	int	stat;

	man_count+=man_inc;

	if(man_count==2 || man_count==0)
		man_inc=-man_inc;

	if(man_count>2)	/* saftey feature - should never be needed */
		man_count=2;
	if(man_count<0)
		man_count=0;

	switch(dir) {
	case RIGHT:
		switch(man_count) {
		case 0:
			if(cab)
				sp_chnge(0,image_r0,outln_r0,X_WID_r0,Y_WID_r0,0,0);
			else
				sp_chnge(0,image_r4,outln_r4,X_WID_r4,Y_WID_r4,0,0);

			break;
		case 1:
			if(cab)
				sp_chnge(0,image_r1,outln_r1,X_WID_r1,Y_WID_r1,0,0);
			else
				sp_chnge(0,image_r5,outln_r5,X_WID_r5,Y_WID_r5,0,0);
			break;
		case 2:
			if(cab)
				sp_chnge(0,image_r2,outln_r2,X_WID_r2,Y_WID_r2,0,0);
			else
				sp_chnge(0,image_r6,outln_r6,X_WID_r6,Y_WID_r6,0,0);
			break;
		}

	
		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;


	case LEFT:
		switch(man_count) {
		case 0:
			if(cab)
				sp_chnge(0,image_l0,outln_l0,X_WID_l0,Y_WID_l0,0,0);
			else
				sp_chnge(0,image_l4,outln_l4,X_WID_l4,Y_WID_l4,0,0);
			break;
		case 1:
			if(cab)
				sp_chnge(0,image_l1,outln_l1,X_WID_l1,Y_WID_l1,0,0);
			else
				sp_chnge(0,image_l5,outln_l5,X_WID_l5,Y_WID_l5,0,0);
			break;
		case 2:
			if(cab)
				sp_chnge(0,image_l2,outln_l2,X_WID_l2,Y_WID_l2,0,0);
			else
				sp_chnge(0,image_l6,outln_l6,X_WID_l6,Y_WID_l6,0,0);
			break;
		}


		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;

	case UP:
		switch(man_count) {
		case 0:
			if(cab)
				sp_chnge(0,image_u0,outln_u0,X_WID_u0,Y_WID_u0,0,0);
			else
				sp_chnge(0,image_u4,outln_u4,X_WID_u4,Y_WID_u4,0,0);
			break;
		case 1:
			if(cab)
				sp_chnge(0,image_u1,outln_u1,X_WID_u1,Y_WID_u1,0,0);
			else
				sp_chnge(0,image_u5,outln_u5,X_WID_u5,Y_WID_u5,0,0);
			break;
		case 2:
			if(cab)
				sp_chnge(0,image_u2,outln_u2,X_WID_u2,Y_WID_u2,0,0);
			else
				sp_chnge(0,image_u6,outln_u6,X_WID_u6,Y_WID_u6,0,0);
			break;
		}

		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;

	case DOWN:
		switch(man_count) {
		case 0:
			if(cab)
				sp_chnge(0,image_d0,outln_d0,X_WID_d0,Y_WID_d0,0,0);
			else
				sp_chnge(0,image_d4,outln_d4,X_WID_d4,Y_WID_d4,0,0);
			break;
		case 1:
			if(cab)
				sp_chnge(0,image_d1,outln_d1,X_WID_d1,Y_WID_d1,0,0);
			else
				sp_chnge(0,image_d5,outln_d5,X_WID_d5,Y_WID_d5,0,0);
			break;
		case 2:
			if(cab)
				sp_chnge(0,image_d2,outln_d2,X_WID_d2,Y_WID_d2,0,0);
			else
				sp_chnge(0,image_d6,outln_d6,X_WID_d6,Y_WID_d6,0,0);
			break;
		}

		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;

	case STOP:
		break;

	case FALL_1:
		sp_chnge(0,image_f1,outln_f1,X_WID_f1,Y_WID_f1,0,0);
		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;
	case FALL_2:
		sp_chnge(0,image_f2,outln_f2,X_WID_f2,Y_WID_f2,0,0);
		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;
	case FALL_3:
		sp_chnge(0,image_f3,outln_f3,X_WID_f3,Y_WID_f3,0,0);
		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;
	case FALL_4:
		sp_chnge(0,image_f4,outln_f4,X_WID_f4,Y_WID_f4,0,0);
		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;
	case FALL_5:
		sp_chnge(0,image_f5,outln_f5,X_WID_f5,Y_WID_f5,0,0);
		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;
	case FALL_9:
		sp_chnge(0,image_f9,outln_f9,X_WID_f9,Y_WID_f9,0,0);
		stat=sp_move(0,x,y);	/* draw the new image & get colision info */
		break;
#ifdef	DEBUG
	default:
		cprintf("error in switch - draw man \n\r");
		break;
#endif
	}

	return(stat);	/* return colision info */


}



