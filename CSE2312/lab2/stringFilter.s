.global stringFilter
.text
stringFilter:
	LDRSB R2, [R1], #1
	CMP R2, #0
	BEQ END
	CMP R2, #'0'
	BLT stringFilter
	CMP R2, #'9'
	BGT stringFilter
	STRB R2, [R0], #1
	BAL stringFilter
END:
	MOV R2, #0
	STRB R2, [R0]
	BX LR
