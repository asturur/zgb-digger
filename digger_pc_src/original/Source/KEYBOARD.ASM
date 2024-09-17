	PAGE	,132

	TITLE	  >	Keyboard / Joy Stick  Routines

	COMMENT	$

===============================================================================

                      Keyboard / Joystick Routines
                    ---------------------------------

Description:
	
	The entry points to these routines were designed to link with modules
	created with the Lattice C Compiler and are called as external C
	procedures.

	This version supports the Hyperion and the I.B.M. keyboard.


Routines Supported:

	chkjoy()	- sets varibles JOY_X, JOY_Y to the x,y position of
			  the joystick.

	kb_init()	- changes the keyboard interrupt vector to point to
			  the keyboard handler contined in this program.

	kb_rst()	- restores the original keyboard interrupt vector.

	peek(addr)	- read from the specified address.

	poke(addr,byte)	- write to the specified address.

=============================================================================== $
			  
	PAGE


; Computer type selection

IBM	EQU	0	; set up for IBM PC if equal to 1
HYP	EQU	1	; set up for Hyperion if equal to 1


; -----------------------------------------------
;
;	<<<<<  Constant Declarations  >>>>>
;
; -----------------------------------------------


CRTADD	EQU	3D4H		; address port of 6845 CRT controller
CRTDAT	EQU	3D5H		; data port of 6845

	if HYP
KB_DATA	EQU	0E0H		; keyboard data port
KB_CNTL	EQU	0E0H		; keyboard control port
	endif
	if IBM
KB_DATA	EQU	60H		; keyboard data port
KB_CNTL	EQU	61H		; keyboard control port
	endif

FALSE		EQU	0
TRUE		EQU	-1
STOP		EQU	0			; stopped movement
LEFT		EQU	1			; left movement
RIGHT		EQU	2			; right movement

DIRR		EQU	0			; Right movement
DIRUR		EQU	1			; Up and right movement
DIRU		EQU	2			; Up
DIRUL		EQU	3			; Up and left
DIRL		EQU	4			; Left
DIRDL		EQU	5			; Down and left
DIRD		EQU	6			; Down
DIRDR		EQU	7			; Down and right
DIRNONE		EQU	-1			; Nothing pressed


	if IBM
L_ARROW_DOWN	EQU	75			; left arrow down
L_ARROW_UP	EQU	L_ARROW_DOWN OR 128     ; left arrow up
R_ARROW_DOWN	EQU	77			; right arrow down
R_ARROW_UP	EQU	R_ARROW_DOWN OR 128	; right arrow key up
U_ARROW_DOWN	EQU	72			; up arrow
U_ARROW_UP	EQU	U_ARROW_DOWN OR 128
D_ARROW_DOWN	EQU	80			; down arrow
D_ARROW_UP	EQU	D_ARROW_DOWN OR 128
UR_ARROW_DOWN	EQU	73			; up & right (9)
UR_ARROW_UP	EQU	UR_ARROW_DOWN OR 128
UL_ARROW_DOWN	EQU	71			; up & left (7)
UL_ARROW_UP	EQU	UL_ARROW_DOWN OR 128
DR_ARROW_DOWN	EQU	81			; down & right (3)
DR_ARROW_UP	EQU	DR_ARROW_DOWN OR 128
DL_ARROW_DOWN	EQU	79			; down & left (1)
DL_ARROW_UP	EQU	DL_ARROW_DOWN OR 128

F1_DOWN		EQU	59			; F1 key down
F1_UP		EQU	F1_DOWN OR 128		; F1 key up
CNTL_DOWN	EQU	29			; control key down
CNTL_UP		EQU	CNTL_DOWN OR 128	; control key up
ALT_DOWN	EQU	56			; alternate key down
ALT_UP		EQU	ALT_DOWN OR 128		; alternate key up
DEL_DOWN	EQU	83			; delete key down
DEL_UP		EQU	DEL_DOWN OR 128		; delete key up
F9_DOWN		EQU	67			; F9 down
F9_UP		EQU	F9_DOWN OR 128		; F9 up
F7_DOWN		EQU	65			; F7 down
F7_UP		EQU	F7_DOWN OR 128		; F7 up

