	page	60,132
	TITLE	- Disk boot program creator   Ver 3.3 -

;	Slow seek rate is used for compatability with other systems


; =========================== Disk Formatter for Non-DOS Game Loader ===============
;
;
;	calling format:
;			format();
;
;	discription:
;
;			This module will be called to format a blank diskette in drive b:
;			The diskette will be created with the following format:
;				Track 0		- standard 512 byte sectors
;				Track 1 - 22	- not formatted
;				Track 23 - 38	- 1024 byte sectors
;				Track 39	- standard 512 byte sectors
;
;	on exit:
;			If the format was sucessfull then a TRUE is returned
;			if any error occured, a message will be displayed & False will be returned
;

; ===================== Disk buffer write (1024 bytes/sector) ==================
;
;	Discription:
;		This routine moves the 4K buffer (diskdat) to the track passed by C
;		to drive b:
;
;	Format:
;		trackw(num);
;
;	On exit:
;		TRUE is returned if the write was sucessfull, False if not.
;

; =========================== Write boot sectors 1 & 2 ==========================
;
;
;	Discription:
;			This module takes the 2 boot programs & writes them to disk
;			drive A:
;			The first boot program "BOOT1" is loaded by DOS from track 0, sector 1
;			This program is stored ar 0000:7C00.  When the program is loaded
;			it is given control.
;			BOOT1 then loads program BOOT2 from track 0, sector 2.
;			BOOT2 is loaded into 0060:0000.  When this program has been loaded
;			it is given control.
;
;	On exit:	True is returned if the operation was sucessfull, FALSE if not.
;


; ---- DATA AREA

DGROUP	GROUP	DATA
DATA	SEGMENT	BYTE	PUBLIC 'DATA'
	ASSUME	DS:DGROUP

	PUBLIC	DISKDAT


;------------- Data buffers for disk transfers --------------------------------
;
;		Diskdat is declared as an external in 'C'
;		This buffer is then loaded with the data to be written to disk
;

DISKDAT	DB	01000H DUP(?)	; Program buffer
SPAREB	DB	400H DUP(?)	; 1K spare read buffer


;------------- Disk format tables ---------------------------------------------
;
;		Track 0 & 39 are formatted in standard 512 byte sectors
;
;		FDATA1 is used to format tracks 23 to 38 to 1024 bytes per sector
;		Sector 38 is formatted to alow the copy protect check
;

TRACK0	DB	0,0,1,2, 0,0,2,2, 0,0,3,2, 0,0,4,2, 0,0,5,2, 0,0,6,2, 0,0,7,2, 0,0,8,2
TRACK39	DB	39,0,1,2, 39,0,2,2, 39,0,3,2, 39,0,4,2, 39,0,5,2, 39,0,6,2, 39,0,7,2, 39,0,8,2

FDATA1	DB	23,0,1,3, 23,0,2,3, 23,0,3,3, 23,0,4,3
	DB	24,0,1,3, 24,0,2,3, 24,0,3,3, 24,0,4,3
	DB	25,0,1,3, 25,0,2,3, 25,0,3,3, 25,0,4,3
	DB	26,0,1,3, 26,0,2,3, 26,0,3,3, 26,0,4,3
	DB	27,0,1,3, 27,0,2,3, 27,0,3,3, 27,0,4,3
	DB	28,0,1,3, 28,0,2,3, 28,0,3,3, 28,0,4,3
	DB	29,0,1,3, 29,0,2,3, 29,0,3,3, 29,0,4,3
	DB	30,0,1,3, 30,0,2,3, 30,0,3,3, 30,0,4,3
	DB	31,0,1,3, 31,0,2,3, 31,0,3,3, 31,0,4,3
	DB	32,0,1,3, 32,0,2,3, 32,0,3,3, 32,0,4,3
	DB	33,0,1,3, 33,0,2,3, 33,0,3,3, 33,0,4,3
	DB	34,0,1,3, 34,0,2,3, 34,0,3,3, 34,0,4,3
	DB	35,0,1,3, 35,0,2,3, 35,0,3,3, 35,0,4,3
	DB	36,0,1,3, 36,0,2,3, 36,0,3,3, 36,0,4,3
	DB	37,0,1,3, 37,0,2,3, 37,0,3,3, 37,0,4,3
	DB	38,0,1,3, 38,0,2,3, 38,0,3,3, 38,0,4,3


