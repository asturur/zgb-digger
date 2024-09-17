	page	60,132
	TITLE	- Sound routine for DIGGER    Ver 1.2 -
;
;	Modified io use normal tick interupt to turn off disk motor
;
;	Modified to run on different speed CPU's
;
;
; 	The tick counter timer is used to generate a second tone 
;	this tone may be pulse type or square wave.
;	The actual period is loaded by C routines.
;
;	The C routine MUST limit the upper frequency of this tone or the sysyem WILL DIE!!!
;	For pulse mode the maximum width is 100
;		       the maximum freq is a period of 1000
;	For square wave the maximum freq is a period of 500
;
;
;	The following int variables control the sound
;	SN2ON	- TRUE enables the interupt sound
;		- FALSE causes the interupt not to efect the speaker bit
;
;	SOUND2	- is a value that is added to a counter to generate a tick
;       	- the value should be that of the timer count that is loaded by C
;	
;	TICK	- This value is incremented similar to the origional tick routine of bios
;
;	PER2	- This is the ON pulse time in the pulse mode
;




TIMER	EQU	40H	; Interupt timer port
SPKRON	EQU	2	; Speaker enable bit
SPKROF	EQU	0FDh	; 
PORTB	EQU	61H	; Speaker control port



DGROUP	GROUP	DATA
DATA	SEGMENT	BYTE	PUBLIC	'DATA'
	ASSUME	DS:DGROUP

	PUBLIC	SN2ON, SOUND2, TICK, PER2

SN2ON	DW	0
SOUND2	DW	2000		; Duration of low part of cycle passed by C
LTICK	DW	0		; Internal 16 bit tick counter
TICK	DB	0		; Counter of ticks
PER2	DW	1
SN_ADR	DW	0		; Storage for old interrupt address
SN_SEG	DW	0		; Storage for old interrupt segment


DATA	ENDS




PGROUP	GROUP	PROG
PROG	SEGMENT	BYTE	PUBLIC	'PROG'
	ASSUME	CS:PGROUP, DS:DGROUP, ES:DGROUP

	PUBLIC	SN2_INIT, SN2_RST

	EXTRN	TIME:NEAR	; 'C' function to handle timer interupt 
	EXTRN	TDELAY:NEAR	; 'C' function to handle time delay 

ALTDAT	PROC	NEAR

DSEG2	DW	?		; location to store value of data segment

ALTDAT	ENDP

; ========================== Setup interupt vectors ====================
;
;
; Initialize interupt to go to this sound routine
; Disk motors must be turned off first as normal operation is terminated	
;

SN2_INIT	PROC	NEAR

	MOV	AX,DS
	MOV	CS:DSEG2,AX		; Save DS value 

	PUSH	DS

	MOV	AX,0
	MOV	DS,AX
	MOV	DI,20H			; location of timer interupt
	MOV	AX,[DI]			; get old interupt address
	MOV	BX,[DI]+2		; ger ols segment
	MOV	CX,OFFSET PGROUP:SNDINT	; get address of new sound timer routine
	MOV	DX,CS			; get segment
	
	CLI				; Disable interrups

	MOV	[DI],CX			; Store new address
	MOV	[DI+2],DX		; Store new segment
	MOV	DI,180H
	MOV	[DI],AX			; Store old timer interupt at INT 60H
	MOV	[DI+2],BX
	STI				; Enable interrupS

	POP	DS

	MOV	SN_ADR,AX		; Store old interrupt address
	MOV	SN_SEG,BX		; Store old segment

	RET

SN2_INIT	ENDP


; ========================== Reset interupt vectors ====================
;
;	To restore the bios tick routine the old interupt vectors are reinstalled
;

SN2_RST	PROC	NEAR

	MOV	AX,0FFFFH		; Normal interrupt frequency
	MOV	DX,TIMER
	OUT	DX,AL
	MOV	AL,AH			; Get high byte
	OUT	DX,AL

	MOV	AX,SN_ADR		; Get old interrupt address
	MOV	BX,SN_SEG		; Get old segment
	MOV	DI,20H			; location of timer interrupt

	PUSH	DS
	MOV	CX,0
	MOV	DS,CX			; Set DS to DOS segment

	CLI				; Disable interups

	MOV	[DI],AX			; Store address
	MOV	[DI+2],BX		; Store segment

	STI				; Enable interrups

	POP 	DS

	RET

SN2_RST	ENDP

; ======================== Timer Interupt Handler =======================
;
;	This interupt will do pulse & square wave speaker updating if SN2ON = TRUE
;
;	The tick counter will be updated as necessary
;


SNDINT	PROC	FAR

	STI				; Allow further interrups

	PUSH	AX
	PUSH	CX
	PUSH	DX
	PUSH	DS			; Save registers
	PUSH	ES

	CLD				; Forward direction

	MOV	AX,CS:DSEG2		; Set DS to data segment
	MOV	DS,AX
	MOV	ES,AX

	CMP	SN2ON,0			; Check the flag for sound on
	JZ	CHECKT			; Skip sound updating

	CMP	SN2ON,1			; Check if using pulse mode 
	JZ	PULSE


; Do speaker maintenence for square wave
;

	MOV	DX,PORTB		; Speaker control port
	IN	AL,DX
	XOR	AL,SPKRON		; Toggle speaker
	OUT	DX,AL
	JMP	CHECKT			; Update tick counter




; Do speaker maintenence for pulse mode
;

PULSE:	MOV	CX,PER2

	MOV	DX,PORTB		; Speaker control port
	IN	AL,DX
	OR	AL,SPKRON		; Turn on speaker
	OUT	DX,AL
ENT1:	LOOP	ENT1
	AND	AL,SPKROF		; Turn off speaker
	OUT	DX,AL


CHECKT:	MOV	AX,SOUND2		; add counter increment for tick
	ADD	LTICK,AX
	JNC	NOTICK			; No tick cycle
	INC	TICK

	PUSH	DI			; Set up for call to 'C' routine
	PUSH	SI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	CALL	TIME
	INT	60H			; Do normal tick function & EOI signal
	CALL	TDELAY
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	SI
	POP	DI

	POP	ES
	POP	DS
	POP	DX
	POP	CX
	POP	AX

	IRET				; EOI issued by tick routine



NOTICK:	MOV	AL,20H
	OUT	20H,AL			; Send end of interrupt to 8259

	POP	ES
	POP	DS
	POP	DX
	POP	CX
	POP	AX

	IRET

SNDINT	ENDP




PROG	ENDS

	END
