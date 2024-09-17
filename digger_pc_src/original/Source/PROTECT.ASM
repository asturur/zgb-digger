	page	,132

	comment	*

===============================================================================

                   Copy Protection - Sector Destroy Routine
                  ------------------------------------------

                  Copyright(C) 1983   Windmill Software Inc.

Description:

	This routine will write an identification byte to a specified sector
	and cause a crc error to occur in the data field only. To verify that
	the sector has been destroyed, the program to be protected must read
	the sector, check the ID byte, and make sure that there was a data crc
	error during the read.

Program Procedure:

	Step 1:
		The program will read a sector with a sector number one less
		than the sector that is to be destroyed.

		i.e.  if sector 2 is to be destroyed, sector 1 is read.

		This will allow the program to synchronize it self with the
		rotation of the disk. If the program cannot read the sector
		sucessfully, a disk format error will be displayed and the
		program will then terminate.

	Step 2:
		The program will then change the user tick routine (INT 1CH)
		to point to a interrupt procedure contained in this program.
		Then the tick rate is increased to interrupt every 2 ms.

	Step 3:
		The program will then proceed to write data to the sector to
		be destroyed. The first byte written will be the program ID
		number. While the sector is being written, every tick interrupt
		is causing a tick counter to be incremented. On the tick count
		defined by NUM_TICK, a hard reset is performed on the FDC chip
		causing the sector write operation to be aborted partially way
		through. However, the write operation was aborted after the
		program ID number was written and before the new data CRC byte
		was written. Now the sector has a proper program ID number but
		cannot be read without causing a data CRC error. This should
		prohibit any de-protection diskcopy programs from operating
		since none can write a sector with a crc error like this
		can.

	Step 4:
		Since the write operation was interrupted by a hard reset,
		the FDC chip must be re-initialized. This is done by calling
		the disk i/o bios routine fucntion RESET. Also, the tick
		interrupt rate is returned to its normal 55 ms, and the
		interrupt vector is also restored to its previous address.

	Step 5:
		The sector destroyed is read to verify that the program ID
		number was written and the only a CRC data error occurs
		during a read. If any of the above conditions are not true,
		the program will start again at step 1 up to 10 times until
		the conditions are met. After 10 tries an error will be
		displayed.
*




; =============================================================================
;
;	<<<<<<<<<<  System Equates  >>>>>>>>>>
;


; ----- custom changable parameters

PROG_ID		equ	0eh		; program ID number
BAD_SECT	equ	2		; sector to destroy (sector 2 - 8)
TRK_NUM		equ	39		; track that sector is on (track 0 - 39)
DRIVE		equ	1		; drive number - kill sector MUST also be changed for other drive numbers
;						       - change status register tests also

; **********  Don't forget to change sign on message (signon)


; ----- system constants - DO NOT CHANGE

FDC_OUT		equ	3f2h		; FDC control/reset port
SYNC_SECT	equ	BAD_SECT-1	; sector read to synchronize program
NUM_TICK	equ	8		; number of ticks before destroy

BEEP		equ	7		; console beep sound
CR		equ	13		; carrige return
LF		equ	10		; line feed


	title	>          Sector CRC Destroy Routine   ver 1.0          <
	subttl	>       Copyright(C) 1983   Windmill Software Inc.       <




; =============================================================================
;
;	<<<<<<<<<<  Group Definitions  >>>>>>>>>>


pgroup	group	base, prog
dgroup	group	data

; The following segment serves only to force "pgroup" lower in
; memory.

base	segment	'program'
base	ends




; =============================================================================
;
;	<<<<<<<<<<  Interrupt Data Storage Definition  >>>>>>>>>>
;
; 		- segement definition for accessing the interrupt
;		  vector table


int_vect	segment at 0

		org	70h

int_addr	dw	?		; address of user tick
int_seg		dw	?		; segment of user tick


int_vect	ends




; =============================================================================
;
;	<<<<<<<<<<  Bios Data Storage Definition  >>>>>>>>>>
;
; 		- segement definition for retrieval of
;		  information from bios data area


bios_data	segment at 40h

		org	42h

st_0		db	?		; status register 0 storage location
st_1		db	?		; status register 1 storage location
st_2		db	?		; status register 2 storage location