; ------------ Disk parameter data --------------------------------------
;
DPARMA	DW	?		; Storage for old disk parm address vector
DPARMS	DW	?		; Storage for old disk parm sector vector

; ----- Disk parm table for 1024 bytes per sector during disk creation 

DPARM	DB	0CFH
	DB	02
	DB	25H		; Motor wait before turn off
	DB	03		; Bytes per sector - 2=512  3=1024
	DB	04		; Last sector in track 
	DB	2AH		; Gap length
	DB 	0FFH		; DTL
	DB 	50H		; Gap for format
	DB	0F6H		; Filler byte for format
	DB	200		; Settle time
	DB	08		; Motor start delay

; ------------- Variables used within this program --------------------
;
ERRCNT	DB	0		; Error retry counter
ERRSTAT	DW	0		; Error status - Set to FALSE to show an error has occured


DATA	ENDS




; --------------- Segment to create interupt vectors --------------------------

LOSEG	SEGMENT AT 0

	ORG	78H

INTADR	DW	?		; Address of disk perm table
INTSEG	DW	?		; Segment of disk perm table


 LOSEG	ENDS




; --------------- Segment to enter boot program -------------------------------

NEWSEG	SEGMENT AT 60H

	ORG	0

BOOT1P	PROC	FAR

DUMMY	DW	?

BOOT1P	ENDP

NEWSEG	ENDS





; ================================  'C' Interface routines  ====================



PGROUP	GROUP	PROG
PROG	SEGMENT	BYTE	PUBLIC	'PROG'
	ASSUME	CS:PGROUP, DS:DGROUP, ES:DGROUP

	PUBLIC	FORMAT, TRACKW, BOOTW


; =========================== Disk Formatter for Non-DOS Game Loader ===============
;
;
;	calling format:
;			format();
;
;	discription:
;
;			This module will be called to format a blank diskette in drive a:
;			The diskette will be created with the following format:
;				Track 0		- standard 512 byte sectors
;				Track 1 - 22	- not formatted
;				Track 23 - 38	- 1024 byte sectors
;				Track 39	- standard 512 byte sectors
;
;	on exit:
;			If the format was sucessfull then a TRUE is returned
;			if any error occured, a message will be displayed & False will be returned
;
FORMAT	PROC	NEAR
	ASSUME	CS:PGROUP, DS:DGROUP, ES:DGROUP

	MOV	ERRSTAT,0FFH	; Initialize error return flag

	CALL	SAVPRM		; Save old disk perams & set up new ones for 1024 byte sectors
	CALL	FORM23		; Format tracks 23 - 38 in 1024 byte sectors
	CALL	RETPRM		; Restore 512 byte sector perams od DOS
	CALL	FORM0		; Format track 0 in standard 512 byte sectors
	CALL	FORM39		; Format track 39 in standard 512 byte sectors

	MOV	AX,ERRSTAT	; Return error status to calling program
	RET

FORMAT	ENDP

; -------------- Save old disk parm vector & set up new one --------------------
;
SAVPRM	PROC	NEAR

	ASSUME	DS:LOSEG

	PUSH	DS		; Save DS
	MOV	AX,0
	MOV	DS,AX		; Set up new DS ot low memory

	MOV	AX,INTADR
	MOV	DPARMA,AX	; Save old address
	MOV	AX,INTSEG
	MOV	DPARMS,AX	; Save old segment

	MOV	AX,OFFSET DGROUP:DPARM		; Point to address of new table
	MOV	BX,ES				; Get new segment
	CLI			; Disable interupts
	MOV	INTADR,AX	; Store new address
	MOV	INTSEG,BX	; Store new segment
	STI			; Enable interupts

	ASSUME	DS:DGROUP
	POP	DS		; Restore data seg

	MOV	AX,0
	INT	13H		; Reset disk system for parms to take efect
	RET

