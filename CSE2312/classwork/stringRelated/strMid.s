.global strMid
.text
strMid:
	PUSH {R4}
	CMP R3, #0
	BEQ END
	CMP R0, #0
	BEQ END
	ADD R0, R0, R2
LOOP:
	LDRSB R4, [R0], #1
	STRB R4, [R1], #1
	CMP R4, #0
	BEQ END
	SUBS R3, R3, #1
	BNE LOOP
	MOV R4, #0
	STRB R4, [R1]
END:
	POP {R4}
	BX LR