ESC_DOWN	EQU	1			; Esc key down
	endif

	if HYP
L_ARROW_DOWN	EQU	82			; left arrow down
L_ARROW_UP	EQU	L_ARROW_DOWN OR 128     ; left arrow up
R_ARROW_DOWN	EQU	84			; right arrow down
R_ARROW_UP	EQU	R_ARROW_DOWN OR 128	; right arrow key up
U_ARROW_DOWN	EQU	79			; up arrow
U_ARROW_UP	EQU	U_ARROW_DOWN OR 128
D_ARROW_DOWN	EQU	87			; down arrow
D_ARROW_UP	EQU	D_ARROW_DOWN OR 128
UR_ARROW_DOWN	EQU	80			; up & right (9)
UR_ARROW_UP	EQU	UR_ARROW_DOWN OR 128
UL_ARROW_DOWN	EQU	78			; up & left (7)
UL_ARROW_UP	EQU	UL_ARROW_DOWN OR 128
DR_ARROW_DOWN	EQU	88			; down & right (3)
DR_ARROW_UP	EQU	DR_ARROW_DOWN OR 128
DL_ARROW_DOWN	EQU	86			; down & left (1)
DL_ARROW_UP	EQU	DL_ARROW_DOWN OR 128

F1_DOWN		EQU	01			; F1 key down
F1_UP		EQU	F1_DOWN OR 128		; F1 key up
CNTL_DOWN	EQU	44			; control key down
CNTL_UP		EQU	CNTL_DOWN OR 128	; control key up
ALT_DOWN	EQU	59			; alternate key down
ALT_UP		EQU	ALT_DOWN OR 128		; alternate key up
DEL_DOWN	EQU	92			; delete key down
DEL_UP		EQU	DEL_DOWN OR 128		; delete key up
F9_DOWN		EQU	10			; F9 down
F9_UP		EQU	F9_DOWN OR 128		; F9 up
F7_DOWN		EQU	08			; F7 down
F7_UP		EQU	F7_DOWN OR 128		; F7 up

ESC_DOWN	EQU	13			; Esc key down
	endif

SYNC_NORM	EQU	2CH			; normal sync position

	PAGE

; ==========  Alternate Segment Definitions  ==========

; -----  ROMSEG - rom bios segment

ROMSEG		SEGMENT AT 0F000H

		ASSUME	CS:ROMSEG

		ORG 0FFF0H

REBOOT		PROC	FAR		; Re-boot jump vector in ROM BIOS
REBOOT		ENDP

		ASSUME	CS:NOTHING

ROMSEG		ENDS


; ----- ROMDATA - rom bios data area

ROMDATA		SEGMENT	AT 40H

       		ORG	72H

RESET_FLAG	DW	?		; keyboard reset flag

	if HYP
		org	0e0h

SCREEN		db	?		; screen active flag for Hyperion
	endif


ROMDATA		ENDS

	PAGE

;--------------------------------------------
;
;	<<<<<  Data Storage Area  >>>>>
;
;--------------------------------------------


DGROUP	GROUP	DATA
DATA	SEGMENT	BYTE PUBLIC 'DATA'

	PUBLIC	KEYCHR, KEYDIR, LKDIR, PDIR, LDIR, FIRE, FIRES
	PUBLIC	ENDGAME, SOUND, MUSIC, JOYX, JOYY, BUTT1, BUTT2, DEFSEG


KB_ADR		DW	?		; keyboard routine address
KB_SEG		DW	?		; keyboard routine segment

L_KEY		DB	FALSE		; key press flags
R_KEY		DB	FALSE
U_KEY		DB	FALSE
D_KEY		DB	FALSE
UR_KEY		DB	FALSE
UL_KEY		DB	FALSE
DR_KEY		DB	FALSE
DL_KEY		DB	FALSE
RST1		DB	FALSE
RST2		DB	FALSE
RST3		DB	FALSE
SND_UP		DB	TRUE
MSC_UP		DB	TRUE

