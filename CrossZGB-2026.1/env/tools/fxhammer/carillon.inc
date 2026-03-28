; *************************************************************************
;
;
;       CARILLON PLAYER
;       Music Engine for Nintendo Game Boy and Game Boy Color
;
;       (c)2000 Aleksi Eeben (aleksi@cncd.fi)
;       http://www.cncd.fi/aeeben
;
;
; *************************************************************************

; Included in every Music Bank:

Player_Initialize       equ     $4000
Player_MusicStart       equ     $4003
Player_MusicStop        equ     $4006
Player_SongSelect       equ     $400c   ; a = song number (0 - 7)
Player_MusicUpdate      equ     $4100   ; call once every frame


; Included in every Sample Bank:

Player_SampleUpdate     equ     $4000   ; call 4 times every frame


; Included in every SoundFX Bank:

SoundFX_Trig            equ     $4000   ; a = sound FX number (0 - 59)
SoundFX_Stop            equ     $4003
SoundFX_Update          equ     $4006   ; call once every frame