bios_data	ends




; =============================================================================
;
;	<<<<<<<<<<  Program Storage Area  >>>>>>>>>>
;
;	- all read/write operations will use this area for
;	  sector information storage


data	segment		'data'

old_addr	dw	?		; old user tick address
old_seg		dw	?		; old user tick segment
err_count	db	?		; error count
loop_count	db	?		; loop retry count

read_data	db 	512 dup(?)	; storage area for read/write sector

write_data	db 	PROG_ID		; program ID number
		db	511 dup(0f6h)	; storage area for read/write sector


; ----- System Messages

errmes_1	db	5 dup(BEEP)
		db	5 dup(CR,LF)
		db	'>>>>>>>>>>  Disk Not Formatted or Disk Formatting error  <<<<<<<<<<'
		db	5 dup(CR,LF)
		db	'$'

errmes_3	db	5 dup(BEEP)
		db	5 dup(CR,LF)
		db	'>>>>>>>>>>  PROGRAM NOT PROTECTED  <<<<<<<<<<'
		db	5 dup(CR,LF)
		db	'$'

errmes_4	db	2 dup(CR,LF)
		db	'----- Operation Sucessful -----'
		db	2 dup(CR,LF)
		db	'$'

signon		db	2 dup(CR,LF)
		db	'Copy Protect Generator for DIGGER'
		db	2 dup(CR,LF)
		db	'$'

;
;****************************************
;
;	Data buffer for Track 39 Format
;
;****************************************
;
;

FDATA	DB	10,0,1,3,10,0,2,3,10,0,3,3
	DB	10,0,4,3,10,0,5,3,10,0,6,3
	DB	10,0,7,3,10,0,8,3,10,0,9,3



data	ends




; =============================================================================
;
;	<<<<<<<<<<  Program Segment Definition  >>>>>>>>>>
;
;	- start of main program


prog	segment		'program'
	assume cs:pgroup, ds:dgroup, es:nothing

	org	100h

destroy	proc	far

; ----- calculate the value for DS

	mov	bx,offset pgroup:prog_end	; get last program address
	mov	cl,4
	shr	bx,cl			; ignore 4 lsb
	inc	bx			; increment address
	mov	ax,cs			; get code segment
	add	ax,bx
	mov	ds,ax			; DS now points to dgroup

	mov	dx,offset dgroup:signon
	call	write_text		; sign on message
	mov	loop_count,0		; clear loop count

; ----- start main program execution

t1:	call	find_sect		; find first sector needed
	jnc	t2			; jump if no error exists
	int	20h			; else return
t2:	call	set_intrpt		; initialize interrupt routine
	call	dest_sect		; start destroy procedure
	call	reset_fdc		; reset FDC
	call	verify			; verify sector has a data crc error
	jnc	done			; jump if sucessful
	inc	loop_count
	cmp	loop_count,10
	jne	t1			; retry in still allowed

	mov	dx,offset dgroup:errmes_3
	call	write_text		; print error message
	int	20h

done:
	mov	dx,offset dgroup:errmes_4
	call	write_text

;
;**************************************
;
;	Re-format track 38
;	
;**************************************
;
;	To format track 38 to 1024 byte sectors
;		AH=5	- Command code
;		DL=1	- Drive number
;		DH=0	- Head number
;		CH=38	- Track number
;		CL=1	- Sector number
;		AL=4	- Number of sectors
;		ES:BX	- Address of buffer
;
;	Buffer format:
;		ES:BX points to a collection of data
;		for sector headers.  Each field
;		contains 4 bytes ( C,H,R,N )
;		C= Track number, H= Head number
;		R= Sector number, N= Number of
;		bytes per sector ( 0=128, 1=256
;		2=512, 3=1024 )
;
	MOV	CX,DS	;	Get data segment into ES
	MOV	ES,CX
	MOV	AH,5	;	Set up command code
	MOV	DL,1	;	Set up peramaters, Drive number
	MOV	DH,0	;	Head number
	MOV	CH,38	;	Track number
	MOV	CL,1	;	Sector number
	MOV	AL,1	;	Number of sectors ( LESS THAN 0 )

	MOV	BX,OFFSET DGROUP:FDATA	; BX=Address of data buffer

	INT	13H	;	Do format


	int	20h			; terminate program

