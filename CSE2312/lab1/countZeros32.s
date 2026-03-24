.global countZeros32

.text

countZeros32:
	MOV R1, R0 // Move value of R0 into R1
	MOV R0, #0 // Initialize the value of the counter to 0
	MOV R2, #1 // Sort of like a tracker value

START:
	MOVS R1, R1, LSL #1 // Shift the values out one by one
	ADDCC R0, R0, #1 // Add 1 to counter if c flag is clear
	MOVS R2, R2, LSL #1 // Shift the one out slowly
	BNE START // If z flag is not set (the 1 in R2 not shifted out) repeat
	BX LR // if z flag is set, return value
