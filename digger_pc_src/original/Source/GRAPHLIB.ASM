	page	,132

	title	>	Medium Resolution Graphics Library

	comment	$

	++++++ sp_blank uses BYTE values for the X_WIDTH instead of pic_cell values ++++
===============================================================================

                     Medium Resolution Graphics Library
                  ----------------------------------------

Description:

	The entry points to these routines were designed to link with modules
	created with the Lattice C Compiler and are called as external C
	procedures.


Routines Supported:

	med_res()		- set color display to medium resolution mode
				- display message on monochrome to turn on color display
				  or error message if no color card present
				- TRUE is returned if color card present, FALSE if not

	palette(n)		- set color card to palette 0 or 1

	backgrnd(color)		- set background to `color'

	clear(color)		- clear screen to `color'

	pset(x,y,color)		- set point x,y to `color'

	xpset(x,y,color)	- exclusive-or point x,y with `color'

	line(x1,y1,x2,xy,color)	- draw line x1,y1 x2,y2 in `color'

	xline(x1,y1,x2,y2,color) - exclusive-or a line x1,y1 x2,y2 with `color'

===============================================================================

             <<<<<<<<<<  Sprite Graphics Routines  >>>>>>>>>>

Description:

	The following routines are designed to simulate a hardware oriented
	sprite graphics system. Routines are called to initialize the software
	with the number of sprites being used. The software is designed to
	allow images to pass over a colored background without the `exclusive-or'
	effect and still leaving the background intact. Also, collision detection
	is supported by the returning a 16 bit integer, each bit signifying another
	image. If a collision was detected, the appropriate bit will be set.
	16 sprites are supported with complete prioritzation. Initially Sprite 0
	has the lowest priority and 15 has the highest but all prioritization
	is completely re-definable.

Commands:

	<<<<<  Initialize Sprite  >>>>>

sp_init(n,image,outline,backgnd,x_width,y_width,x_trim,y_trim);
int	n, x_width, y_width;
char	*image, *outline, *backgnd;

		n        - sprite number (0 to 15)
		*image   - pointer to an array which contains the sprite image
		*outline - pointer to an array which contains the sprite outline
		*backgnd - pointer to an array which will contain the background image
		x_width  - x width of sprite
		y_width  - y width of sprite
		x_trim   - number of x pixels from edges to ignore during collision check
		y_trim   - number of y pixels from edges to ignore during collision check

Description:

         *image	- This array contains the sprite image information. Each byte
		  contains information for 4 pixels.

       *outline	- This array contains the outline information of the image. For
		  every pixel that is wanted to be placed, the bit pattern must
		  be 00b (binary). If the background color is wanted at a location,
		  11b (binary) must be used. As above, each byte contains information
		  for 4 pixels.

       *backgnd - This array will store the background inforamtion for the present
		  location of the image.

x_width,y_width - x and y widths of sprite.

 x_trim, y_trim - This specifies to number of pixels to ignore from the left, right
		  top, and bottom edges of the sprite to ignore when returning
		  collision information.



	<<<<<<<<<<  Place Sprite  >>>>>>>>>>

sp_place(n,x,y);
int	n, x, y;

	n - sprite number (0 to 15) - collision information is returned
	x - x location of sprite
	y - y location of sprite

Description:

	The sprite specified by n is placed on the screen at location x,y. Background
	information is stored in the background buffer. Movement of the sprite requires
	the use of the sp_move command. Collision information is returned.



	<<<<<<<<<<  Erase Sprite  >>>>>>>>>>

sp_erase(n);
int	n;

	n - sprite number (0 to 15)

Description:

	The specified sprite is erased by dumping the background buffer. The
	sprite however may be re-placed by using the sp_place command again.



	<<<<<<<<<<  Move Sprite  >>>>>>>>>>

sp_move(n,x,y);
int	n, x, y;

	n - sprite number (0 to 15)
	x - new x location of sprite
	y - new y location of sprite

Description:

	This routine will move a sprite from one location to another while
	still leaving the background intact. Collision information is returned.



	<<<<<<<<<<  Change Sprite Image  >>>>>>>>>>

sp_chnge(n,image,outline,x_width,y_width,x_trim,y_trim);
int	n, x_width, y_width;
char	*image, *outline;

              n	- sprite number (0 to 15)
        x_width - x width of new sprite
        y_width - y width of new sprite
         *image	- new image pointer
       *outline	- new outline pointer
 x_trim, y_trim - new trim factors

Description:

	The sprite image specified will switch its image and outline buffers
	to the new ones specified. The next sp_move will erase the old image
	and place the new image at the specified location. If the image was
	already erased when sp_chnge is used, the next sp_place will place
	the newly defined image.



	<<<<<<<<<<  Change Sprite Priority  >>>>>>>>>>

sp_table(array);
int	*array;

	array - integer array showing new sprite priorities.

Description:

	When sprites are drawn on the screen, the priority of the sprites
	(the order in which they are re-drawn) can be dynamically changed
	simply by passing the address of the array which contains the new
	priorities. If 0 is passed as the address, the internal table will
	be used with sprite 0 having the lowest priority and sprite 15
	having the highest. Otherwise, the passed array must have 16 sprite
	numbers stored in the order in which they are to be re-drawn. The
	first element is drawn first.



	<<<<<<<<<<  Put Sprite Image  >>>>>>>>>>

sp_put(x,y,image,outline,x_width,y_width);
int	x, y, x_width, y_width;
char	*image, *outline;

        x, y - coordinates of image
     x_width - x width of image
     y_width - y width of image
      *image - address of image array
    *outline - address of outline array

Description:

	This routine will place the specified sprite on the screen and is
	then forgotten. It is then part of the background. This is used
	for screen initialization to place permanent objects on the screen.
	Note that the outline array is still needed and used.



	<<<<<<<<<<  Blank Conflicting Sprites  >>>>>>>>>>

sp_blank(x,y,x_width,y_width);
int	x, y, x_width, y_width;

	x, y		 - coordinates of top left cornor of area
	x_width, y_width - widths of area

Description:

	This routine will erase any sprites that are within the specified
	area. They may be re-place by using the sp_disp command.



	<<<<<<<<<<  Display Blanked Sprites  >>>>>>>>>>

sp_disp();

Description:

	This routine will re-display any sprites blanked by the sp_blank
	command.


	<<<<<<<<<<  Check area for Conflicting Sprites  >>>>>>>>>>

sp_check(x,y,x_width,y_width);
int	x, y, x_width, y_width;

	x, y		 - coordinates of top left cornor of area
	x_width, y_width - widths of area

Description:

	This routine will check if any sprites that are within the specified
	area.


	<<<<<<<<<<  Put Image  >>>>>>>>>>

put(x,y,image,color);
int	x, y, color;
char	*image;

Description:

	This routine will place an image at the specified location. Note that
	the x and y widths are stored as the first two elements of the image
	array. Also there is no outline array used for this type of put. This
	put is basically used placing text and scoring on the screen. Each
	pixel will be and'd with the value of `color'. If the image is stored
	as white/yellow, then the image can be plotted in cyan/green or magenta/red
	depending on the value of `color'.
$

;-------------------------------------------------
;
;	<<<<<  Constant Declarations >>>>>
;
; ------------------------------------------------



; -----  Misc  -----

FALSE		equ	0
TRUE		equ	-1

CR		equ	13		; carrige return
LF		equ	10		; line feed

	page

; ------------------------------------------------
;
;	<<<<<  Data Segment Definitions  >>>>>
;
; ------------------------------------------------




; -----  ROM Bios Data Segment  -----

biosdata_seg	segment at 40h

		org	10h

equip_flag	db	?		; equipment flags

biosdata_seg	ends



; -----  Color Card Segment  -----

color_seg	segment at 0b800h

		org	0

color_base	label	byte		; base of color card

color_seg	ends

	page

;------------------------------------------------
;
;	-----  Program Data Segment  -----
;
;------------------------------------------------


dgroup	group	data
data	segment	byte public 'data'


mes_1		db	25 dup (CR,LF)
		db	'                      Please turn on display connected to'
		db	2 dup (CR,LF)
		db	'                      your Color Graphics Display Adapter'
		db	11 dup (CR,LF)
		db	'$'
		

mes_2		db	25 dup (CR,LF)
		db	'          This program requires the Color/Graphics Display Adapter'
		db	14 dup (CR,LF)
		db	'$'

; -----  System Varibles  -----

v_sync		db	TRUE		; TRUE = wait for verticle sync
x_op		db	?		; exclusive-or operation flag
color_byte	db	?		; color select byte
pix_color	db	?		; color of pixels to write

xstart		dw	?		; line x start coordinate
ystart		dw	?		; line y start coordinate
xend		dw	?		; line x end coordinate
yend		dw	?		; line y end coordinate

d1		dw	?		; misc variables for line
d2		dw	?
d3		dw	?
a1		dw	?
a2		dw	?
s1		dw	?
s2		dw	?
n1		dw	?
	
; -----  Sprite Variables and Arrays

col_flag	dw	17 dup(?)	; collision flag table
col_check	dw	17 dup(?)	; collision check table

active		dw	16 dup(FALSE)	; sprite enable flag
image		dw	17 dup(?)	; sprite image address
outline		dw	17 dup(?)	; sprite outline address
backgnd		dw	16 dup(?)	; background buffer address
sp_x		dw	17 dup(?)	; x coordinates of sprites
sp_y		dw	17 dup(?)	; y coordinates of sprites
x_width		dw	17 dup(?)	; x width of sprites
y_width		dw	17 dup(?)	; y width of sprites
x_trim		dw	16 dup(?)	; x coordinate trim
y_trim		dw	16 dup(?)	; y coordinate trim

new_image	dw	16 dup(?)	; new image address
new_outline	dw	16 dup(?)	; new outline address
new_x		dw	?		; new sprite x coordinate
new_y		dw	?		; new sprite y coordinate
new_x_width	dw	16 dup(?)	; new x width
new_y_width	dw	16 dup(?)	; new y width
new_x_trim	dw	16 dup(?)	; new x trim
new_y_trim	dw	16 dup(?)	; new y trim

color_mask	db	?		; color mask for put
even_line	db	?		; even/odd line flag

; ----------  Priority Table

table_addr	dw	offset dgroup:intern_table	; address of table

intern_table	dw	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15

data		ends

	page

;-------------------------------------------
;
;	-----  Program Segment  -----
;
;-------------------------------------------


pgroup	group	prog
prog	segment	byte public 'prog'

	public	med_res, clear, palette, backgrnd
	public	pset, xpset, line, xline
	public	sp_init, sp_place, sp_erase, sp_move, sp_chnge, sp_table
	public	sp_blank, sp_check, sp_disp, sp_put, put

	assume	cs:pgroup, ds:dgroup, es:dgroup, ss:dgroup



; ==========  Initialize Color Card to Medium Resolution  ==========
;
; Entry:
;
;	med_res();
;
; Description:
;
;	The color card will be initialzed to display medium resolution
;	graphics in color palette 1 and a message will be displayed on
;	the monochrome saying to turn on the color display. A TRUE will
;	be returned. If there is no color card, a message will be dis-
;	played on the monochrome saying so and a FALSE will be returned.


med_res	proc	near

	push	bp
	mov	bp,sp

	call	_med_res		; call routine

	pop	bp
	ret

med_res	endp



_med_res	proc	near

	mov	si,offset dgroup:mes_1
	call	monomes
	call	chk_disp		; check for color card
	jne	no_card			; jump if no card

card:					; color card present
	call	init_crt		; initialize crt controller
	mov	ax,TRUE			; return TRUE
	ret

no_card:				; color card not present
	mov	si,offset dgroup:mes_2
	call	monomes			; display error message on monochrome
	mov	ax,FALSE		; return FALSE
	ret


chk_disp:				; check for color card
	assume	ds:color_seg

	push	ds
	mov	ax,color_seg
	mov	ds,ax			; set ES to color card segment

	mov	al,55h
	mov	color_base,al		; store a value in color card
	cmp	al,color_base		; compare values
	pop	ds
	ret				; CY = TRUE if no color card

	assume	ds:dgroup


init_crt:				; initialize color card to medium res
	assume	es:biosdata_seg

	push	es
	mov	ax,biosdata_seg				      
	mov	es,ax

	mov	al,equip_flag		; get equipment flag
	and	al,0cfh
	or	al,20h			; show in 80 colum color
	mov	equip_flag,al
	pop	es
	assume	es:dgroup

	mov	ah,0			; set video mode
	mov	al,4			; set to 320x200 color graphics
	int	10h

	mov	ah,11
	mov	bh,1			; palette
	mov	bl,0
	int	10h
	mov	ah,11			; set background color
	mov	bh,0
	mov	bl,1
	int	10h	

	ret


monomes:				; send message *SI to monochrome
	assume	es:biosdata_seg

	push	es
	mov	ax,biosdata_seg				      
	mov	es,ax

	mov	al,equip_flag		; get equipment flag

	pop	es
	assume	es:dgroup

	and	al,30h
	cmp	al,30h			; check monochrome flag bits
	jz	pmes			; jump if monochrome present
	ret				; else return

pmes:
	mov	cx,2000h		; set for no cursor
	mov	ah,1			; set cursor command
	int	10h			; call video I/O
	mov	bx,0
	mov	al,[si]			; get character
	cmp	al,'$'			; check for string end
	je	pmesend			; jump if end
	mov	ah,14			; TTY write routine
	push	si
	int	10h			; video I/O call
	pop	si
	inc	si			; point to next chracter
	jmp	pmes

pmesend:
	ret

_med_res	endp

	page



; ==========  Clear Screen  ==========
;
; Entry:
;
;	clear(color);
;	int	color;
;
; Description:
;
;	The screen will be cleared to `color'.


