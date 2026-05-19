.global strCat
.text
strCat:
	LDRSB R2, [R0], #1
	CMP R2, #0
	BNE strCat
	SUB R0, R0, #1
LOOP:
	LDRSB R2, [R1], #1
	STRB R2, [R0], #1
	CMP R2, #0
	BNE LOOP
	BX LR