KEYCHR		DB	0		; scan code of key pressed
KEYDIR		DW	DIRNONE		; 8 direction variable
LKDIR		DW	DIRNONE		; last of 8 directions pressed
PDIR		DW	STOP		; 2 direction variable
LDIR		DW	STOP		; last of 2 directions pressed
FIRE		DW	FALSE		; fire key (F1)
FIRES		DW	FALSE		; fire key flag (TRUE = was pressed)
ENDGAME		DW	FALSE		; TRUE = escape key pressed
SOUND		DW	TRUE		; sound toggle
MUSIC		DW	TRUE		; music toggle

JOYX		DW	?		; joystick x coordinate
JOYY		DW	?		; joystick y coordinate
BUTT1		DW	FALSE		; button 1 flag
BUTT2		DW	FALSE		; button 2 flag

SYNC_POS	DB	SYNC_NORM	; current sync position
DEFSEG		DW	?		; data segment value for peek/poke


; ----- Keyboard Decode Table

KB_TABLE	LABEL	BYTE


	DB	L_ARROW_DOWN
	DW	OFFSET PGROUP:LEFT_DOWN
	DB	L_ARROW_UP
	DW	OFFSET PGROUP:LEFT_UP

	DB	R_ARROW_DOWN
	DW	OFFSET PGROUP:RIGHT_DOWN
	DB	R_ARROW_UP
	DW	OFFSET PGROUP:RIGHT_UP

	DB	U_ARROW_DOWN
	DW	OFFSET PGROUP:UP_DOWN
	DB	U_ARROW_UP
	DW	OFFSET PGROUP:UP_UP

	DB	D_ARROW_DOWN
	DW	OFFSET PGROUP:DOWN_DOWN
	DB	D_ARROW_UP
	DW	OFFSET PGROUP:DOWN_UP

; -----  blank out all 45 degree keys
;	DB	UR_ARROW_DOWN
;	DW	OFFSET PGROUP:UPR_DOWN
;	DB	UR_ARROW_UP
;	DW	OFFSET PGROUP:UPR_UP

;	DB	UL_ARROW_DOWN
;	DW	OFFSET PGROUP:UPL_DOWN
;	DB	UL_ARROW_UP
;	DW	OFFSET PGROUP:UPL_UP

;	DB	DR_ARROW_DOWN
;	DW	OFFSET PGROUP:DOWNR_DOWN
;	DB	DR_ARROW_UP
;	DW	OFFSET PGROUP:DOWNR_UP

;	DB	DL_ARROW_DOWN
;	DW	OFFSET PGROUP:DOWNL_DOWN
;	DB	DL_ARROW_UP
;	DW	OFFSET PGROUP:DOWNL_UP


	DB	F1_DOWN
	DW	OFFSET PGROUP:FIRE_DOWN
	DB	F1_UP
	DW	OFFSET PGROUP:FIRE_UP

	DB	CNTL_DOWN
	DW	OFFSET PGROUP:RESET1_DOWN
	DB	CNTL_UP
	DW	OFFSET PGROUP:RESET1_UP

	DB	ALT_DOWN
	DW	OFFSET PGROUP:RESET2_DOWN
	DB	ALT_UP
	DW	OFFSET PGROUP:RESET2_UP

	DB	DEL_DOWN
	DW	OFFSET PGROUP:RESET3_DOWN
	DB	DEL_UP
	DW	OFFSET PGROUP:RESET3_UP

	DB	F7_DOWN
	DW	OFFSET PGROUP:MUSIC_DOWN
	DB	F7_UP
	DW	OFFSET PGROUP:MUSIC_UP

	DB	F9_DOWN
	DW	OFFSET PGROUP:SOUND_DOWN
	DB	F9_UP
	DW	OFFSET PGROUP:SOUND_UP

	DB	ESC_DOWN
	DW	OFFSET PGROUP:EXIT_DOWN


NUM_CHR	EQU	($-KB_TABLE)/3			; calculate number of entries

DATA	ENDS

	PAGE

;---------------------------------------
;
;	<<<<<  Program Area  >>>>>
;
;---------------------------------------


PGROUP	GROUP	PROG
PROG	SEGMENT	BYTE PUBLIC 'PROG'

	PUBLIC	CHKJOY, KB_INIT, KB_RST, PEEK, POKE

	ASSUME	CS:PGROUP, DS:DGROUP, ES:DGROUP, SS:DGROUP


