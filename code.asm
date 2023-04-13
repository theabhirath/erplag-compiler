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

sub rsp, 49

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
mov rdi, prompt_arr
mov rsi, 6
mov rdx, int_str
mov rcx, -3
mov r8, 2
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
mov rdi, scan_num
mov rsi, rbp
sub rsi, 16
mov rax, 0
call scanf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, scan_num
mov rsi, rbp
sub rsi, 18
mov rax, 0
call scanf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, scan_num
mov rsi, rbp
sub rsi, 20
mov rax, 0
call scanf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, scan_num
mov rsi, rbp
sub rsi, 22
mov rax, 0
call scanf
mov rsp, r13

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
mov rdi, print_out
mov rax, 0
call printf
mov rsp, r13





mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -12]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -14]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -16]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -18]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -20]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -22]
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


mov ax, 2
mov bx, 3
mul bx
mov word[rbp-34], ax


mov ax, word[rbp-34]
neg ax
mov word[rbp-36], ax

mov ax, word[rbp-36]
mov word[rbp-2], ax


mov ax, 2
mov bx, 3
mul bx
mov word[rbp-38], ax

mov ax, word[rbp-38]
mov word[rbp-4], ax


mov ax, 3
neg ax
mov word[rbp-40], ax


mov ax, 2
mov bx, word[rbp-40]
mul bx
mov word[rbp-42], ax

mov ax, word[rbp-42]
mov word[rbp-6], ax


mov ax, 3
neg ax
mov word[rbp-44], ax


mov ax, 2
mov bx, word[rbp-44]
mul bx
mov word[rbp-46], ax


mov ax, word[rbp-46]
neg ax
mov word[rbp-48], ax

mov ax, word[rbp-48]
mov word[rbp-8], ax

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
mov rdi, print_num
movsx rsi, word[rbp-8]
mov rax, 0
call printf
mov rsp, r13

label0:

mov ax, word[rbp-2]
mov bx, 0
cmp ax, bx

jg label1


mov byte[rbp-49], 0

jmp label2

label1:

mov byte[rbp-49], 1

label2:

mov al, byte[rbp-49]
mov bl, byte[cnst_false]
cmp al, bl

jz label3

nop

nop


mov ax, word[rbp-2]
mov bx, 1
sub ax, bx
mov word[rbp-26], ax

mov ax, word[rbp-26]
mov word[rbp-2], ax

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-2]
mov rax, 0
call printf
mov rsp, r13

nop

jmp label0

label3:

mov rsp, rbp
pop rbp
push rbp

mov rax, 60
mov rdi, 0
syscall
