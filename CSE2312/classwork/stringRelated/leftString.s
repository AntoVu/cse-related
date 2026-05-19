.global leftString
.text
leftString:
	CMP R2, #0
	BEQ EXIT

LOOP:
	LDRSB R3, [R1], #1
	STRB R3, [R0], #1
	CMP R3, #0
	BEQ EXIT
	SUBS R2, R2, #1
	BNE LOOP
	MOV R3, #0
	STRB R3, [R0]

EXIT:
	BX LR