.global strFindN
.text
strFindN:
	PUSH {R4}
	MOV R3, R0
	MOV R0, #0
LOOP:
	LDRSB R4, [R3], #1
	CMP R4, #0
	MOVEQ R0, #-1
	BEQ END
	CMP R4, R1
	ADDNE R0, R0, #1
	SUBEQS R2, R2, #1
	ADDNE R0, R0, #1
	BNE LOOP
END:
	BX LR
