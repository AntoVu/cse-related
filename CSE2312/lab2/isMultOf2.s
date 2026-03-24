.global isMultOf2
.text
isMultOf2: // The file name was isMultOf2 in the c function instead of is_mult_of_2
	TST R0, #1
	MOV R0, #0
	MOVEQ R0, #1
	BX LR