SAVPRM	ENDP


; ----------------- Restore old disk parm vector ------------------------
;
RETPRM	PROC	NEAR

	ASSUME	DS:LOSEG

	PUSH	DS		; Save DS
	MOV	AX,0
	MOV	DS,AX		; Set up new DS ot low memory

	MOV	AX,DPARMA	; Get old address
	MOV	BX,DPARMS	; Get old segment

	CLI			; Disable interupts
	MOV	INTADR,AX	; Store address
	MOV	INTSEG,BX	; Store segment
	STI			; Enable interupts

	ASSUME	DS:DGROUP
	POP	DS		; Restore data seg

	MOV	AX,0
	INT	13H		; Reset disk system for parms to take efect

	RET

RETPRM	ENDP



; ---------------- Format tracks 23 to 38 in 1024 byte sectors ----------------
;
FORM23	PROC	NEAR

	MOV	ERRCNT,6		; Set up error retry count

FORMT1:	MOV	CH,23	; Get track number
	MOV	BX,OFFSET DGROUP:FDATA1		; Get address of buffer


FORM1R:	MOV	AX,DS		; Set segment of buffer	
	MOV	ES,AX
; BX already set up
	MOV	AH,5		; Format code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CL,1		; Start sector (1-8)
; CH already set up
	MOV	AL,4		; Number of sectors
	INT	13H
	JC	FORM1E		; Retry if error
	MOV	AH,4		; Verify code
	MOV	AL,4		; Number of sectors
	INT	13H
	JC	FORM1E		; Retry if error

	ADD	BX,16		; Add offset to next track data
	INC	CH
	CMP	CH,39
	JNZ	FORM1R
	
	RET


FORM1E:	MOV	AX,0
	INT	13H		; Reset disk system
	DEC	ERRCNT
	JNZ	FORMT1		; Try again
	MOV	ERRSTAT,0	; Show an error occured
	RET			; Exit

FORM23	ENDP




; ------------ Format track 0 ---------------
;
FORM0	PROC	NEAR

	MOV	ERRCNT,3		; Set up error retry count

FORM0R:	MOV	AX,DS		; Set segment of buffer	
	MOV	ES,AX
	MOV	BX,OFFSET DGROUP:TRACK0		; Get address of buffer
	MOV	AH,5		; Format code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CL,1		; Start sector (1-8)
	MOV	CH,0 		; Get track number
	MOV	AL,8		; Number of sectors
	INT	13H
	JC	FORM0E		; Retry if error
	MOV	AH,4		; Verify code
	MOV	AL,8		; Number of sectors
	INT	13H
	JC	FORM0E		; Retry if error

	RET


FORM0E:	MOV	AX,0
	INT	13H		; Reset disk system
	DEC	ERRCNT
	JNZ	FORM0R		; Try again
	MOV	ERRSTAT,0	; Show an error occured
	RET			; Exit

FORM0	ENDP



; ------------ Format track 39 ---------------
;
FORM39	PROC NEAR

	MOV	ERRCNT,3		; Set up error retry count

FORM9R:	MOV	AX,DS		; Set segment of buffer	
	MOV	ES,AX
	MOV	BX,OFFSET DGROUP:TRACK39		; Get address of buffer
	MOV	AH,5		; Format code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CL,1		; Start sector (1-8)
	MOV	CH,39	; Get track number
	MOV	AL,8		; Number of sectors
	INT	13H
	JC	FORM9E		; Retry if error
	MOV	AH,4		; Verify code
	MOV	AL,8		; Number of sectors
	INT	13H
	JC	FORM9E		; Retry if error

	RET


FORM9E:	MOV	AX,0
	INT	13H		; Reset disk system
	DEC	ERRCNT
	JNZ	FORM9R		; Try again
	MOV	ERRSTAT,0	; Show an error occured
	RET			; Exit

FORM39	ENDP