; -----  Variable used to store value of DS so interrupt handler can retrieve

CSDATA	PROC	NEAR

DATASEG	DW	?		; location to store value of data segment

CSDATA	ENDP

	
; ========== Joystick Read Routine ==========
;
; Calling format:
;
;       chkjoy()
;
; Description:
;
;       This module will read new values from the joystick adapter
;		JOYX = X value
;		JOYY = Y value
;		BUTT1 - Set to TRUE when button 1 is depressed
;		BUTT2 - Set to TRUE when button 2 is depressed
;


CHKJOY	PROC	NEAR

	PUSH	BP
	MOV	BP,SP		; point to passed parameters

	MOV	DX,201H		; Game adapter port number

; -----  read button status

	MOV	BUTT1,FALSE	; Reset button status to false
	MOV	BUTT2,FALSE
	IN	AL,DX		; Read buttons
	TEST	AL,10H		; Check button #1
	JNZ	NBUT1
	MOV	BUTT1,TRUE	; Button 1 is depressed
NBUT1:	TEST	AL,20H		; Check button #2
	JNZ	NBUT2
	MOV	BUTT2,TRUE	; Button 2 is depressed

; -----  read joystick values

NBUT2:	MOV	BX,0		; Reset X value
	MOV	CX,0		; Reset Y value
	MOV	AH,0		; Set AH for null addition

	OUT	DX,AL		; Start timer pulse
JOY1:	IN	AL,DX		; Get new status

	RCR	AL,1		; Get X status into carry
	ADC	BL,AH	
	RCR	AL,1		; Get Y status into carry
	ADC	CL,AH
	RCR	AL,1
	INC	CH		; Check for time out
	JNZ	JOY1
	
JOYDON:	MOV	CH,0		; Reset CH after use as a timer
	MOV	JOYX,BX		; Save X value
	MOV	JOYY,CX		; Save Y value

	POP	BP		; restore BP
	RET

CHKJOY	ENDP

	PAGE

; <<<<<<<<<<  Keyboard Interrupt Initialization  >>>>>>>>>>
;
; Command Format:
;
;       kb_init();
;
; Description:
;
;       This routine will change the keyboard interrupt vector to point to
;       a machine language routine located in this graphics library which
;       contains its own keyboard handeller.


KB_INIT	PROC	NEAR

	MOV	AX,DS
	MOV	DATASEG,AX		; store DS value in location DATASEG

	PUSH	DS

	MOV	AX,0
	MOV	DS,AX			; set DS to dos seg
	MOV	DI,24H			; location of keybrd interrupt
	MOV	AX,[DI]			; get old interrupt address
	MOV	BX,[DI]+2		; get old interrupt segment
	MOV	CX,OFFSET PGROUP:KEYINT	; get address of routine
	MOV	DX,CS			; get code segment

	CLI				; disable interrupts

	MOV	[DI],CX			; store address
	MOV	[DI+2],DX		; store code seg
                                        
	STI				; enable interrupts

	POP	DS

	MOV	KB_ADR,AX		; store interrupt address
	MOV	KB_SEG,BX		; store interrupt segment

	if HYP
	assume	ds:romdata
	push	ds
	mov	ax,40h
	mov	ds,ax
	mov	screen,0
	pop	ds
	assume	ds:dgroup
	endif

	RET

KB_INIT	ENDP



; <<<<<<<<<<  Keyboard Restore Routine  >>>>>>>>>>
;
; Calling Format:
;
;       kb_rst();
;
; Description:
;
;       This routine will restore the original keyboard interrupt address.


KB_RST	PROC	NEAR

	MOV	AX,KB_ADR		; get old interrupt address
	MOV	BX,KB_SEG		; get old interrupt segment
	MOV	DI,24H			; location of keyboard interrupt

	PUSH	DS			; save data segment
	MOV	CX,0
	MOV	DS,CX			; set DS to DOS segment

	CLI				; disable interrupts

	MOV	[DI],AX			; store address
	MOV	[DI+2],BX		; store segment

	STI				; enable interrupts

	POP	DS
	RET

KB_RST	ENDP

	


