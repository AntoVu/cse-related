.global binaryToUint32
.text
binaryToUint32:
	MOV R1, R0
	MOV R0, #0
	MOV R2, #0x80000000
LOOP:
	LDRSB R3, [R1], #1
	CMP R3, #0
	BEQ END
	CMP R3, #'1'
	ORREQ R0, R0, R2
	MOVS R2, R2, LSR #1
	BNE LOOP
END:
	BX LR