clear	proc	near

	push	bp
	mov	bp,sp

	mov	al,[bp]+4		; get color to clear screen in al
	call	_clear			; clear screen

	pop	bp
	ret

clear	endp


_clear	proc	near

	and	al,3			; look at 2 lsb only
	mov	cl,2
	mov	dl,al			; set bits
	shl	al,cl			; shift left 2
	or	dl,al			; set bits
	shl	al,cl			; shift left 2
	or	dl,al			; set bits
	shl	al,cl			; shift left 2
	or	dl,al			; set bits
	mov	dh,dl
	
	assume	ds:color_seg

	push	ds
	mov	ax,color_seg
	mov	ds,ax				; set DS to color card segment
	mov	bx,0				; point to beginning of segment
	mov	cx,8192				; loop to clear all words in display

cl1:
	mov	[bx],dx				; clear byte
	inc	bx
	inc	bx				; point to next word
	loop	cl1				; loop until done

	pop	ds
	ret

	assume	ds:dgroup

_clear	endp




; ==========  Set Color Palette  ==========
;
; Entry:
;
;	palette(n);
;	int	n;
;
; Description:
;
;	The color palette specified by n will be selected.


palette		proc	near

	push	bp
	mov	bp,sp

	mov	ah,11				; set color palette
	mov	bh,1
	mov	bl,[bp]+4			; get palette number
	int	10h

	pop	bp
	ret

