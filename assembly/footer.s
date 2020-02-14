    MOVW R1, 0x0000
    MOVT R1, 0x0000
    PUSH R1


    MOVW R4, 0
    MOVT R4, 0x3F20
    ADD R2, R4, 0x08 
    LDR R3, (R2) 
    OR R3, R3, 0x00000008 
    STR R3, (R2)
    B loop

delay:
    MOVW R5, 0x0000
    MOVT R5, 0x0020
    MOVW R6, 0x0
    MOVT R6, 0x0
    ADD R6, R6, 0x1
    CMP R6, R5
    BLT 0xFFFFFFFC
    MOV PC, LR

on:
    ADD R3, R4, 0x1c 
    MOVW R2, 0x0000 
    MOVT R2, 0x0020 
    STR R2, (R3)
    MOV PC, LR

off:
    ADD R3, R4, 0x28
    STR R2, (R3)
    MOV PC, LR

loop:
    BL on
    BL delay
    BL off
    BL delay
    POP R1
    POP R0
    ADD R1, R1, 0x1
    CMP R1, R0
    PUSH R0
    PUSH R1
    BLT loop