; <<<<<<<<<<  Keyboard Interrupt Handler  >>>>>>>>>>
;
; Description:
;
;       When a key is pressed on the keyboard, an interrupt will be
;       generated to this routine. This routine will then handle
;       the generation of an interrupt acknowledge and store the
;       character typed in KEYCHR.


KEYINT	PROC	FAR

	STI				; allow further interrupts

	PUSH	AX
	PUSH	CX
	PUSH	SI
	PUSH	DS			; save registers

	CLD				; forward direction

	MOV	AX,DATASEG
	MOV	DS,AX			; set DS to data seg

	if IBM
	IN	AL,KB_DATA		; read in the character
	PUSH	AX			; save character
	IN	AL,KB_CNTL		; read control port
	MOV	AH,AL			; save value
	OR	AL,80H			; reset bit for keyboard
	OUT	KB_CNTL,AL
	XCHG	AH,AL			; get back original control
	OUT	KB_CNTL,AL		; keyboard has been reset
	POP	AX			; get back character pressed
	endif
	if HYP
	IN	AL,KB_DATA		; read in the character
	NOT	AL
	PUSH	AX			; save character
	MOV	AL,80H			; reset bit for keyboard
	OUT	KB_CNTL,AL
	POP	AX			; get back character pressed
	endif

	MOV	KEYCHR,AL		; store character

	CALL	KB_DCD			; keyboard decode routine

	CLI				; turn off interrupts
	MOV	AL,20H
	OUT	20H,AL			; send end of interrupt to 8259

	POP	DS
	POP	SI
	POP	CX
	POP	AX			; restore registers

	IRET				; return from interrupt

KEYINT	ENDP




; ==========  Keyboard Decode Routine  ==========
;
; On Entry:
;
;       KEYCHR = key that has been pressed
;
; Description:
;
;       This routine will decode the key pressed and if valid, will call
;	the proper keyboard handeller routine.


KB_DCD	PROC	NEAR

	MOV	SI,OFFSET DGROUP:KB_TABLE	; point to table
	MOV	AL,KEYCHR			; get key pressed
	MOV	CX,NUM_CHR			; load number of characters

; ----- loop until character found in table or end of table

KD1:	CMP	AL,[SI]			; compare character to table
	JE	KD2			; jump if the same
	ADD	SI,3			; point to next character
	LOOP	KD1			; loop if not all characters checked

	RET				; character doesn't match table entrys

; ----- jump to the address stored in the table

KD2:	INC	SI			; point to address
	JMP	WORD PTR [SI]		; jump to address pointed to by SI

KB_DCD	ENDP

	PAGE

; ==========  Key Press Handler  ==========
;
; Description:
;
;       The routines in this procedure are called when a valid key has been
;       pressed and the appropiate action will take place. Also, when
;       control-left arrow or control-right arrow has been pressed, the
;       screen will shift left or right if the display is the color card.
;
;       The following PUBLIC variables are used:
;
;	PDIR	-     0	= stop
;		      1	= left   ( left arrow )
;		      2 = right  ( right arrow )
;
;	LDIR	-     1	= left   ( left arrow has been pressed )
;		      2 = right  ( right arrow has been pressed )
;
;	KEYDIR	- player direction -  0 - right
;				      1 - right-up
;				      2 - up
;				      3 - left-up
;				      4 - left
;				      5 - left-down
;				      6 - down
;				      7 - right-down
;				     -1 - none pressed
;
;	LKDIR	- last direction pressed
;
;	FIRE	- TRUE	= key is down  ( F1 )
;		  FALSE	= key is up
;
;	FIRES	- TRUE  = key has been pressed  ( F1 ) - program must reset variable
;
;	SOUND	- TRUE	= enable sound  ( F10 )
;		  FALSE = disable sound
;
;	ENDGAME	- TRUE	= Esc has been pressed



KEY_PRS	PROC	NEAR

; ----- left arrow key down

LEFT_DOWN:

	MOV	L_KEY,TRUE		; set left key flag
	MOV	KEYDIR,DIRL		; Show present key is left
	MOV	LKDIR,DIRL		; Show that the last key pressed was left
	MOV	PDIR,LEFT		; set direction variable to left
	MOV	LDIR,LEFT		; set last direction variable to left
	CALL	SCREEN_ADJ		; call screen adjust program
	RET

