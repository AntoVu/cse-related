.global strConcatenate
.text
strConcatenate:
	LDRSB R2, [R0], #1
	CMP R2, #0
	BNE strConcatenate
	SUB R0, R0, #1
LOOP:
	LDRSB R2, [R1], #1
	STRB R2, [R0], #1
	CMP R2, #0
	BNE LOOP
	BX LR