palette		endp



; ==========  Set Background Color  ==========
;
; Entry:
;
;	backgnd(color);
;	int	color;
;
; Description:
;
;	The background color will be set to `color'


backgrnd	proc	near

	push	bp
	mov	bp,sp

	mov	ah,11			; set background color
	mov	bh,0
	mov	bl,[bp]+4		; get background color
	int	10h

	pop	bp
	ret

backgrnd	endp



; ==========  Set Point  ==========
;
; Entry:
;
;	pset(x,y,color);
;	int	x, y, color;
;
; Description:
;
;	The point x,y will be set to `color'.


pset	proc	near

	push	bp
	mov	bp,sp

	mov	bx,[bp]+4		; get x coordinate
	mov	cx,[bp]+6		; get y coordinate
	mov	al,[bp]+8		; get color
	and	al,3
	mov	pix_color,al		; store color
	mov	x_op,FALSE		; not exclusive-or operation
	call	write_point		; call write point routine

	pop	bp
	ret

pset	endp




; ==========  Exclusive-or Point  ==========
;
; Entry:
;
;	xpset(x,y,color);
;	int	x, y, color;
;
; Description:
;
;	The point x,y will be exclusive-or'd with `color'


xpset	proc	near

	push	bp
	mov	bp,sp

	mov	bx,[bp]+4		; get x coordinate
	mov	cx,[bp]+6		; get y coordinate
	mov	al,[bp]+8		; get color
	and	al,3
	mov	pix_color,al		; store color
	mov	x_op,TRUE		; not exclusive-or operation
	call	write_point		; call write point routine

	pop	bp
	ret

xpset	endp

	page


; <<<<<<<<<<  Write Point Routine  >>>>>>>>>>
;
; Entry:
;
;	bx        = x coordinate
;	cx        = y coordinate
;	pix_color = color to set/exclusive-or pixel
;	x_op      = TRUE - exclusive-or point, FALSE - set point
;
; Exit:
;
;	ax = TRUE if point is valid, FALSE if not valid
;
;
; Description:
;
;	The coordinates of the point will be verified as valid. If the
;	point is  valid, the pixel will be set or exclusive-or'd with
;	`pix_color' depending of the condition of `x_op'. A TRUE will
;	be returned. If the point is not valid, no screen operation will
;	take place and a FALSE will be returned.


write_point	proc	near

	call	chk_point		; verify point is valid
	jc	w2			; jump if point valid
	mov	ax,FALSE		; signal point is unvalid
	ret

w2:
	call	point_addr		; get address of point in DI
	call	plot_point		; plot point
	mov	ax,TRUE			; signal valid point
	ret


; -----  check x < 320 and y < 200  ///  CY = TRUE if point valid

chk_point:

	cmp	bx,320			; check x coordinate
	jb	chk2			; jump if below 320
	ret
chk2:
	cmp	cx,200			; check y coordinate
	ret


; -----  get address of point BX,CX in DI

point_addr:

	push	cx			; save y
	and	cl,0feh			; reset lsb
	mov	al,40
	mul	cl			; multiply y by 40
	pop	cx			; restore y
	test	cl,1			; check if even or odd scan line
	jz	p2			; jump if even scan line
	add	ax,2000h		; add 8000 to address for odd scan line

p2:
	push	bx
	shr	bx,1
	shr	bx,1			; ignore 2 lsb
	add	ax,bx
	mov	di,ax			; di = address of byte containing pixel
	pop	bx
	ret


; -----  plot point at address pointed to by DI

plot_point:

	push	bx
	push	cx
	and	bl,3			; look at 2 lsb
	xor	bl,3
	shl	bl,1			; double
	mov	cl,bl
	mov	dl,pix_color		; get color of pixel
	shl	dl,cl			; shift color bits into proper position
	mov	dh,3
	shl	dh,cl			; shift mask bits into proper position
	cmp	x_op,0			; check for exclusive-or operation
	jne	xplot			; jump if exclusive_or point

; -----  set point
;
; di = address, dl = color mask, dh = reset mask

	assume	ds:color_seg

	push	ds
	mov	ax,color_seg
	mov	ds,ax			; set DS to color card segment

	mov	al,[di]			; get byte
	not	dh			; complement mask
	and	al,dh			; reset color bits
	or	al,dl			; set new color bits
	mov	[di],al			; write out new byte
	pop	ds
	pop	cx
	pop	bx
	ret

	assume	ds:dgroup


; -----  exclusive-or point
;
; di = address, dl = color mask

xplot:
	assume	ds:color_seg

	push	ds
	mov	ax,color_seg
	mov	ds,ax

	xor	[di],dl
	pop	ds
	pop	cx
	pop	bx
	ret

	assume	ds:dgroup

write_point	endp


	page

; ==========  Draw Line  ==========
;
; Entry:
;
;	line(x1,y1,x2,y2,color);
;	int	x1, y1, x2, y2, color;
;
; Description:
;
;	A line will be drawn from x1,y1, to x2,y2 in `color'


line	proc	near

	push	bp
	mov	bp,sp

	mov	ax,[bp]+4
	mov	xstart,ax		; store x1
	mov	ax,[bp]+6
	mov	ystart,ax		; store y1
	mov	ax,[bp]+8
	mov	xend,ax			; store x2
	mov	ax,[bp]+10
	mov	yend,ax			; store y2
	mov	ax,[bp]+12
	and	al,3
	mov	pix_color,al		; store color
	mov	x_op,FALSE
	call	draw_line		; call routine

	pop	bp
	ret

line	endp




; ==========  Exclusive-Or Line  ==========
;
; Entry:
;
;	xline(x1,y1,x2,y2,color);
;	int	x1, y1, x2, y2, color;
;
; Description:
;
;	A line will be exclusive-or'd from x1,y1, to x2,y2 with `color'


xline	proc	near

	push	bp
	mov	bp,sp

	mov	ax,[bp]+4
	mov	xstart,ax		; store x1
	mov	ax,[bp]+6
	mov	ystart,ax		; store y1
	mov	ax,[bp]+8
	mov	xend,ax			; store x2
	mov	ax,[bp]+10
	mov	yend,ax			; store y2
	mov	ax,[bp]+12
	and	al,3
	mov	pix_color,al		; store color
	mov	x_op,TRUE
	call	draw_line		; call routine

	pop	bp
	ret

xline	endp

	page


; <<<<<<<<<<  Draw Line Routine  >>>>>>>>>>
;
; Entry:
;
;	xstart = x1, ystart = y2, xend = x2, yend = y2,
;
;	pix_color = color
;
;	x_op = TRUE  - exclusive-or line, FALSE - plot line