; ----- left arrow key up

LEFT_UP:

	MOV	L_KEY,FALSE		; set left key flag
	CMP	KEYDIR,DIRL
	JNZ	KSP1
	CALL	KEYCHK
KSP1:	CMP	PDIR,LEFT
	JE	KP1			; jump if left arrow key is down
	RET				; return in it is not

KP1:	CMP	R_KEY,TRUE
	JNE	KP2			; jump if right key not down
	MOV	PDIR,RIGHT		; else set PDIR to right
	RET

KP2:	MOV	PDIR,STOP		; indicate key has been released
	RET				; by setting PDIR to stop

; ----- right arrow key down

RIGHT_DOWN:

	MOV	R_KEY,TRUE		; set left key flag
	MOV	KEYDIR,DIRR		; Show present key is right
	MOV	LKDIR,DIRR		; Show that the last key pressed was right
	MOV	PDIR,RIGHT		; set direction variable to right
	MOV	LDIR,RIGHT		; set last direction variable to right
	CALL	SCREEN_ADJ		; call screen adjust routine
	RET

; ----- right arrow key up

RIGHT_UP:

	MOV	R_KEY,FALSE		; set right key flag
	CMP	KEYDIR,DIRR
	JNZ	KSP2
	CALL	KEYCHK
KSP2:	CMP	PDIR,RIGHT
	JE	KP3			; jump if right arrow key is down
	RET

KP3:	CMP	L_KEY,TRUE
	JNE	KP4			; jump if left key not down
	MOV	PDIR,LEFT		; else set PDIR to left
	RET

KP4:	MOV	PDIR,STOP		; set direction to stop
	RET

; ----- up arrow key down

UP_DOWN:

	MOV	U_KEY,TRUE		; set left key flag
	MOV	KEYDIR,DIRU		; Show present key is left
	MOV	LKDIR,DIRU		; Show that the last key pressed was left
	RET

; ----- up arrow key up

UP_UP:

	MOV	U_KEY,FALSE		; set left key flag
	CMP	KEYDIR,DIRU
	JNZ	KSP3
	CALL	KEYCHK
KSP3:	RET				; return in it is not

; ----- down arrow key down

DOWN_DOWN:

	MOV	D_KEY,TRUE		; set key flag
	MOV	KEYDIR,DIRD		; Show present key
	MOV	LKDIR,DIRD		; Show that the last key pressed 
	RET

; ----- down arrow key up

DOWN_UP:

	MOV	D_KEY,FALSE		; set left key flag
	CMP	KEYDIR,DIRD
	JNZ	KSP4
	CALL	KEYCHK
KSP4:	RET				; return in it is not

; ----- up-right arrow key down

UPR_DOWN:

	MOV	UR_KEY,TRUE		; set key flag
	MOV	KEYDIR,DIRUR		; Show present key
	MOV	LKDIR,DIRUR		; Show that the last key pressed 
	RET

; ----- up-right arrow key up

UPR_UP:

	MOV	UR_KEY,FALSE		; set left key flag
	CMP	KEYDIR,DIRUR
	JNZ	KSP5
	CALL	KEYCHK
KSP5:	RET				; return in it is not

; ----- up-left arrow key down

UPL_DOWN:

	MOV	UL_KEY,TRUE		; set key flag
	MOV	KEYDIR,DIRUL		; Show present key
	MOV	LKDIR,DIRUL		; Show that the last key pressed 
	RET

; ----- up-left arrow key up

UPL_UP:

	MOV	UL_KEY,FALSE		; set left key flag
	CMP	KEYDIR,DIRUL
	JNZ	KSP6
	CALL	KEYCHK
KSP6:	RET				; return in it is not

; ----- down-right arrow key down

DOWNR_DOWN:

	MOV	DR_KEY,TRUE		; set key flag
	MOV	KEYDIR,DIRDR		; Show present key
	MOV	LKDIR,DIRDR		; Show that the last key pressed 
	RET

; ----- down-right arrow key up

DOWNR_UP:

	MOV	DR_KEY,FALSE		; set left key flag
	CMP	KEYDIR,DIRDR
	JNZ	KSP7
	CALL	KEYCHK
KSP7:	RET				; return in it is not


