.global isStrEqual
.text
isStrEqual:
	PUSH {R4}
	MOV R2, R0
	MOV R0, #0
LOOP:
	LDRSB R3, [R2], #1
	LDRSB R4, [R1], #1
	CMP R3, R4
	BNE END
	CMP R3, #0
	MOVEQ R0, #1
	BNE LOOP
END:
	POP {R4}
	BX LR