draw_line	proc	near

	mov	ax,xend
	sub	ax,xstart
	mov	d1,ax			; d1 = dx
	mov	ax,yend
	sub	ax,ystart
	mov	d2,ax			; d2 = dy
	mov	s1,0			; initialize variables
	mov	s2,1
	mov	a1,1
	mov	a2,0

l1:
	cmp	d1,0
	jge	l2			; jump if dx >= 0
	mov	a1,-1			; a1 = -1
	neg	d1			; d1 = -d1

l2:
	cmp	d2,0
	jge	l3			; jump if dy >= 0
	mov	s2,-1			; s2 = -1
	neg	d2			; d2 = -d2

l3:
	mov	ax,d2
	cmp	d1,ax
	jge	l4			; jump if d1 >= d2
	push	d1
	push	d2
	pop	d1
	pop	d2			; swap d1 with d2
	mov	ax,a1
	mov	s1,ax			; s1 = a1
	mov	a1,0			; a1 = 0
	mov	ax,s2
	mov	a2,ax			; a2 = s2
	mov	s2,0			; s2 = 0

l4:
	mov	ax,d1
	shr	ax,1
	mov	d3,ax			; d3 = d1/2
	mov	n1,1			; n1 = 1
	mov	bx,xstart
	mov	cx,ystart
	call	write_point		; plot point

l5:
	mov	ax,d1
	cmp	n1,ax
	jle	l6			; jump if n1 > d1
	ret

l6:
	add	bx,a1			; xstart = xstart + a1
	add	cx,a2			; ystart = ystart + a2
	mov	ax,d2
	add	d3,ax			; d3 = d3 + d2
	inc	n1
	mov	ax,d1
	cmp	d3,ax
	jle	l7			; jump if d3 <+ d1
	mov	ax,d1
	sub	d3,ax			; d3 = d3 - d1
	add	bx,s1			; xstart = xstart + s1
	add	cx,s2			; ystart = ystart + s2

l7:
	call	write_point		; plot point
	jmp	l5


draw_line	endp


; ==========  Sprite Initialize  ==========
;
; Entry:
;
;	sp_init(n,image,outline,backgnd,x_width,y_width,x_trim,y_trim);
;	int	n, x_width, y_width, x_trim, y_trim;
;	char	*image, *outline, *backgnd;


sp_init	proc	near

	push	bp
	mov	bp,sp

	mov	bx,[bp]+4		; get sprite number in bx
	and	bx,15			; maximum 16 sprites
	shl	bx,1			; multiply by 2 for 16 bit array address

	mov	ax,[bp]+6
	mov	new_image[bx],ax
	mov	image[bx],ax		; store image address
	mov	ax,[bp]+8
	mov	new_outline[bx],ax
	mov	outline[bx],ax		; store outline address
	mov	ax,[bp]+10
	mov	backgnd[bx],ax		; store background buffer address
	mov	ax,[bp]+12
	mov	new_x_width[bx],ax
	mov	x_width[bx],ax		; store x width of sprite
	mov	ax,[bp]+14
	mov	new_y_width[bx],ax
	mov	y_width[bx],ax		; store y width of sprite
	mov	ax,[bp]+16
	mov	new_x_trim[bx],ax
	mov	x_trim[bx],ax		; store x trim factor
	mov	ax,[bp]+18
	mov	new_y_trim[bx],ax
	mov	y_trim[bx],ax		; store y trim factor

	mov	active[bx],FALSE	; not active till placed

	pop	bp
	ret

sp_init	endp

	page

; ==========  Place Sprite Entry Point  ==========
;
; Entry:
;
;	sp_place(n,x,y);
;	int	n, x, y;


sp_place	proc	near

	push	bp
	mov	bp,sp

	mov	bx,[bp]+4		; get sprite number
	and	bx,15
	shl	bx,1			; get 16 bit array address

	mov	ax,[bp]+6
	and	ax,0fffch		; clear 2 lsb
	mov	sp_x[bx],ax		; store x location
	mov	ax,[bp]+8
	mov	sp_y[bx],ax		; store y loation

	call	_sp_place		; call routine

	pop	bp
	ret

sp_place	endp

	page

; ==========  Erase Sprite Entry Point  ==========
;
; Entry:
;
;	sp_erase(n);
;	int	n;


sp_erase	proc	near

	push	bp
	mov	bp,sp

	mov	bx,[bp]+4		; get sprite number
	and	bx,15
	shl	bx,1			; get 16 bit array address
	call	_sp_erase		; call erase sprite routine

	pop	bp
	ret

sp_erase	endp

	page

; ==========  Move Sprite Entry Point  ==========
;
; Entry:
;
;	sp_move(n,x,y);
;	int	n, x, y;


sp_move		proc	near

	push	bp
	mov	bp,sp

	mov	bx,[bp]+4	; get sprite number
	and	bx,15
	shl	bx,1		; get point to sprite arrays

	mov	ax,[bp]+6	; get new x coordinate
	and	ax,0fffch	; ignore 2 lsb
	mov	new_x,ax	; store it
	mov	ax,[bp]+8	; get new y coordinate
	mov	new_y,ax	; store it
	call	_sp_move	; call move sprite routine

	pop	bp
	ret

sp_move		endp

	page

; ==========  Change Sprite Image  ==========
;
; Entry:
;
;	sp_change(n,image,outline,x_width,y_width,x_trim,y_trim);
;	int	n, x_width, y_width, x_trim, y_trim;
;	char	*image, *outline;


sp_chnge	proc	near

	push	bp
	mov	bp,sp

	mov	bx,[bp]+4		; get sprite number
	and	bx,15
	shl	bx,1			; get pointer address

	mov	ax,[bp]+6		; get new image address
	mov	new_image[bx],ax	; store it
	mov	ax,[bp]+8		; get new outline address
	mov	new_outline[bx],ax	; store it
	mov	ax,[bp]+10		; get new x width
	mov	new_x_width[bx],ax	; store it
	mov	ax,[bp]+12		; get new y width
	mov	new_y_width[bx],ax	; store it
	mov	ax,[bp]+14		; get new x trim
	mov	new_x_trim[bx],ax	; store it
	mov	ax,[bp]+16		; get new y trim
	mov	new_y_trim[bx],ax	; store it

	pop	bp
	ret

sp_chnge	endp

	page

; ==========  Change Sprite Priority Table  ==========
;
;	sp_table(array);
;	int	*array;


sp_table	proc	near

	push	bp
	mov	bp,sp

	mov	ax,[bp]+4		; get address of new priority table
	cmp	ax,0			; check if internal table wanted
	jne	spt1				; jump if not wanted
	mov	ax,offset dgroup:intern_table	; internal table address

spt1:
	mov	table_addr,ax		; store new table address
	pop	bp
	ret

sp_table	endp

	page

; ==========  Blank Conflicting Sprite  ==========
;
;	sp_blank(x,y,x_width,y_width);
;	int	x, y, x_width, y_width;


sp_blank	proc	near

	push	bp
	mov	bp,sp

	mov	bx,16*2			; point to 17th element

	mov	ax,[bp]+4
	mov	sp_x[bx],ax		; store x coordinate
	mov	ax,[bp]+6
	mov	sp_y[bx],ax		; store y coordinate
	mov	ax,[bp]+8