; ----- down-left arrow key down

DOWNL_DOWN:

	MOV	DL_KEY,TRUE		; set key flag
	MOV	KEYDIR,DIRDL		; Show present key
	MOV	LKDIR,DIRDL		; Show that the last key pressed 
	RET

; ----- down-left arrow key up

DOWNL_UP:

	MOV	DL_KEY,FALSE		; set left key flag
	CMP	KEYDIR,DIRDL
	JNZ	KSP8
	CALL	KEYCHK
KSP8:	RET				; return in it is not

; ----- fire key down

FIRE_DOWN:

	MOV	FIRES,TRUE		; set fire key flag
	MOV	FIRE,TRUE		; set fire variable to true
	RET

; ----- fire key up

FIRE_UP:

	MOV	FIRE,FALSE		; set fire variable to false
	RET

; ----- control key down

RESET1_DOWN:

	MOV	RST1,TRUE		; indicate control key is down
	RET

; ----- control key up

RESET1_UP:

	MOV	RST1,FALSE		; indicate control key up
	RET

; ----- alternale key down

RESET2_DOWN:

	MOV	RST2,TRUE		; indicate alternate key down
	RET

; ----- alternate key up

RESET2_UP:

	MOV	RST2,FALSE		; indicate alternate key up
	RET

; ----- delete key down

RESET3_DOWN:

	MOV	RST3,TRUE		; indicate delete key down
	JMP	CHECK_END		; check for end of game

; ----- delete key up

RESET3_UP:

	MOV	RST3,FALSE		; indicate delete key up
	RET

; ----- F7 down

MUSIC_DOWN:

	CMP	MSC_UP,TRUE
	JE	MS1			; jump if F7 was not down before
	RET				; else return

MS1:	NOT	MUSIC			; change the state of MUSIC
	MOV	MSC_UP,FALSE		; indicate F7 was down
	RET

; ----- F7 up

MUSIC_UP:

	MOV	MSC_UP,TRUE		; indicate F7 was up
	RET

; ----- F9 down

SOUND_DOWN:

	CMP	SND_UP,TRUE
	JE	SD1			; jump if F9 was not down before
	RET				; else return

SD1:	NOT	SOUND			; change the state of SOUND
	MOV	SND_UP,FALSE		; indicate F9 was down
	RET

; ----- F9 up

SOUND_UP:

	MOV	SND_UP,TRUE		; indicate F9 was up
	RET
	
; ----- Esc down

EXIT_DOWN:

	MOV	ENDGAME,TRUE		; indicate Esc was down
	RET

KEY_PRS	ENDP




; ==========  Key Down Check  =========
;
; Description:
;
;	This routine checks if any of the 8 direction keys are still down
;	if a match is found, it is treated as a new key press
;

KEYCHK	PROC	NEAR

	MOV	KEYDIR,DIRNONE
	CMP	U_KEY,0
	JZ	CHK1		; Key is not pressed
	MOV	KEYDIR,DIRU
	MOV	LKDIR,DIRU
CHK1:	CMP	D_KEY,0
	JZ	CHK2		; Key is not pressed
	MOV	KEYDIR,DIRD
	MOV	LKDIR,DIRD
CHK2:	CMP	L_KEY,0
	JZ	CHK3		; Key is not pressed
	MOV	KEYDIR,DIRL
	MOV	LKDIR,DIRL
CHK3:	CMP	R_KEY,0
	JZ	CHK4		; Key is not pressed
	MOV	KEYDIR,DIRR
	MOV	LKDIR,DIRR
CHK4:	CMP	UR_KEY,0
	JZ	CHK5		; Key is not pressed
	MOV	KEYDIR,DIRUR
	MOV	LKDIR,DIRUR
CHK5:	CMP	UL_KEY,0
	JZ	CHK6		; Key is not pressed
	MOV	KEYDIR,DIRUL
	MOV	LKDIR,DIRUL
CHK6:	CMP	DR_KEY,0
	JZ	CHK7		; Key is not pressed
	MOV	KEYDIR,DIRDR
	MOV	LKDIR,DIRDR
CHK7:	CMP	DL_KEY,0
	JZ	CHK8		; Key is not pressed
	MOV	KEYDIR,DIRDL
	MOV	LKDIR,DIRDL