destroy	endp




; ----- code segment data storage for tick trap

cs_data		proc	near

tick_count	db	0		; tick trap counter

cs_data		endp



; -----------------------------------------------------------------------------
;
;	<<<<<<<<<<  Find Synchronization Sector  >>>>>>>>>>
;
; Description:
;
;	This routine will read the sector defined by FIND_SECT
;	to synchronize the program with the revolment of the
;	disk.


find_sect	proc	near

	mov	err_count,0		; clear error count


find:
	mov	cl,SYNC_SECT		; sector number
	mov	ch,TRK_NUM		; track number

	call	read_sec		; read sector
	jnc	found			; jump if no error

	inc	err_count
	cmp	err_count,5		; have there been 5 retries
	jne	find			; jump if not

find_error:
	mov	dx,offset dgroup:errmes_1
	call	write_text
	stc				; signal error

found:
	ret

find_sect	endp




; -----------------------------------------------------------------------------
;
;	<<<<<<<<<<  Set Interrupt  >>>>>>>>>>
;
; Description:
;
;	This routine will set the user tick routine
;	to point to the interrupt routine in this module.
;	It will then set the interrupt routine to trigger
;	the destroy sector routine once the write routine
;	has started.
;
; On Exit:
;
;	AX destroyed


set_intrpt	proc	near
		assume es:int_vect

	mov	ax,int_vect
	mov	es,ax

	mov	tick_count,0			; clear tick count

	mov	ax,int_addr
	mov	old_addr,ax			; store old tick address
	mov	ax,int_seg
	mov	old_seg,ax			; store old interrupt address
	mov	dx,43h				; address of tick timer

	cli					; disable interrupts
	mov	ax,offset pgroup:tick_trap
	mov	int_addr,ax			; store new interrupt address
	mov	ax,cs
	mov	int_seg,ax			; store new interrupt segment
	mov	al,36h				; set timer mode
	out	dx,al
	mov	dx,40h
	mov	al,51h
	out	dx,al
	mov	al,9h
	out	dx,al				; speed tick interrupt
	sti					; enable interrupts

	ret

	assume	es:nothing

set_intrpt	endp




; -----------------------------------------------------------------------------
;
;	<<<<<<<<<<  Destroy Sector  >>>>>>>>>>
;
; Description:
;
;	This routine will start a write operation on 
;	BAD_SECT and it should be interrupted half way
;	through the routine by tick trap. This will
;	cause the sector to have a good ID crc but will
;	generate a bad data crc.
;
; On Exit:
;
;	This routine should not exit by the completeion
;	of the write since it should be interrupted. If
;	it does, however, an error message will be displayed.
	

dest_sect	proc	near

	mov	cl,BAD_SECT
	mov	ch,TRK_NUM
	call	write_sec			; write sector
	ret

dest_sect	endp




; -----------------------------------------------------------------------------
;
;	<<<<<<<<<<  Reset FDC  >>>>>>>>>>
;
; Description:
;
;	This routine will restore the old user tick
;	vector and reset the FDC since it was
;	subjected to a hardware reset to abort a
;	write operation.


reset_fdc	proc	near
		assume	es:int_vect

; ----- restore old tick vector

	mov	ax,int_vect
	mov	es,ax			; set ES to vector table segment
	mov	dx,40h

	cli				; disable interrupts
	mov	ax,old_addr		; get old address
	mov	int_addr,ax		; store it
	mov	ax,old_seg		; get old segment
	mov	int_seg,ax		; store it
	mov	al,0
	out	dx,al
	out	dx,al			; restore tick speed
	sti				; enable interrupts

; ----- reset FDC

	mov	ah,0			; reset FDC function
	int	13h			; call bios routine
	ret

	assume	es:nothing

reset_fdc	endp




; -----------------------------------------------------------------------------
;
;	<<<<<<<<<<  Verify Bad Sector  >>>>>>>>>>
;
; Description:
;
;	This routine will read the sector just destroyed
;	to verify that the only error it has is a bad
;	data crc.


verify	proc	near

	mov	cl,BAD_SECT
	mov	ch,TRK_NUM
	call	read_sec		; read the destroyed sector
	jnc	verror			; jump if error detected

