DISP PROC NEAR USES AS BX DX SI

    PUSHF
    MOV BX, 8
    MOV AH, 7FH
    MOV SI, OFFSET MEM-1
    MOV DX, 701H

    .REPEAT
        MOV AL, AH
        OUT DX, AL
        DEC DX
        MOV AL, [BX+SI]
        OUT DX, AL
        CALL DELAY
        ROR AH, 1
        INC DX
        DEC BX
    .UNTIL BX == 0

    POPF
    RET

DISP ENDP


DELAY PROC NEAR USES CX
    MOV CX, XXXX

D1:
    LOOP D1
    RET

DELAY ENDP



PORTA_ADDRESS EQU 700H
PORTB_ADDRESS EQU 701H
COMMAND_ADDRESS EQU 703H

SEND MACRO PORTA_DATA, PORTB_DATA, DELAY    
    MOV AL, PORTA_DATA
    MOV DX, PORTA_ADDRESS
    OUT DX, AL
    MOV AL, PORTB_DATA
    MOV DX, PORTB_ADDRESS
    OUT DX, AL
    OR AL, 00000100B
    OUT DX, AL
    AND AL, 11111011B
    NOP
    NOP
    OUT DX, AL
    MOV BL, DELAY
    CALL MS_DELAY
    ENDM

START:
    MOV AL, 80H
    MOV DX, COMMAND_ADDRESS
    OUT DX, AL
    MOV AL, 0
    MOV DX, PORTB_ADDRESS
    SEND 30H, 2, 16
    SEND 30H, 2, 5
    SEND 30H, 2, 1
    SEND 38H, 2, 1
    SEND 8, 2, 1
    SEND 1, 2, 2
    SEND 0CH, 2, 1
    SEND 6, 2, 1



PORTA_ADDRESS EQU 700H
PORTB_ADDRESS EQU 701H
COMMAND_ADDRESS EQU 703H

BUSY PROC NEAR USES DX AX

    PUSHF
    MOV DX, COMMAND_ADDRESS
    MOV AL, 90H
    OUT DX, AL
    .REPEAT 
        MOV AL, 1
        OUT DX, AL
        MOV DX, PORTA_ADDRESS
        MOV AL, DX
        SHL AL, 1
    .UNTIL !CARRY?
    MOV DX, COMMAND_ADDRESS
    MOV AL, 80H
    OUT DX, AL
    POPF
    RET

BUSY ENDP




WRITE PROC NEAR 
      MOV AL, BL
      MOV DX, PORTA_ADDRESS
      OUT DX, AL
      MOV AL, 0
      MOV DX, PORTB_ADDRESS
      OUT DX, AL
      OR AL, 00000100B
      OUT DX, AL
      AND AL, 11111011B
      NOP
      NOP
      OUT DX, AL
      CALL BUSY
      RET
WRITE ENDP


CLS PROC NEAR 
    SEND 1, 2, 2
    RET
CLS ENDP



PORT EQU 40H

STEP PROC NEAR USES CX AX

    MOV AL, POS
    OR CX, CX
    IF !ZERO?
        .IF !SIGN?
            .REPEAT
                ROL AL, 1
                OUT PORT, AL
                CALL DELAY
            .UNTILCXZ
        .ELSE
            .REPEAT
                ROR AL, 1
                OUT PORT, AL
                CALL DELAY
            .UNTILCXZ
        .ENDIF
    .ENDIF
    MOV POS, AL
    RET

STEP ENDP




C0H --> 1100 0000
C1H --> 1100 0001
C2H --> 1100 0010
C3H --> 1100 0011
C4H --> 1100 0100
C5H --> 1100 0101
C6H --> 1100 0110
C7H --> 1100 0111