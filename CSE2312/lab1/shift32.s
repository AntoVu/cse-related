.global lsl32
.global lsr32
.global asl32
.global asr32

.text

lsl32:
	MOV R0, R0, LSL R1
	BX LR
	
lsr32:
	MOV R0, R0, LSR R1
	BX LR

asl32:
	MOV R0, R0, ASL R1
	BX LR
	
asr32:
	MOV R0, R0, ASR R1
	BX LR
