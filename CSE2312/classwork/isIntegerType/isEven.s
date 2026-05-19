.global isEven
.text
isEven:
	TST R0, #1
	MOV R0, #0
	MOVEQ R0, #1
	BX LR