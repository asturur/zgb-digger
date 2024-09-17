	page	,132


;	Disk buffer read/write routines for STYX
;
;	Video write character routines are also included
;
;	Protection check removed 11/20/83 to be compatible with other systems
;



	title	>                Copy Protect Check               <
	subttl	>       Copyright(C) 1983  Windmill Software      <


; ----- system data area

dgroup		group	data
data		segment	word public 'data'
		assume	ds:dgroup
		public	diskdata

diskdata	db	513 dup(?)		; disk read sector storage
count		dw	?

data		ends


; ----- start of program

pgroup	group	prog
prog	segment	byte public 'prog'
	assume cs:pgroup

	public	prochk, getbuf, savbuf, ttyrst, ttywrt




; -------- Video TTY write routine -----------------------------
;
;	discription:
;			This routine writes one character to the vidio display
;			using bios interupt 10h.
;
;	format:
;			ttywrt(char,color);
;
;
ttywrt	proc	near

	push	bp
	mov	bp,sp

	mov	ax,[bp]+4	; get character
	mov	ah,14		; tty write code
	mov	bx,[bp]+6	; get forground color
	mov	bh,0
	int	10h		; do video IO
	
	pop	bp
	ret

ttywrt	endp


; -------- Video TTY initialize routine -----------------------------
;
;	discription:
;			This routine sets the cursor to position row,column.
;
;	format:
;			ttyrst(row, column);
;
;
ttyrst	proc	near

	push	bp
	mov	bp,sp

	mov	bx,0
	mov	ax,[bp]+4	; get row
	mov	dh,al
	mov	ax,[bp]+6	; get column
	mov	dl,al
	mov	ah,2		; set cursor code
	int	10h		; do video IO
	
	pop	bp
	ret

ttyrst	endp




prochk	proc	near

	push	bp
	push	ss
	push	es

	mov	ax,69			; return true for ok
	pop	es
	pop	ss
	pop	bp
	ret

prochk	endp


; ======================= Get disk buffer for high score =================
;
;	gets track 39, sector 1, of drive 0.
;	data is read into diskdata which is defined as an external string in C.
;


getbuf	proc	near

	push	bp
	push	ss
	push	es
	mov	count,3			; retry count

getlop:	mov	ax,ds
	mov	es,ax			; set ES
	mov	ah,2			; read sector
	mov	dl,0			; drive
	mov	dh,0			; head 0
	mov	ch,39			; track
	mov	cl,1			; sector
	mov	al,1			; one sector
	mov	bx,offset dgroup:diskdata

	int	13h			; disk i/o
	
	jae	getext			; operation sucessfull

	dec	count
	jnz	getlop			; retry 3 times

getext:	pop	es
	pop	ss
	pop	bp
	ret


getbuf	endp


; ======================= Save disk buffer for high score =================
;
;	saves track 39, sector 1, of drive 0.
;	data is written from diskdata which is defined as an external string in C.
;

savbuf	proc	near

	push	bp
	push	ss
	push	es
	mov	count,3			; retry count

; ---------- Verify a sector to start the motor then write the sector

savlop:	mov	ax,ds
	mov	es,ax			; set ES
	mov	ah,4			; verify sector
	mov	dl,0			; drive
	mov	dh,0			; head 0
	mov	ch,39			; track
	mov	cl,1			; sector
	mov	al,1			; one sector
	int	13h			; disk i/o

	mov	ax,ds
	mov	es,ax			; set ES
	mov	ah,3			; write sector
	mov	dl,0			; drive
	mov	dh,0			; head 0
	mov	ch,39			; track
	mov	cl,1			; sector
	mov	al,1			; one sector
	mov	bx,offset dgroup:diskdata

	int	13h			; disk i/o
	
	jae	savext			; operation sucessfull

	dec	count
	jnz	savlop			; retry 3 times

savext:	pop	es
	pop	ss
	pop	bp
	ret


savbuf	endp



prog	ends

	end
