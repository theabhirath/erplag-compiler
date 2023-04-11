extern printf
extern scanf
global main
section .data
cnst_true dd 1
cnst_false dd 0
scan_num db "%d", 0
print_num db "Output: %d", 10, 0
true_str db "Output: true", 10, 0
false_str db "Output: false", 10, 0
prompt_num db "Input: Enter an integer value", 10, 0
prompt_rnum db "Input: Enter a real value", 10, 0
prompt_bool db "Input: Enter a boolean value", 10, 0
prompt_arr db "Input: Enter %d array elements of %s type for range %d to %d", 10, 0
print_out db "Output: ", 0
print_newline db 10, 0
print_single_num db "%d ", 0
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

sub rsp, 44

mov eax, 19
mov dword[rbp-4], eax

mov eax, 56
mov dword[rbp-8], eax

nop

nop

mov eax, 3
mov dword[rbp-20], eax

mov eax, 6
mov dword[rbp-24], eax

mov eax, dword[rbp-20]
mov dword[rbp-28], eax

label0:

mov eax, dword[rbp-28]
mov ebx, dword[rbp-24]
cmp eax, ebx

jg label1

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
sub rsi, 0
mov rax, 0
call scanf
mov rsp, r13

mov eax, dword[rbp-0]
mov ebx, 1
cmp eax, ebx

jnz label3

nop


mov eax, dword[rbp-0]
mov ebx, 2
mul ebx
mov dword[rbp-36], eax


mov eax, dword[rbp-36]
mov ebx, dword[rbp-4]
sub eax, ebx
mov dword[rbp-40], eax


mov eax, dword[rbp-40]
mov ebx, dword[rbp-28]
add eax, ebx
mov dword[rbp-44], eax

mov eax, dword[rbp-44]
mov dword[rbp-4], eax

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, dword[rbp-4]
mov rax, 0
call printf
mov rsp, r13

nop

jmp label2

label3:

mov eax, dword[rbp-0]
mov ebx, 2
cmp eax, ebx

jnz label4

nop


mov eax, dword[rbp-0]
mov ebx, 3
mul ebx
mov dword[rbp-40], eax


mov eax, dword[rbp-40]
mov ebx, dword[rbp-8]
sub eax, ebx
mov dword[rbp-44], eax


mov eax, dword[rbp-44]
mov ebx, dword[rbp-28]
add eax, ebx
mov dword[rbp-48], eax

mov eax, dword[rbp-48]
mov dword[rbp-8], eax

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, dword[rbp-8]
mov rax, 0
call printf
mov rsp, r13

nop

jmp label2

label4:

nop

nop

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, dword[rbp-0]
mov rax, 0
call printf
mov rsp, r13

nop

jmp label2

label2:

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, dword[rbp-4]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, dword[rbp-8]
mov rax, 0
call printf
mov rsp, r13

inc dword[rbp-28]

jmp label0

label1:

nop

mov rsp, rbp
pop rbp

mov rax, 60
mov rdi, 0
syscall
