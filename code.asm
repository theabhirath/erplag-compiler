extern printf
extern scanf
global main
section .data
cnst_true db 1
cnst_false db 0
scan_num db "%hi", 0
print_num db "Output: %hi", 10, 0
print_bool db "Output: %s", 10, 0
print_rnum db "Output: %f", 10, 0
true_str db "true ", 0
false_str db "false ", 0
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

mov rbp, rsp

sub rsp, 8

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, prompt_num
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, scan_num
mov rsi, rbp
sub rsi, 2
mov rax, 0
call scanf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, prompt_num
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, scan_num
mov rsi, rbp
sub rsi, 4
mov rax, 0
call scanf
mov rsp, r13

mov al, byte[cnst_false]
mov byte[rbp-8], al

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-2]
mov rax, 0
call printf
mov rsp, r13

push rbp

mov ax, word[rbp-2]
mov word[rsp-2], ax

mov ax, word[rbp-4]
mov word[rsp-4], ax

mov al, byte[rbp-8]
mov byte[rsp-5], al

mov rbp, rsp

sub rsp, 8

sub rsp, 29

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, prompt_num
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, scan_num
mov rsi, rbp
sub rsi, 14
mov rax, 0
call scanf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, prompt_num
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, scan_num
mov rsi, rbp
sub rsi, 12
mov rax, 0
call scanf
mov rsp, r13


mov ax, word[rbp-2]
mov bx, word[rbp-4]
mul bx
mov word[rbp-17], ax


mov ax, word[rbp-14]
mov bx, 2
mul bx
mov word[rbp-19], ax


mov ax, word[rbp-17]
mov bx, word[rbp-19]
add ax, bx
mov word[rbp-21], ax


mov ax, word[rbp-21]
mov bx, 3
sub ax, bx
mov word[rbp-23], ax

mov ax, word[rbp-23]
mov word[rbp-10], ax

mov ax, word[rbp-2]
mov bx, word[rbp-4]
cmp ax, bx

jle label0


mov byte[rbp-24], 0

jmp label1

label0:

mov byte[rbp-24], 1

label1:


mov al, byte[rbp-5]
mov bl, byte[rbp-24]
and al, bl
mov byte[rbp-25], al

mov al, byte[rbp-25]
mov byte[rbp-15], al


mov ax, word[rbp-10]
mov bx, word[rbp-12]
add ax, bx
mov word[rbp-27], ax

mov ax, word[rbp-27]
mov word[rbp-7], ax

mov ax, word[rbp-10]
mov bx, word[rbp-12]
cmp ax, bx

jle label2


mov byte[rbp-28], 0

jmp label3

label2:

mov byte[rbp-28], 1

label3:


mov al, byte[rbp-15]
mov bl, byte[rbp-28]
or al, bl
mov byte[rbp-29], al

mov al, byte[rbp-29]
mov byte[rbp-8], al

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-2]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-4]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_bool
mov al, byte[rbp-5]
sub al, 1
jz label4
mov rsi, false_str
jmp label5
label4:
mov rsi, true_str
label5:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-14]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-10]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-12]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-7]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_bool
mov al, byte[rbp-8]
sub al, 1
jz label6
mov rsi, false_str
jmp label7
label6:
mov rsi, true_str
label7:
mov rax, 0
call printf
mov rsp, r13

mov rsp, rbp
pop rbp
push rbp

mov ax, word[rsp-7]
mov word[rbp-6], ax

mov al, byte[rsp-8]
mov byte[rbp-7], al

pop rbp

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-2]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-4]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-6]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_bool
mov al, byte[rbp-7]
sub al, 1
jz label8
mov rsi, false_str
jmp label9
label8:
mov rsi, true_str
label9:
mov rax, 0
call printf
mov rsp, r13

mov rsp, rbp
pop rbp
push rbp

mov rax, 60
mov rdi, 0
syscall
