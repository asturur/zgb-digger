; =============================================================================
;
;		<<<<<<<<<<  Bad CRC Verify Program  >>>>>>>>>>
;
; Description:
;
;	This program will read the sector defined by TRACK and BAD_SECT
;	and check the program ID. It will the make sure that there was
;	only a crc error during the read. An error message is printed
;	if the above conditions are not true.



; ----- system equates

BAD_SECT	equ	2		; bad sector number
TRACK		equ	39		; track number
PROG_ID		equ	0eh		; program ID
DRIVE		equ	1		; drive


pgroup	group	base,prog
dgroup	group	data

base	segment	'code'			; dummy segment
base	ends


; ----- system data area

data	segment	memory

diskdata	db	512 dup(?)		; disk read sector storage

signon		db	13,10,13,10,'DIGGER Copy Protect Verify',13,10,'$'

errmes		db	7,7,7,7,7,
		db	13,10,13,10,13,10
		db	'>>>>>>>>>>  ERROR -- PROGRAM NOT PROTECTED  <<<<<<<<<<'
		db	13,10,13,10,'$'

ok		db	13,10,13,10
		db	'----------  Program Protected  ----------'
		db	13,10,13,10,'$'

data	ends


; ----- bios data definition

bios_data	segment at 40h

	org	42h

st_0	db	?		; status reg 0
st_1	db	?		; status reg 1
st_2	db	?		; status reg 2

bios_data	ends





; ----- start of program

prog	segment	'code'
	assume cs:pgroup, ds:dgroup

	org 100h

start	proc	far

	mov	bx,offset pgroup:prog_end
	mov	cl,4
	shr	bx,cl
	inc	bx
	mov	ax,cs
	add	ax,bx
	mov	ds,ax			; set DS

	mov	dx,offset dgroup:signon
	mov	ah,9
	int	21h			; print message

	call	check			; check sector to start drive
	call	check			; check sector again
	jc	error			; jump if error
	call	check			; check sector again
	jc	error			; jump if error
	call	check			; check sector
	jc	error			; jump if error again

	mov	dx,offset dgroup:ok
	mov	ah,9
	int	21h			; print ok message
	int	20h			; exit

error:
	mov	dx,offset dgroup:errmes
	mov	ah,9
	int	21h			; print error message
	int	20h			; exit

start	endp



; ----- read sector, check program ID and CRC error

check	proc	near

	call	read_sect		; read sector
	jnc	cerror			; jump if no error

	cmp	diskdata,PROG_ID	; check program ID
	jne	cerror			; jump if not correct

	assume	es:bios_data

	mov	ax,bios_data
	mov	es,ax			; set es

	cmp	st_0,41h		; check status reg 0
	jne	cerror
	cmp	st_1,20h		; check status reg 1
	jne	cerror
	cmp	st_2,20h		; check status reg 2
	jne	cerror

	assume	es:nothing

	or	al,al			; clear carry
	ret

cerror:
	stc				; indicate error
	ret

check	endp


; ----- read sector routine

read_sect	proc	near

	mov	ax,ds
	mov	es,ax			; set ES
	mov	ah,2			; read sector
	mov	dl,DRIVE		; drive
	mov	dh,0			; head 0
	mov	ch,TRACK		; track
	mov	cl,BAD_SECT		; sector
	mov	al,1			; one sector
	mov	bx,offset dgroup:diskdata

	int	13h			; disk i/o
	ret

read_sect	endp


prog_end	label	byte		; last address


prog	ends

	end	start