;	shr	ax,1
;	shr	ax,1			; ignore 2 lsb
;	inc	ax			; round off
	mov	x_width[bx],ax		; store x width
	mov	ax,[bp]+10
	mov	y_width[bx],ax		; store y width

	call	_sp_blank		; call routine

	pop	bp
	ret

sp_blank	endp




; ==========  Display Blanked Sprites  ==========
;
;	sp_disp();

sp_disp	proc	near

	push	bp
	mov	bp,sp

	call	bkgnd_all		; get new background for all flagged sprites
	call	replace_all		; call routine

	pop	bp
	ret

sp_disp	endp

	page


; ==========  Check Conflicting Sprite  ==========
;
;	sp_check(x,y,x_width,y_width);
;	int	x, y, x_width, y_width;


sp_check	proc	near

	push	bp
	mov	bp,sp

	mov	bx,16*2			; point to 17th element

	mov	ax,[bp]+4
	mov	sp_x[bx],ax		; store x coordinate
	mov	ax,[bp]+6
	mov	sp_y[bx],ax		; store y coordinate
	mov	ax,[bp]+8
	shr	ax,1
	shr	ax,1			; ignore 2 lsb
	inc	ax			; round off
	mov	x_width[bx],ax		; store x width
	mov	ax,[bp]+10
	mov	y_width[bx],ax		; store y width

	call	_sp_check		; call routine

	pop	bp
	ret

sp_check	endp




; ==========  Put Sprite Image  ==========
;
;	sp_put(x,y,image,outline,x_width,y_width);
;	int	x, y, x_width, y_width;
;	char	*image, *outline;


sp_put	proc	near

	push	bp
	mov	bp,sp

	mov	bx,16*2			; point to 17th array element

	mov	ax,[bp]+4
	and	ax,0fffch
	mov	sp_x[bx],ax		; store x coordinate
	mov	ax,[bp]+6
	mov	sp_y[bx],ax		; store y coordinate
	mov	ax,[bp]+8
	mov	image[bx],ax		; store image address
	mov	ax,[bp]+10
	mov	outline[bx],ax		; store outline address
	mov	ax,[bp]+12
	mov	x_width[bx],ax		; store x width
	mov	ax,[bp]+14
	mov	y_width[bx],ax		; store y width

	call	put_sp			; call routine

	pop	bp
	ret

sp_put	endp

	page

; ==========  Put Image Routine  ==========
;
;	put(x,y,image,color);
;	int	x, y, color;
;	char	*image;


put	proc	near

	push	bp
	mov	bp,sp

	mov	bx,16*2			; point to 17th element

	mov	ax,[bp]+4
	and	ax,0fffch
	mov	sp_x[bx],ax		; store x coordinate
	mov	ax,[bp]+6
	mov	sp_y[bx],ax		; store y coordinate

	mov	ah,0			; clear ah
	mov	si,[bp]+8		; get address of array
	mov	al,[si]			; get x width
	mov	x_width[bx],ax		; store x width
	mov	al,[si]+1		; get y width
	mov	y_width[bx],ax		; store y width
	add	si,2			; point to first image element
	mov	image[bx],si		; store address

	mov	dl,[bp]+10		; get color mask value
	and	dl,3
	mov	al,0			; clear al
	mov	cl,2			; shift count
	or	al,dl			; get first pixel mask
	shl	dl,cl			; shift left 2 bits
	or	al,dl			; second pixel mask
	shl	dl,cl			; shift
	or	al,dl			; third pixel mask
	shl	dl,cl			; shift
	or	al,dl			; fourth pixel mask
	mov	color_mask,al		; store color mask

	call	put_image		; call put routine

	pop	bp
	ret

put	endp

	page

; <<<<<<<<<<  Place Sprite Routine  >>>>>>>>>>
;
; Description:
;
;	A check will be made to see if the new sprite location to be used will
;	overlap any existing sprites. If so, the confliction sprites are removed.
;	The background area of the sprite to be placed will be dumped to the
;	background buffer. Then the new sprite to be placed as well as the
;	previously removed sprites will be re_placed according to their priority.
;	Collision information from the overlap check is then returned.

_sp_place	proc	near

	mov	ax,new_image[bx]
	mov	image[bx],ax		; get new image address
	mov	ax,new_outline[bx]
	mov	outline[bx],ax		; get new outline address
	mov	ax,new_x_width[bx]
	mov	x_width[bx],ax		; get new x width
	mov	ax,new_y_width[bx]
	mov	y_width[bx],ax		; get new y width
	mov	ax,new_x_trim[bx]
	mov	x_trim[bx],ax		; get new x trim
	mov	ax,new_y_trim[bx]
	mov	y_trim[bx],ax		; get new y trim

	call	clear_table		; clear collision tables
	call	collision		; fill table with colliding sprites
	call	remove_all		; remove all sprites in table
	call	get_backgnd		; fill background buffer
	mov	active[bx],TRUE		; flag sprite as active
	mov	col_flag[bx],TRUE	; flag new sprite in table

	call	replace_all		; re-place new and prevoiusly removed sprites
	call	col_detect		; return collision information
	ret

_sp_place	endp




; <<<<<<<<<<  Sprite Erase Routine  >>>>>>>>>>
;
; Description:
;
;	The background buffer of the specified sprite will be dumped to
;	the screen to erase the sprite. A check will then be made to see
;	if there were any colliding sprites. If so, the sprites will be
;	re-placed on the screen.


_sp_erase	proc	near

	call	remove_sp		; remove specified sprite
	mov	active[bx],FALSE	; flag as inactive
	call	clear_table		; clear collision tables
	call	collision		; fill table with collided sprites
	call	replace_all		; replace sprites
	ret

_sp_erase	endp




; <<<<<<<<<<  Move Sprite Routine  >>>>>>>>>>
;
; Description:
;
;	The routine will first erase any conflicting sprites at the present
;	sprite location. Then it will erase conflicting sprites at its
;	destination. Then all sprites are re-drawn including the present
;	sprite at its new location according to their priority. Collision
;	information is then returned.


_sp_move	proc	near

	call	clear_table		; clear collision tables
	call	collision		; flag colliding sprites

	push	sp_x[bx]
	push	sp_y[bx]
	push	x_width[bx]
	push	y_width[bx]		; save old parameters

	mov	ax,new_x		; get new x coordinate
	mov	sp_x[bx],ax		; store it
	mov	ax,new_y		; get new y coordinate
	mov	sp_y[bx],ax		; store it

	mov	ax,new_x_width[bx]
	mov	x_width[bx],ax		; get possible new x width
	mov	ax,new_y_width[bx]
	mov	y_width[bx],ax		; get possible new y width

	call	clear_chk		; clear col_check table
	call	collision		; flag sprites at new location to be removed

	pop	y_width[bx]
	pop	x_width[bx]
	pop	sp_y[bx]
	pop	sp_x[bx]		; restore old parameters

	mov	col_flag[bx],TRUE	; flag present sprite to be removed
	call	remove_all		; remove flagged sprites

	mov	ax,new_x		; get new x coordinate
	mov	sp_x[bx],ax		; store it
	mov	ax,new_y		; get new y coordinate
	mov	sp_y[bx],ax		; store it

	mov	ax,new_image[bx]
	mov	image[bx],ax		; get possible new image address
	mov	ax,new_outline[bx]
	mov	outline[bx],ax		; get possible new outline address
	mov	ax,new_x_width[bx]
	mov	x_width[bx],ax		; get possible new x width
	mov	ax,new_y_width[bx]
	mov	y_width[bx],ax		; get possible new y width
	mov	ax,new_x_trim[bx]
	mov	x_trim[bx],ax		; get possible new x trim
	mov	ax,new_y_trim[bx]
	mov	y_trim[bx],ax		; get possible new y trim

	call	get_backgnd		; fill background buffer with new background
	call	replace_all		; replace all flagged sprites
	call	col_detect		; return collision information
	ret

