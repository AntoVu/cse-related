.global uint32ToBinary
.text
uint32ToBinary:
	MOV R2, #0x80000000

LOOP:
	TST R1, R2
	MOVEQ R3, #'0'
	MOVNE R3, #'1'
	STRB R3, [R0], #1
	MOV R2, R2, LSR #1
	BNE LOOP
	MOV R3, #0
	STRB R3, [R0]
	BX LR