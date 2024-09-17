;
;
;  Generate medium res title page from run length encoded data array
;
;  Version 1.0		9-FEB-83	W.J. Montgomery
;
;
;-------------------------------------------------------
;
;
colseg	segment at 0b800h

colseg	ends
;
;---------------------------------
;
dgroup	group	data
data	segment	word public 'data'
	assume 	ds:dgroup

	include b:title.asm

data	ends
;
;
;
;
;---------------------------------
;
pgroup	group	prog
prog	segment	byte public 'prog'
	public gentitle
	assume cs:pgroup,ds:dgroup,es:colseg


;
; run length decode to screen
;
; rlesize contains number of bytes of encoded data in array
;
; rledata is the address of array[0]
;
;
gentitle proc	near
	push	es
	push	bp
	mov	ax,0b800h	;point to screen start
	mov	es,ax
	mov	di,0		;offset into screen
	mov	si,offset dgroup:rledata ;source of data

loop:	mov	al,[si]		;pickup byte
	cmp	al,0feh		;check for rle key flag
	jz	rle		;is run lenghth coded

notrle:	mov	es:[di],al	;store data to screen
	inc	di
	inc	si
	cmp	di,4000h	;end of screen?
	jnz	loop
	jmp	exit		;done

rle:	inc	si		;point to number of bytes
	mov	bl,[si]	
	inc	si
	mov	al,[si]		;pickup data byte
	inc	si
again:	mov	es:[di],al	;write to screen
	inc	di		;bump screen pointer
	cmp	di,4000h	;end of screen?
	jz	exit
	dec	bl		;count down character counter
	jnz	again
	jmp	loop

exit:	pop	bp
	pop	es
	ret
gentitle endp

prog	ends
	end