_sp_move	endp




; <<<<<<<<<<  Blank Conflicting Sprites Routine  >>>>>>>>>>
;
; Description:
;
;	This routine will flag all sprites overlapping in the
;	specified area and then erase them.


_sp_blank	proc	near

	call	clear_table		; clear collision tables
	call	collision		; flag conflicting sprites
	call	remove_all		; remove them

	ret

_sp_blank	endp



; <<<<<<<<<<  Check for Conflicting Sprites Routine  >>>>>>>>>>
;
; Description:
;
;	This routine will flag all sprites overlapping in the
;	specified area.


_sp_check	proc	near

	call	clear_table		; clear collision tables
	call	collision		; flag conflicting sprites

	mov	bx,0			; point to first array element
	mov	dx,0			; sprite # 0
	mov	ax,0			; clear colision register
chp1:
	cmp	col_flag[bx],TRUE	; check if sprite flagged
	jne	chloop			; jump if not
	mov	di,1			; set bit 0
	mov	cl,dl			; put sprite number into cl
	shl	di,cl			; shift bit `sprite' number of times
	or	ax,di			; set bit in ax


chloop:
	add	bx,2			; point to next element
	inc	dx			; next sprite number
	cmp	dx,16			; do all sprites
	jne	chp1
	ret

_sp_check	endp



; ----------  Clear Collision Tables  ----------
;
; Description:
;
;	This routine will clear the collision tables.


clear_table	proc	near

	call	clear_chk
	push	bx
	mov	cx,17			; clear all 16 positions
	mov	bx,0			; point to first array element

clr1:
	mov	col_flag[bx],FALSE	; clear entry
	add	bx,2			; point to next element
	loop	clr1			; loop until done

	pop	bx
	ret

clear_table	endp




; ----------  Clear Collision Check Table  ----------

clear_chk	proc	near

	push	bx
	mov	cx,17			; clear all 17 positions
	mov	bx,0			; point to first array element

cler1:
	mov	col_check[bx],FALSE	; clear entry
	add	bx,2			; point to next element
	loop	cler1			; loop until done

	pop	bx
	ret

clear_chk	endp




; ----------  Sprite Collision Check  ----------
;
; Description:
;
;	A check will be made to see if the present sprite location will cause
;	overlapping or a collision with any other active sprites. If so, the
;	affected sprites will be flagged in the col_flag table.
;
; Entry:
;
;	bx = pointer to sprite arrays
;
; Exit:
;
;	bx is preserved


collision	proc	near

	push	bx
	push	si

	cmp	col_check[bx],TRUE	; has this sprite already been checked
	je	col_end			; jump if it has

	mov	col_check[bx],TRUE	; flag sprite as alread checked
	mov	si,bx			; get array pointer for sprite under use
	mov	bx,0			; point to first array element

co1:
	cmp	active[bx],TRUE		; check if sprite is active
	jne	cloop			; ignore if not active
	cmp	si,bx			; check for collision with itself
	je	cloop			; leave check if it is
	call	overlap			; check for sprite overlap
	jnc	cloop1			; jump to second check if no overlap
	mov	col_flag[bx],TRUE	; flag sprite as overlapping
	call	collision		; recursive call

cloop1:	add	sp_x[bx],320		; check alternate position
	sub	sp_y[bx],2
	call	overlap
	jnc	rsm1
	mov	col_flag[bx],TRUE	; flag sprite as overlapping
	call	collision		; recursive call

rsm1:	sub	sp_x[bx],640		; check second alternate position variables
	add	sp_y[bx],4
	call	overlap
	jnc	rsm3
	mov	col_flag[bx],TRUE	; flag sprite as overlapping
	call	collision		; recursive call

rsm3:	add	sp_x[bx],320		; correct values
	sub	sp_y[bx],2

cloop:
	add	bx,2			; point to next array element
	cmp	bx,32			; gone through 16 sprites?
	jne	co1			; jump if not

col_end:
	pop	si
	pop	bx
	ret

collision	endp



; ----------  Sprite Overlap Check  ----------
;
; Description:
;
;	This routine will check to see if a sprite whos number is in dx,
;	is overlapping the present sprite under use.
;
; Entry:
;
;	bx = pointer to test sprite arrays
;	si = pointer to sprite under use arrays
;
; Exit:
;
;	cy = TRUE  - sprite overlap
;	cy = FALSE - no overlap
;
;	bx, dx, si are preserved


overlap	proc	near

; -----  check x coordinates

	mov	ax,sp_x[bx]		; get test sprite x coordinate
	cmp	ax,sp_x[si]		; compare against sprite under use
	jl	ov2			; jump if test sprite left of sprite

; -----  test sprite right of sprite under use

ov1:
	mov	ax,x_width[si]		; get x width to x coordinate
	shl	ax,1
	shl	ax,1			; multiply width by 4 (4 pixels per byte)
	add	ax,sp_x[si]		; add x coordinate of sprite
	dec	ax			; decrement for proper value
	cmp	sp_x[bx],ax		; compare x adjusted coordinates
	jle	ov3			; jump if match so far
	jmp	nomatch

; -----  test sprite left of sprite under use

ov2:
	mov	ax,x_width[bx]		; get x width of test sprite
	shl	ax,1
	shl	ax,1			; multiply width by 4 (4 pixels per byte)
	add	ax,sp_x[bx]		; add x coordinate of test sprite
	dec	ax			; decrement for proper value
	cmp	ax,sp_x[si]		; compare adjusted x coordinates
	jge	ov3			; jump if match
	jmp	nomatch

; -----  compare y coordinate

ov3:
	mov	ax,sp_y[bx]		; get test sprite y coordinate
	cmp	ax,sp_y[si]		; compare against sprite under use
	jl	ov5			; jump if test sprite above sprite

; -----  test sprite below sprite under use

ov4:
	mov	ax,y_width[si]		; get y width of sprite
	add	ax,sp_y[si]		; add y coordinate
	dec	ax			; decrement for proper value
	cmp	sp_y[bx],ax		; compare adjusted coordinates
	jle	match			; jump if matched
	jmp	nomatch

; -----  test sprite above sprite under use

ov5:
	mov	ax,y_width[bx]		; get y width of test sprite
	add	ax,sp_y[bx]		; add y coordinate
	dec	ax			; decrement for proper value
	cmp	ax,sp_y[si]		; compare adjusted y values
	jge	match			; jump if there is a match
	jmp	nomatch

; -----  match not found

nomatch:
	or	al,al			; clear carry
	ret

; -----  match found

match:
	stc				; set carry
	ret

overlap	endp




; ----------  Return Collision Information  ----------
;
; Description:
;
;	This routine will check to see if a sprite pointed to by bx
;	is overlapping the present sprite under use pointed to by si
;	after the trim factors have been taken into account for.
;
; Entry:
;
;	bx = pointer to test sprite arrays
;	si = pointer to sprite under use arrays
;
; Exit:
;
;	cy = TRUE  - sprite overlap
;	cy = FALSE - no overlap
;
;	bx, dx, si are preserved


ret_col	proc	near

; -----  check x coordinates

	mov	ax,sp_x[bx]		; get test sprite x coordinate
	cmp	ax,sp_x[si]		; compare against sprite under use
	jl	rc2			; jump if test sprite left of sprite

; -----  test sprite right of sprite under use

rc1:
	mov	ax,x_width[si]		; get x width to x coordinate
	shl	ax,1
	shl	ax,1			; multiply width by 4 (4 pixels per byte)
	add	ax,sp_x[si]		; add x coordinate of sprite
	sub	ax,x_trim[si]		; subtract x trim factor
	dec	ax			; decrement for proper value
	mov	cx,sp_x[bx]		; get test sprite x coordinate
	add	cx,x_trim[bx]		; add x trim factor
	cmp	cx,ax			; compare adjusted x coordinates
	jle	rc3			; jump if match so far
	jmp	rnomatch

; -----  test sprite left of sprite under use

rc2:
	mov	ax,x_width[bx]		; get x width of test sprite
	shl	ax,1
	shl	ax,1			; multiply width by 4 (4 pixels per byte)
	add	ax,sp_x[bx]		; add x coordinate of test sprite
	sub	ax,x_trim[bx]		; subtract x trim factor
	dec	ax			; decrement for proper value
	mov	cx,sp_x[si]		; get sprite x coordinate
	add	cx,x_trim[si]		; add x trim factor
	cmp	ax,cx			; compare adjusted x coordinates
	jge	rc3			; jump if match
	jmp	rnomatch

; -----  compare y coordinate

rc3:
	mov	ax,sp_y[bx]		; get test sprite y coordinate
	cmp	ax,sp_y[si]		; compare against sprite under use
	jl	rc5			; jump if test sprite above sprite

; -----  test sprite below sprite under use

rc4:
	mov	ax,y_width[si]		; get y width of sprite
	add	ax,sp_y[si]		; add y coordinate
	sub	ax,y_trim[si]		; subtract y trim factor
	dec	ax			; decrement for proper value
	mov	cx,sp_y[bx]		; get test sprite y coordinate
	add	cx,y_trim[bx]		; add y trim factor
	cmp	cx,ax			; compare adjusted y coordinates
	jle	rmatch			; jump if matched
	jmp	rnomatch

; -----  test sprite above sprite under use

rc5:
	mov	ax,y_width[bx]		; get y width of test sprite
	add	ax,sp_y[bx]		; add y coordinate
	sub	ax,y_trim[bx]		; subtract y trim factor
	dec	ax			; decrement for proper value
	mov	cx,sp_y[si]		; get sprite y coordinate
	add	cx,y_trim[si]		; add y trim factor
	cmp	ax,cx			; compare adjusted y values
	jge	rmatch			; jump if there is a match
	jmp	rnomatch

; -----  match not found

rnomatch:
	or	al,al			; clear carry
	ret

; -----  match found

rmatch:
	stc				; set carry
	ret

ret_col	endp




; ----------  Remove Sprite  ----------
;
; Description:
;
;	The sprite specified will be removed by dumping the background
;	buffer for that sprite.
;
; Entry:
;
;	bx = pointer to sprite arrays


remove_sp	proc	near

	push	es
	push	si
	push	cx

	call	xfer_init		; get address of sprite top left corner in di
	mov	si,backgnd[bx]		; get address of background buffer
	mov	cx,y_width[bx]		; get y width as line counter

	assume	es:color_seg

	mov	ax,color_seg
	mov	es,ax			; set es to color card segment

re1:
	call	xfer_line		; dump line of buffer
	call	next_line		; get address of next line
	loop	re1			; loop until all lines are done

	pop	cx
	pop	si
	pop	es
	ret

	assume	es:dgroup

remove_sp	endp



; ----------  Put Image  ----------
;
; Description:
;
;	The image specified by the put command will be placed on the screen
;
; Entry:
;
;	bx = pointer to 17th element


put_image	proc	near

	push	es
	push	si
	push	cx

	call	xfer_init		; get address of sprite top left corner in di
	mov	si,image[bx]		; get address of image buffer
	mov	cx,y_width[bx]		; get y width as line counter

	assume	es:color_seg

	mov	ax,color_seg
	mov	es,ax			; set es to color card segment

put1:
	call	xfer_put_line		; dump line of buffer
	call	next_line		; get address of next line
	loop	put1			; loop until all lines are done

	pop	cx
	pop	si
	pop	es
	ret

	assume	es:dgroup

put_image	endp



; ----------  Sprite Tranfer Initialize Routine  ----------
;
; Description:
;
;	This routine will calculate the address of the top left pixel of
;	the specified sprite. Also it will set a flag signaling if the
;	present line is odd or even.
;
; Entry:
;
;	bx = pointer to sprite arrays
;
; Exit:
;
;	di = screen address of pixel



xfer_init	proc	near

	push	bx
	push	cx

	mov	cx,sp_y[bx]		; get y coordinate
	mov	ax,sp_x[bx]		; get x coordinate
	mov	bx,ax			; put in bx
	call	point_addr		; get address of byte in di
	test	cx,1			; check for odd line
	jz	xf2			; jump if even line
	mov	even_line,FALSE		; signal on odd line

	pop	cx
	pop	bx
	ret

xf2:
	mov	even_line,TRUE		; signal even line
	pop	cx
	pop	bx
	ret

xfer_init	endp




; ----------  Transfer Image Line  ----------
;
; Description:
;
;	This routine will transfer an screen image line from/to an
;	image buffer.
;
; Entry:
;
;	bx = pointer to sprite arrays
;	si = source buffer address
;	di = screen line starting address
;	ds = source segment
;	es = destination segment
;
; Exit:
;	si = address of next image byte
;	di = one address past last address used for transfer


xfer_line	proc	near

	assume	ds:nothing, es:nothing

	push	cx
	cld				; clear direction flag
	mov	cx,x_width[bx]		; get x width of image
	rep	movsb			; transfer byte image string
	pop	cx
	ret

	assume	ds:dgroup, es:dgroup

xfer_line	endp




; ----------  Transfer Sprite Image Line  ----------
;
; Description:
;
;	This routine will transfer an image line from the sprite image
;	array to the screen. It also uses the outline array to see if
;	a pixel location should be black or the background color.
;
; Entry:
;
;	bx = pointer to sprite arrays
;	si = pointer into image arrays
;	di = screen line starting addresss
;	es = screen segment


xfer_sp_line	proc	near

	assume	es:color_seg

	push	cx
	mov	cx,x_width[bx]		; use x width as row counter

xf1:
	mov	bp,outline[bx]		; get address of outline array
	mov	al,[bp+si]		; get outline byte
	and	al,es:[di]		; and outline with screen background
	mov	bp,image[bx]		; get address of image array
	or	al,[bp+si]		; or al with image byte
	mov	es:[di],al		; put byte on screen
	inc	si			; point to next element
	inc	di			; point to next screen address
	loop	xf1			; loop until row is done

	pop	cx
	ret

	assume	es:dgroup

xfer_sp_line	endp




; ----------  Transfer `Put' Image Line  ----------
;
; Description:
;
;	This routine will transfer an image line from the image array,
;	specified by the put command, to the screen.
;
; Entry:
;
;	bx = pointer to sprite array
;	si = pointer into image array
;	di = screen line starting addresss
;	es = screen segment


