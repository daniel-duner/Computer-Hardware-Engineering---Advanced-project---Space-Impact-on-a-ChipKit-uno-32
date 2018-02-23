 # labwork.S

 # Written 2015-2017 by F Lundevall

 # Skeleton file for IS1200/IS1500 lab 1.

 # The macros PUSH and POP are in the public domain.

 # Please add your own code at the end of the file.


 # Please keep the two macros PUSH and POP unchanged


.macro	PUSH reg

addi	$sp,$sp,-4

sw	\reg,0 ($sp)

.endm

.macro POP reg

lw	\reg,0 ($sp)

addi	$sp,$sp,4

.endm

.global time2string
.global delay
.global enable_interrupt

 # Please add your own code below this line
  #
enable_interrupt:
ei
jr $ra
nop


##------------------------------------------
hexasc:
li 	$t3,0x0000000f
and 	$t3,$a0,$t3
ble 	$t3,9, start
nop
addi 	$v0,$t3,0x37
j done
nop
start:
addi 	$v0,$t3,0x30

done:
jr $ra
nop
##----------------------------------------
delay:
li $t4,-1 	#i �r $t4
loop:
ble $a0,0,endloop
nop
addi $a0,$a0,-1
do:
addi $t4,$t4,1
ble $t4,0x450000, do
nop
j loop
nop
endloop:
 jr $ra
 nop
 ##---------------------------------------
time2string:
PUSH $ra
PUSH $v0
PUSH $a0


srl $a0,$a1,12

jal hexasc
nop
POP $a0
sb 	$v0, 0 ($a0)

PUSH $a0
srl $a0,$a1,8
jal hexasc
nop
POP $a0
sb 	$v0, 1 ($a0)

li $t0,0x3a
sb $t0,2 ($a0)

PUSH $a0
srl $a0,$a1,4
jal hexasc
nop
POP $a0
sb 	$v0, 3 ($a0)

PUSH $a0
addi $a0,$a1,0
jal hexasc
nop
POP $a0
sb 	$v0, 4 ($a0)


andi $t6,$a1,0xff
beqz $t6,addx
nop

sb 	$0, 5 ($a0)
back:
POP $v0
POP $ra
jr $ra
nop

addx:
li $t7,0x58
sb $t7, 5 ($a0)
sb 	$0, 6 ($a0)
j back
nop
