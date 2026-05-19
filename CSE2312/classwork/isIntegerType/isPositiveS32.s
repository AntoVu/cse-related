.global isPositiveS32
.text
isPositiveS32:
	CMP R0, #0
	MOV R0, #0
	MOVPL R0, #1
	MOVEQ R0, #0
	BX LR