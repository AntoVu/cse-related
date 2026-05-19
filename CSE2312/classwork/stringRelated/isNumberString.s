.global isNumberString
.text
isNumberString:
	MOV R1, R0
	MOV R0, #1
	LDRSB R2, [R0]
	CMP R2, #0
	MOVEQ R0, #0
	BEQ END
LOOP:
	LDRSB R2, [R1], #1
	CMP R2, #0
	BEQ END
	CMP R2, #'0'
	MOVLT R0, #0
	BLT END
	CMP R2, #'9'
	MOVGT R0, #0
	BGT END
	BAL LOOP
END:
	BX LR