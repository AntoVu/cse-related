.global strLength
.text
strLength:
	MOV R1, R0
	MOV R0, #0
LOOP:
	LDRSB R2, [R1], #1
	CMP R2, #0
	ADDNE R0, R0, #1
	BNE LOOP
	BX LR