CHK8:	RET

KEYCHK	ENDP




; ==========  Game End Check  ==========
;
; Description:
;
;       This routine will check to see if the control-alternate-delete
;       keys are down. If they are, a reboot will occour.


CHECK_END	PROC	NEAR

	CMP	RST1,TRUE
	JE	BC1			; jump if control key is down
	RET				; else return

BC1:	CMP	RST2,TRUE
	JE	BC2			; jump if alternate key is down
	RET				; else return

BC2:	CMP	RST3,TRUE
	JE	BC3			; jump if delete key is down
	RET				; else return

	ASSUME	DS:ROMDATA

BC3:	MOV	AX,ROMDATA
	MOV	DS,AX			; set data seg to rom data segment

	MOV	RESET_FLAG,1234H	; signal keyboard reset
	JMP	FAR PTR REBOOT		; jump to warm boot routine

	ASSUME	DS:DGROUP

CHECK_END	ENDP



; ==========  Screen Adjust Routine  ==========
;
; Description:
;
;       This routine will determine if a control-left arrow or
;       control-right arrow is pressed. If they are and the display
;       is the color card, then the horizontal sync position will be
;       altered to cause the screen to move left or right.


SCREEN_ADJ	PROC	NEAR

	CMP	RST1,TRUE
	JE	SA2			; jump if control key is down
	RET				; else return

SA2:	CMP	L_KEY,TRUE
	JNE	SA3			; jump if left arrow key is not down
	INC	SYNC_POS		; increment the sync position
	CMP	SYNC_POS,SYNC_NORM+3	; check for limit
	JNZ	SBB1
	MOV	SYNC_POS,SYNC_NORM+2	; reset sync position
SBB1:	PUSH	DX
	MOV	DX,3D4H			; address of register select port
	MOV	AL,2			; horizontal sync position register
	OUT	DX,AL			; send to CRT controller ( 6845 )
	MOV	AL,SYNC_POS		; get sync position
	INC	DX			; point to data register of 6845
	OUT	DX,AL			; send to 6845
	POP	DX
	RET

SA3:	CMP	R_KEY,TRUE
	JNE	SA4			; jump if right arrow key is not down
	DEC	SYNC_POS		; decrement the sync position
	CMP	SYNC_POS,SYNC_NORM-3	; check for limit
	JNZ	SBB2
	MOV	SYNC_POS,SYNC_NORM-2	; reset sync position
SBB2:	PUSH	DX
	MOV	DX,3D4H			; address of register select port
	MOV	AL,2			; horizontal sync position register
	OUT	DX,AL			; send to CRT controller ( 6845 )
	MOV	AL,SYNC_POS		; get sync position
	INC	DX			; point to data register of 6845
	OUT	DX,AL			; send to 6845
	POP	DX
	RET

SA4:	RET

SCREEN_ADJ	ENDP




; ==========  Peek Routine  ==========
;
; Command Format:
;
;       n=peek(addr);
;	int  addr;
;
; Description:
;
;       The value at location addr and segment defined
;       by defseg will returned.


PEEK	PROC	NEAR

	PUSH	BP
	MOV	BP,SP

	PUSH	DS

	MOV	SI,[BP]+4		; put address in BX
	MOV	CX,DEFSEG		; get data segment
	MOV	DS,CX			; and put in DS

	MOV	AL,[SI]			; get value into AX
	MOV	AH,0			; clear high byte

	POP	DS
	POP	BP
	RET

PEEK	ENDP



; ==========  Poke Routine  ==========
;
; Commamd format:
;
;       poke(addr,byte);
;       int	addr;
;	char	byte;
;
; Description:
;
;       The value byte will be placed in location addr
;       and segment defseg.


POKE	PROC	NEAR

	PUSH	BP
	MOV	BP,SP

	PUSH	DS

	MOV	DI,[BP]+4		; get address
	MOV	BX,[BP]+6		; get data byte
	MOV	CX,DEFSEG
	MOV	DS,CX			; set DS to proper segment

	MOV	[DI],BL			; write byte

	POP	DS
	POP	BP
	RET

POKE	ENDP

PROG	ENDS
	END