xfer_put_line	proc	near

	assume	es:color_seg

	push	cx
	mov	cx,x_width[bx]		; get x width of image

xpu1:
	mov	al,[si]			; get image byte
	and	al,color_mask		; and with color mask
	mov	es:[di],al		; transfer to screen
	inc	si			; point to next image byte
	inc	di			; point to next screen address
	loop	xpu1			; loop til done

	pop	cx
	ret

	assume	es:dgroup

xfer_put_line	endp




; ----------  Calculate Next Line Address  ----------
;
; Description:
;
;	This routine will calculate the address of the next line for another
;	image line transfer without using a multiply instruction.
;
; Entry:
;
;	bx = pointer to sprite arrays
;	di = one address past last address used for transfer
;	even_line = TRUE/FALSE
;
; Exit:
;
;	di = new line destination address


next_line	proc	near

	assume	ds:nothing, es:nothing

	sub	di,x_width[bx]		; subtract x width of image
	not	even_line		; switch status of even_line
	cmp	even_line,TRUE		; is new line even
	je	ne2			; jump if it is

; ----- new line is odd

	add	di,2000h		; add 8k offset to address
	ret

; ----- new line is even

ne2:
	sub	di,2000h-80		; subtract 8k offset from address
	ret

	assume	ds:dgroup, es:dgroup

