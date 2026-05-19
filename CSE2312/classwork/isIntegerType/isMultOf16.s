.global isMultOf16
.text
isMultOf16:
	TST R0, #15
	MOV R0, #0
	MOVEQ R0, #1
	BX LR