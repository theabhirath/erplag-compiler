extern printf
extern scanf
global main
section .data
cnst_true dq 1
cnst_false dq 0


 section .text
main:

label0:

mov rax, cnst_true
mov rbx, cnst_false
cmp rax, rbx

jz label1

mov rax, 5
mov a, rax

jmp label0

label1:

label2:

mov rax, 5
mov rbx, 6
cmp rax, rbx

jl label3

mov $temp_var0, 0

jmp label4

label3:

mov $temp_var0, 1

label4:

mov rax, $temp_var0
mov rbx, cnst_false
cmp rax, rbx

jz label5

mov rax, 5
mov x, rax

mov rax, x
mov rbx, 1
add rax, rbx
mov $temp_var1, rax

mov rax, $temp_var1
mov x, rax

mov rax, x
mov rbx, 1
sub rax, rbx
mov $temp_var2, rax

mov rax, $temp_var2
mov x, rax

jmp label2

label5:

label6:

mov rax, i
mov rbx, $end
cmp rax, rbx

jge label7

mov rax, 5
mov a, rax

mov rax, 6
mov b, rax

mov rax, a
mov rbx, b
add rax, rbx
mov $temp_var3, rax

mov rax, $temp_var3
mov rbx, 5
sub rax, rbx
mov $temp_var4, rax

mov rax, $temp_var4
mov c, rax

dec i

jmp label6

label7:

label8:

mov rax, i
mov rbx, $end
cmp rax, rbx

jle label9

mov rax, 5
mov a, rax

mov rax, 6
mov b, rax

mov rax, a
mov rbx, b
add rax, rbx
mov $temp_var5, rax

mov rax, $temp_var5
mov rbx, 5
sub rax, rbx
mov $temp_var6, rax

mov rax, $temp_var6
mov c, rax

mov rax, 3
mov rbx, 5
cmp rax, rbx

jl label10

mov $temp_var7, 0

jmp label11

label10:

mov $temp_var7, 1

label11:

mov rax, cnst_false
mov rbx, $temp_var7
or rax, rbx
mov $temp_var8, rax

mov rax, cnst_true
mov rbx, $temp_var8
and rax, rbx
mov $temp_var9, rax

mov rax, $temp_var9
mov bolle, rax

mov rax, bolle
mov rbx, cnst_false
cmp rax, rbx

mov rax, 5
mov a, rax

jmp label12

label14:

label12:

inc i

jmp label8

label9:

mov rax, 2
mov a, rax

jmp label15

label15:

mov rax, 60
mov rdi, 0
syscall