; ===================== Disk buffer write (1024 bytes/sector) ==================
;
;	Discription:
;		This routine moves the 4K buffer (diskdat) to the track passed by C
;		to drive a:
;
;	Format:
;		trackw(num);
;
;	On exit:
;		TRUE is returned if the write was sucessfull, False if not.
;

TRACKW	PROC	NEAR
	ASSUME	CS:PGROUP, DS:DGROUP, ES:DGROUP
	
	PUSH	BP
	MOV	BP,SP
	PUSH	SS
	PUSH	ES


	MOV	ERRCNT,10	; Error retry count
	MOV	ERRSTAT,0FFH	; Set error flag for OK

	CALL	SAVPRM		; Save old disk parm vector & set up new one

LOOP1:	MOV	AH,14		; Print a '.' to show attempts
	MOV	AL,'.'
	MOV	BX,0
	INT	10H

;	Read a sector to get disk up to speed

READY:
	MOV	AX,DS		; Set segment of buffer	
	MOV	ES,AX
	MOV	BX,OFFSET DGROUP:SPAREB	; Get address of buffer
	MOV	AH,2		; read code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CX,[BP]+4	; Get track number into CL
	MOV	CH,CL
	MOV	CL,1		; Start sector (1-8)
	MOV	AL,1		; Number of sectors
	INT	13H

	JC	ERRP		; Retry if error

;	Write the track

	MOV	AX,DS		; Set segment of buffer	
	MOV	ES,AX
	MOV	BX,OFFSET DGROUP:DISKDAT	; Get address of buffer
	MOV	AH,3		; write code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CX,[BP]+4	; Get track number into CL
	MOV	CH,CL
	MOV	CL,1		; Start sector (1-8)
	MOV	AL,4		; Number of sectors
	INT	13H
	JC	ERRP		; Retry if error
	MOV	AH,4		; Verify code
	MOV	AL,4		; Number of sectors
;	INT	13H
;	JC	ERRP		; Retry if error
	JMP	OKTRK		; Continue on


ERRP:	MOV	AX,0
	INT	13H		; Reset disk drive
	DEC	ERRCNT
	JNZ	LOOP1		; Try again
	MOV	ERRSTAT,0	; Show an error
	JMP	EXITW		; Dont print '#' if an error occured

OKTRK:	MOV	AH,14
	MOV	AL,'#'
	MOV	BX,0
	INT	10H


EXITW:	CALL	RETPRM		;	Restore old disk parm vector


	MOV	AX,ERRSTAT	; Return error status
	POP	ES
	POP	SS
	POP	BP

	RET

TRACKW	ENDP




; =========================== Write boot sectors 1 & 2 ==========================
;
;
;	Discription:
;			This module takes the 2 boot programs & writes them to disk
;			drive a:
;			The first boot program "BOOT1" is loaded by DOS from track 0, sector 1
;			This program is stored ar 0000:7C00.  When the program is loaded
;			it is given control.
;			BOOT1 then loads program BOOT2 from track 0, sector 2.
;			BOOT2 is loaded into 0060:0000.  When this program has been loaded
;			it is given control.
;
;	On exit:	True is returned if the operation was sucessfull, FALSE if not.


;			
; ----------- Write boot track 0, sector 1 to disk ---------------------------

BOOTW	PROC	NEAR
	ASSUME	CS:PGROUP, DS:DGROUP, ES:DGROUP

	PUSH	BP
	MOV	BP,SP
	PUSH	SS
	PUSH	ES

	MOV	ERRSTAT,0FFH		; Set up error flag to OK

	CALL	SECT1W			; Write sector 1 to disk B:
	CALL	SECT2W			; Write sector 2 to disk B:

	MOV	AX,ERRSTAT		; Return error flag

	POP	ES
	POP	SS
	POP	BP
	
	RET


BOOTW	ENDP


SECT1W	PROC	NEAR
;
; ------------------ Write boot sector 1 

	MOV	ERRCNT,3		; Error retry count
LOOP3:	MOV	AH,14
	MOV	AL,'1'
	MOV	BX,0
	INT	10H

