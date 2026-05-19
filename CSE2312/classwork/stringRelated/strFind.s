.global strFind
.text
strFind:
	MOV R2, R0
	MOV R0, #0
LOOP:
	LDRSB R3, [R2], #1
	CMP R3, #0
	MOVEQ R0, #-1
	BEQ END
	CMP R3, R1
	ADDNE R0, R0, #1
	BNE LOOP
END:
	BX LR