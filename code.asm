extern printf
extern scanf
global main
section .data
cnst_true db 1
cnst_false db 0
scan_num db "%hi", 0
print_num db "Output: %hi", 10, 0
true_str db "Output: true", 10, 0
false_str db "Output: false", 10, 0
prompt_num db "Input: Enter an integer value", 10, 0
prompt_rnum db "Input: Enter a real value", 10, 0
prompt_bool db "Input: Enter a boolean value", 10, 0
prompt_arr db "Input: Enter %hi array elements of %s type for range %hi to %hi", 10, 0
print_out db "Output: ", 0
print_newline db 10, 0
print_single_num db "%hi ", 0
print_single_rnum db "%f ", 0
print_single_bool db "%s ", 0
int_str db "integer", 0
real_str db "real", 0
bool_str db "boolean", 0
array_index_out_of_bounds db "RUN TIME ERROR:  Array index out of bound", 10, 0


section .text
main:

push rbp
mov rbp, rsp

sub rsp, 86


mov ax, 3
neg ax
mov word[rbp-6], ax

mov ax, word[rbp-6]
mov bx, -3
cmp ax, bx

jl label0

mov ax, word[rbp-6]
mov bx, 2
cmp ax, bx

jg label0

jmp label1

label0:

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, array_index_out_of_bounds
mov rax, 0
call printf
mov rsp, r13
mov rax, 60
mov rdi, 0
syscall

label1:


mov ax, word[rbp-6]
mov bx, -3
sub ax, bx
mov word[rbp-8], ax


mov ax, word[rbp-8]
mov bx, 1
mul bx
mov word[rbp-10], ax

mov ax, word[rbp-10]
mov bx, 0
add ax, bx
mov word[rbp-10], ax



movsx rax, word[rbp-10]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-14], rbx


mov rax, qword[rbp-14]
mov bl, byte[rax]
mov byte[rbp-16], bl

mov rax, qword[rbp-14]
mov bl, byte[cnst_true]
mov byte[rax], bl


mov ax, 2
neg ax
mov word[rbp-17], ax

mov ax, word[rbp-17]
mov bx, -3
cmp ax, bx

jl label2

mov ax, word[rbp-17]
mov bx, 2
cmp ax, bx

jg label2

jmp label3

label2:

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, array_index_out_of_bounds
mov rax, 0
call printf
mov rsp, r13
mov rax, 60
mov rdi, 0
syscall

label3:


mov ax, word[rbp-17]
mov bx, -3
sub ax, bx
mov word[rbp-19], ax


mov ax, word[rbp-19]
mov bx, 1
mul bx
mov word[rbp-21], ax

mov ax, word[rbp-21]
mov bx, 0
add ax, bx
mov word[rbp-21], ax



movsx rax, word[rbp-21]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-25], rbx


mov rax, qword[rbp-25]
mov bl, byte[rax]
mov byte[rbp-27], bl

mov rax, qword[rbp-25]
mov bl, byte[cnst_false]
mov byte[rax], bl

mov ax, 1
mov bx, 2
cmp ax, bx

jl label4


mov byte[rbp-28], 0

jmp label5

label4:

mov byte[rbp-28], 1

label5:


mov ax, 1
neg ax
mov word[rbp-29], ax

mov ax, word[rbp-29]
mov bx, -3
cmp ax, bx

jl label6

mov ax, word[rbp-29]
mov bx, 2
cmp ax, bx

jg label6

jmp label7

label6:

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, array_index_out_of_bounds
mov rax, 0
call printf
mov rsp, r13
mov rax, 60
mov rdi, 0
syscall

label7:


mov ax, word[rbp-29]
mov bx, -3
sub ax, bx
mov word[rbp-31], ax


mov ax, word[rbp-31]
mov bx, 1
mul bx
mov word[rbp-33], ax

mov ax, word[rbp-33]
mov bx, 0
add ax, bx
mov word[rbp-33], ax



movsx rax, word[rbp-33]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-37], rbx


mov rax, qword[rbp-37]
mov bl, byte[rax]
mov byte[rbp-39], bl

mov rax, qword[rbp-37]
mov bl, byte[rbp-28]
mov byte[rax], bl


mov ax, 1
neg ax
mov word[rbp-40], ax

mov ax, word[rbp-40]
mov bx, -3
cmp ax, bx

jl label8

mov ax, word[rbp-40]
mov bx, 2
cmp ax, bx

jg label8

jmp label9

label8:

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, array_index_out_of_bounds
mov rax, 0
call printf
mov rsp, r13
mov rax, 60
mov rdi, 0
syscall

label9:


mov ax, word[rbp-40]
mov bx, -3
sub ax, bx
mov word[rbp-42], ax


mov ax, word[rbp-42]
mov bx, 1
mul bx
mov word[rbp-44], ax

mov ax, word[rbp-44]
mov bx, 0
add ax, bx
mov word[rbp-44], ax



movsx rax, word[rbp-44]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-48], rbx


mov rax, qword[rbp-48]
mov bl, byte[rax]
mov byte[rbp-50], bl

mov r13, rsp
sub rsp, 8
and rsp, -16
mov al, byte[rbp-50]
sub al, 1
jz label16
mov rdi, false_str
jmp label17
label16:
mov rdi, true_str
label17:
mov rax, 0
call printf
mov rsp, r13

mov ax, 0
mov bx, -3
cmp ax, bx

jl label10

mov ax, 0
mov bx, 2
cmp ax, bx

jg label10

jmp label11

label10:

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, array_index_out_of_bounds
mov rax, 0
call printf
mov rsp, r13
mov rax, 60
mov rdi, 0
syscall

label11:


mov ax, 0
mov bx, -3
sub ax, bx
mov word[rbp-51], ax


mov ax, word[rbp-51]
mov bx, 1
mul bx
mov word[rbp-53], ax

mov ax, word[rbp-53]
mov bx, 0
add ax, bx
mov word[rbp-53], ax



movsx rax, word[rbp-53]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-57], rbx


mov rax, qword[rbp-57]
mov bl, byte[rax]
mov byte[rbp-59], bl

mov rax, qword[rbp-57]
mov bl, byte[cnst_false]
mov byte[rax], bl

mov ax, 1
mov bx, -3
cmp ax, bx

jl label12

mov ax, 1
mov bx, 2
cmp ax, bx

jg label12

jmp label13

label12:

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, array_index_out_of_bounds
mov rax, 0
call printf
mov rsp, r13
mov rax, 60
mov rdi, 0
syscall

label13:


mov ax, 1
mov bx, -3
sub ax, bx
mov word[rbp-60], ax


mov ax, word[rbp-60]
mov bx, 1
mul bx
mov word[rbp-62], ax

mov ax, word[rbp-62]
mov bx, 0
add ax, bx
mov word[rbp-62], ax



movsx rax, word[rbp-62]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-66], rbx


mov rax, qword[rbp-66]
mov bl, byte[rax]
mov byte[rbp-68], bl

mov rax, qword[rbp-66]
mov bl, byte[cnst_true]
mov byte[rax], bl

mov ax, 2
mov bx, -3
cmp ax, bx

jl label14

mov ax, 2
mov bx, 2
cmp ax, bx

jg label14

jmp label15

label14:

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, array_index_out_of_bounds
mov rax, 0
call printf
mov rsp, r13
mov rax, 60
mov rdi, 0
syscall

label15:


mov ax, 2
mov bx, -3
sub ax, bx
mov word[rbp-69], ax


mov ax, word[rbp-69]
mov bx, 1
mul bx
mov word[rbp-71], ax

mov ax, word[rbp-71]
mov bx, 0
add ax, bx
mov word[rbp-71], ax



movsx rax, word[rbp-71]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-75], rbx


mov rax, qword[rbp-75]
mov bl, byte[rax]
mov byte[rbp-77], bl

mov rax, qword[rbp-75]
mov bl, byte[cnst_false]
mov byte[rax], bl

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_out
mov rax, 0
call printf
mov rsp, r13





mov r13, rsp
sub rsp, 8
and rsp, -16
mov al, byte[rbp -0]
sub al, 1
jz label18
mov rdi, false_str
jmp label19
label18:
mov rdi, true_str
label19:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov al, byte[rbp -1]
sub al, 1
jz label20
mov rdi, false_str
jmp label21
label20:
mov rdi, true_str
label21:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov al, byte[rbp -2]
sub al, 1
jz label22
mov rdi, false_str
jmp label23
label22:
mov rdi, true_str
label23:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov al, byte[rbp -3]
sub al, 1
jz label24
mov rdi, false_str
jmp label25
label24:
mov rdi, true_str
label25:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov al, byte[rbp -4]
sub al, 1
jz label26
mov rdi, false_str
jmp label27
label26:
mov rdi, true_str
label27:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov al, byte[rbp -5]
sub al, 1
jz label28
mov rdi, false_str
jmp label29
label28:
mov rdi, true_str
label29:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_newline
mov rax, 0
call printf
mov rsp, r13

mov rsp, rbp
pop rbp

mov rax, 60
mov rdi, 0
syscall