;	Read a sector to get disk up to speed

READY1:	MOV	AX,DS		; Set segment of buffer	
	MOV	ES,AX
	MOV	BX,OFFSET DGROUP:SPAREB	; Get address of buffer
	MOV	AH,2		; read code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CH,0		; Get track number 
	MOV	CL,1		; Start sector (1-8)
	MOV	AL,1		; Number of sectors
	INT	13H
	JC	ERR1		; Retry if error

;	Write the track

	MOV	AX,CS		; Set segment of buffer	
	MOV	ES,AX
	MOV	BX,OFFSET PGROUP:FIRSTB	; Get address of buffer
	MOV	AH,3		; write code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CH,0		; Get track number 
	MOV	CL,1		; Start sector (1-8)
	MOV	AL,1		; Number of sectors
	INT	13H
	JC	ERR1		; Retry if error
	MOV	AH,4		; Verify code
	MOV	AL,1		; Number of sectors
	INT	13H
	JC	ERR1		; Retry if error
	RET

ERR1:	MOV	AX,0
	INT	13H		; Reset disk drive
	DEC	ERRCNT
	JNZ	LOOP3
	MOV	ERRSTAT,0	; Show an error occured
	RET

SECT1W	ENDP



SECT2W	PROC	NEAR

; -------------- Write boot sector 2 

	MOV	ERRCNT,3		; Error retry count
LOOP4:	MOV	AH,14
	MOV	AL,'2'
	MOV	BX,0
	INT	10H

;	Read a sector to get disk up to speed

READY2:	MOV	AX,DS		; Set segment of buffer	
	MOV	ES,AX
	MOV	BX,OFFSET DGROUP:SPAREB	; Get address of buffer
	MOV	AH,2		; read code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CH,0		; Get track number 
	MOV	CL,2		; Start sector (1-8)
	MOV	AL,1		; Number of sectors
	INT	13H
	JC	ERR2		; Retry if error

;	Write the track

	MOV	AX,CS		; Set segment of buffer	
	MOV	ES,AX
	MOV	BX,OFFSET PGROUP:PSTART	; Get address of buffer
	MOV	AH,3		; write code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CH,0		; Get track number 
	MOV	CL,2		; Start sector (1-8)
	MOV	AL,1		; Number of sectors
	INT	13H
	JC	ERR2		; Retry if error
	MOV	AH,4		; Verify code
	MOV	AL,1		; Number of sectors
	INT	13H
	JC	ERR2		; Retry if error
	RET


ERR2:	MOV	AX,0
	INT	13H		; Reset disk drive
	DEC	ERRCNT
	JNZ	LOOP4
	MOV	ERRSTAT,0	; Show an error occured
	RET

SECT2W	ENDP


; =================== First boot loader
;
;	This loader will be placed at 0000:7C00 by the ROM BIOS boot routines
;
;	It will then load the second loader into 0060:0000 & transfer 
;	control to the second loader


BOOT1	PROC	NEAR
	ASSUME	CS:PGROUP, DS:PGROUP, ES:NOTHING

FIRSTB:	MOV	AX,CS		; Set up segments	
	MOV	DS,AX
	
        MOV	AX,0
	INT	13H		; Reset disk system

	MOV	AX,60H
	MOV	ES,AX
	MOV	BX,0		; Set buffer for 0060:0000
	MOV	AH,2		; Read code
	MOV	DL,0		; Drive number
	MOV	DH,0		; Head number
	MOV	CH,0		; Track number
	MOV	CL,2		; Sector number (1-8)
	MOV	AL,1		; Number of sectors to read
	INT	13H		; Do disk read
	JC	FIRSTB		; Do untill it works
	JMP	FAR PTR BOOT1P	; Run the program just loaded

BOOT1	ENDP



; ======================= Second boot loader ================================
;
;	This loader will be placed at 0060:0000
;
;	It will 	- create a stack at the top of 64K memory
;			- load the 'C' program from tracks 23 - 37 of drive A:
;			- Jump to 0060:0100 which sould be the start of the 'C' program
;