; ----- error was detected - make sure only data crc

	assume	es:bios_data

	mov	ax,bios_data
	mov	es,ax			; set ES to bios data segment

; ----- check status registers for proper errors

	cmp	st_0,41h		; check status reg 0 for abnorm term.
	jne	verror			; jump if no error
	cmp	st_1,20h		; check status reg 1 for crc error
	jne	verror			; jump if no crc error
	cmp	st_2,20h		; check status reg 2 for data crc error
	jne	verror			; jump if no data crc

; ----- check program ID

	cmp	read_data,PROG_ID
	jne	verror			; jump if program ID is incorrect
	

; ----- sector was properly destroyed

	or	al,al			; clear carry
	ret

; ----- sector was not properly destroyed

verror:
	stc				; indicate error
	ret

	assume	es:nothing

verify	endp




; -----------------------------------------------------------------------------
;
;		<<<<<  Tick Interrupt Trap Routine  >>>>>
;
; Description:
;
;	This routine is called every tick of the system clock.
;	Once the tick interrupt vector has been changed to point
;	to this routine, on the third tick, this routine will
;	perform a hardware reset to the FDC to abort the write
;	sector routine already in progress. This will cause a
;	data crc error to occur in that sector.


tick_trap	proc	far

	sti				; enable interrupts

	inc	tick_count		; increment tick counter
	cmp	tick_count,NUM_TICK	; check tick count
	je	kill_sect		; jump if ready to interrupt write

	iret				; return from interrupt

; ----- time to interrupt write operation

kill_sect:

	push	dx
	push	ax

	mov	dx,FDC_OUT		; address of control/reset port
	mov	al,21h			; 21=drive 2   10=drive 1
	out	dx,al			; send reset signal

	pop	ax
	pop	dx
	iret

tick_trap	endp




; -----------------------------------------------------------------------------
;
;		<<<<<  Read Sector  >>>>>
;
; Description:
;
;	This routine will read a specified sector into the data
;	area from drive A
;
; On Entry:
;
;	CL = sector number
;	CH = track number
;
; On Exit:
;
;	CY = false if read was sucessful
;	CY = true if error in reading
;
;	AL = data from status register 2 of FDC
;		bit	7: 0
;			6: control mark
;			5: data error	(set when sector has data error)
;			4: wrong cylinder
;			3: scan equal
;			2: scan not satisfied
;			1: bad cylinder
;			0: missing addr mark
;
;	BX, DX, ES are destroyed


read_sec	proc	near
		assume  es:dgroup

	mov	ax,ds
	mov	es,ax			; set ES to dgroup segment

	mov	ah,2			; write sector
	mov	dl,DRIVE
	mov	dh,0			; head 0
	mov	al,1			; one sector

	mov	bx,offset dgroup:read_data	; store in data area

	int	13h   			; disk i/o

errext:	ret

	assume	es:nothing

read_sec	endp




; -----------------------------------------------------------------------------
;
;		<<<<<  Write Sector  >>>>>
;
; Description:
;
;	This routine will write the data area to a specified sector
;	on drive A:
;
; On Entry:
;
;	CL = sector number
;	CH = track number
;
; On Exit:
;
;	CY = false if write was sucessful
;	CY = true if error in writing
;
;	BX, DX, ES are destroyed


write_sec	proc	near
		assume es:dgroup

	mov	ax,ds
	mov	es,ax		; set ES to dgroup segment

	mov	ah,3		; write sector
	mov	dl,DRIVE
	mov	dh,0		; head 0
	mov	al,1		; one sector

	mov	bx,offset dgroup:write_data	; write from data area

	int	13h			; disk i/o
	ret

	assume	es:nothing

write_sec	endp




; -----------------------------------------------------------------------------
;
;	<<<<<<<<<<  Write Text Routine  >>>>>>>>>>
;
; Description:
;
;	This rouine will print a message on the console
;
; On Entry:
;
;	DX = offset of message


write_text	proc	near

	push	ax
	mov	ah,9			; DOS write string function
	int	21h			; call DOS
	pop	ax

	ret

write_text	endp




; ----- define address of last byte in program


prog_end	label	byte


prog	ends

	end	destroy
