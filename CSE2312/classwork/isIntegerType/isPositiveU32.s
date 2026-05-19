.global isPositiveU32
.text
isPositiveU32:
	CMP R0, #0
	MOV R0, #1
	MOVEQ, R0, #0
	BX LR