BOOT2	PROC	NEAR
	ASSUME	CS:PGROUP, DS:PGROUP, ES:NOTHING

PSTART:	MOV	AX,CS
	MOV	DS,AX
	MOV	BX,0F9FFH	; Top of 64K memory with segment of 60H

	CLI
	MOV	SS,AX
	MOV	ES,AX
	MOV	SP,BX		; Set up stack
	STI

; ----------- Set up new disk parms table (1024 byte sectors)

	ASSUME	DS:LOSEG

	MOV	AX,0
	MOV	DS,AX		; Set up data seg for vector storage
	MOV	AX,(OFFSET PGROUP:DISKPS)-(OFFSET PGROUP:PSTART)	; Get address of table
	MOV	BX,60H		; Segment of table

	CLI			; Disable interupts
	MOV	INTADR,AX	; Store address of table
	MOV	INTSEG,BX	; Store segment
	STI			; Enable interupts

	ASSUME	CS:PGROUP, DS:PGROUP, ES:NOTHING

	MOV	AX,CS		; Reload data seg register
	MOV	DS,AX

	MOV	AX,0
	INT	13H		; Reset disk drive


; --------------------- Read in the program 

	MOV	CH,23	; Get track number
	MOV	BX,100H		; Get address of buffer


GETTRK:	MOV	AX,CS		; Set segment of buffer	
	MOV	ES,AX
; BX already set up
	MOV	AH,2		; read code
	MOV	DL,0		; Drive 0
	MOV	DH,0		; Head 0
	MOV	CL,1		; Start sector (1-8)
; CH already set up
	MOV	AL,4		; Number of sectors
	INT	13H
	JC	GETTRK		; Retry if error

	ADD	BX,1000H	; Add 4K to buffer address
	INC	CH
	CMP	CH,38
	JNZ	GETTRK


; ---------------- Set up new disk parms table (512 byte sectors)

	ASSUME	DS:LOSEG

	MOV	AX,0
	MOV	DS,AX		; Set up data seg for vector storage
	MOV	AX,(OFFSET PGROUP:DISKPR)-(OFFSET PGROUP:PSTART)	; Get address of table
	MOV	BX,60H		; Segment of table

	CLI			; Disable interupts
	MOV	INTADR,AX	; Store address of table
	MOV	INTSEG,BX	; Store segment
	STI			; Enable interupts

	ASSUME	CS:PGROUP, DS:PGROUP, ES:NOTHING

	MOV	AX,CS		; Reload data seg register
	MOV	DS,AX

	MOV	AX,0
	INT	13H		; Reset disk drive


	JMP	CPROG		; Go to it!!!!


; ----------- Disk parm table for 512 bytes per sector

DISKPR	DB	0CFH
	DB	02
	DB	25H		; Motor wait before turn off
	DB	02		; Bytes per sector - 2=512  3=1024
	DB	08		; Last sector in track 
	DB	2AH		; Gap length
	DB 	0FFH		; DTL
	DB 	50H		; Gap for format
	DB	0F6H		; Filler byte for format
	DB	200		; Settle time
	DB	08		; Motor start delay


; ----------- Disk parm table for 1024 bytes per sector

DISKPS	DB	0CFH
	DB	02
	DB	25H		; Motor wait before turn off
	DB	03		; Bytes per sector - 2=512  3=1024
	DB	04		; Last sector in track 
	DB	2AH		; Gap length
	DB 	0FFH		; DTL
	DB 	50H		; Gap for format
	DB	0F6H		; Filler byte for format
	DB	200		; Settle time
	DB	08		; Motor start delay



; This has the effect of an ORG 100H within the present segment

TLOC3:	DB	(OFFSET PGROUP:PSTART)+0100H -(OFFSET PGROUP:TLOC3) DUP(?)

CPROG:	NOP
;		CPROG will be the new 'C' program that has just been loaded.
;		It is given control at 0060:0100.


BOOT2	ENDP


PROG	ENDS

	END