next_line	endp




; ----------  Get New Background for Flagged Sprites  ----------
;
; Description:
;
;	This routine will update the background buffer for all flagged
;	sprites (flagged by the sp_blank command).


bkgnd_all	proc	near

	push	bx
	mov	bx,0		; point to first element

bka1:
	cmp	col_flag[bx],TRUE	; check if sprite flagged
	jne	bkaloop			; jump if not flagged
	call	get_backgnd		; get background

bkaloop:
	add	bx,2			; point to next element
	cmp	bx,32			; done all sprites
	jne	bka1			; jump if not

	pop	bx
	ret

bkgnd_all	endp




; ----------  Fill Background Buffer  ----------
;
; Description:
;
;	The specified sprite's background buffer will be filled with the
;	background that the sprite will be occupying.
;
; Entry:
;
;	bx = pointer to sprite arrays


get_backgnd	proc	near

	call	xfer_init		; get address of top left corner in di
	mov	si,di			; move address into si
	mov	di,backgnd[bx]		; get address of background buffer
	mov	cx,y_width[bx]		; get y width as line counter

	assume	ds:color_seg

	push	ds
	mov	ax,color_seg
	mov	ds,ax			; set ds to color card segment

ge1:
	call	xfer_line		; transfer line of image
	xchg	si,di			; exchange si with di
	call	next_line		; get address of next line
	xchg	si,di			; transfer new address back to si
	loop	ge1			; loop until all lines transfered

	pop	ds
	ret

	assume	ds:dgroup

get_backgnd	endp




; ----------  Put Sprite Routine  ----------
;
; Description:
;
;	This routine will transfer a sprite image onto the screen from
;	the image array with complete background prioritization using
;	the outline array
;
; Entry:
;
;	bx = pointer to sprite arrays


put_sp	proc	near

	push	es
	push	bx
	push	cx
	push	si

	call	xfer_init		; get address of top left corner in di
	mov	si,0			; point to first array element
	mov	cx,y_width[bx]		; get y width as line counter

	assume	es:color_seg

	mov	ax,color_seg
	mov	es,ax			; set es to color card segment

pu1:
	call	xfer_sp_line		; transfer sprite line
	call	next_line		; get address of next line
	loop	pu1			; loop until all line transfered

	pop	si
	pop	cx
	pop	bx
	pop	es
	ret

	assume	es:dgroup

put_sp	endp




; ----------  Replace Erased Sprites  ----------
;
; Description:
;
;	The col_flag table will be scanned to see which sprites are to
;	be placed on the screen.


replace_all	proc	near

	push	bx
	mov	cx,16			; check all 16 entries
	mov	si,table_addr		; address of priority table

repl1:
	mov	bx,[si]			; get sprite number
	shl	bx,1			; convert to pointer
	cmp	col_flag[bx],TRUE	; check if sprite flagged
	jne	reploop			; jump if not
	call	put_sp			; call put sprite routine

reploop:
	add	si,2			; point to next sprite number
	loop	repl1			; loop through all sprites

	pop	bx
	ret

replace_all	endp




; ----------  Removed Flagged Sprites  ----------
;
; Description:
;
;	The col_flag table will be scanned to see which sprites are to
;	be erased.


remove_all	proc	near

	push	bx
	mov	cx,16			; check all 16 entries
	mov	bx,0			; point to first array element

rep1:
	cmp	col_flag[bx],TRUE	; check if sprite flagged
	jne	reloop			; jump if not
	call	remove_sp		; remove sprite

reloop:
	add	bx,2			; point to next element
	loop	rep1			; loop through all sprites

	pop	bx
	ret

remove_all	endp



; ----------  Return Collision Information  ----------
;
; Description:
;
;	This routine will set bits 0 through 15 to one if a collision
;	has taken place with any sprite(s) 0 through 15.
;
; Entry:
;
;	bx = pointer to sprite array


col_detect	proc	near

	mov	si,bx			; get sprite under use in si
	mov	dx,0			; sprite number 0
	mov	bx,0			; point to first array element
	mov	ax,0			; clear collision register

ret1:
	cmp	active[bx],TRUE		; is sprite active
	jne	retl1			; jump if not active
	cmp	si,bx			; is sprite checking itself
	je	retl1			; jump if it is
	push	ax
	call	ret_col			; check for collision
	pop	ax
	jnc	retloop			; jump if not
	mov	di,1			; set bit 0
	mov	cl,dl			; put sprite number in cl
	shl	di,cl			; shift bit `sprite' number of times
	or	ax,di			; set bit in ax

retloop:
	add	sp_x[bx],320		; check alternate locations
	sub	sp_y[bx],2
	push	ax
	call	ret_col
	pop	ax
	jnc	rsm2
	mov	di,1			; set bit 0
	mov	cl,dl			; put sprite number in cl
	shl	di,cl			; shift bit `sprite' number of times
	or	ax,di			; set bit in ax

rsm2:	sub	sp_x[bx],640
	add	sp_y[bx],4
	push	ax
	call	ret_col
	pop	ax
	jnc	rsm4
	mov	di,1			; set bit 0
	mov	cl,dl			; put sprite number in cl
	shl	di,cl			; shift bit `sprite' number of times
	or	ax,di			; set bit in ax

rsm4:	add	sp_x[bx],320		; restore values
	sub	sp_y[bx],2

retl1:	add	bx,2			; point to next element
	inc	dx			; next sprite number
	cmp	dx,16			; done looping?
	jne	ret1			; jump if not

	ret

col_detect	endp


prog		ends
		end
