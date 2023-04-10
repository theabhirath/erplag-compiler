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


section .text
main:

push rbp
mov rbp, rsp

sub rsp, 16

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, prompt_num
mov rax, 0
call printf
sub rsp, 8
and rsp, -16
mov rdi, scan_num
mov rsi, rbp
sub rsi, 0
mov rax, 0
call scanf
mov rsp, r13

mov eax, 19
mov dword[rbp-4], eax

mov eax, 56
mov dword[rbp-8], eax

mov eax, dword[rbp-0]
mov ebx, 1
cmp eax, ebx

jnz label1

push rbp
mov rbp, rsp


mov eax, 10
neg eax
mov dword[rbp-8], eax

mov eax, dword[rbp-8]
mov dword[rbp-4], eax


mov eax, dword[rbp-0]
mov ebx, 2
mul ebx
mov dword[rbp-12], eax


mov eax, dword[rbp-12]
mov ebx, dword[rbp-4]
sub eax, ebx
mov dword[rbp-16], eax


mov eax, dword[rbp-4]
mov ebx, 3
mul ebx
mov dword[rbp-20], eax


mov eax, dword[rbp-16]
mov ebx, dword[rbp-20]
add eax, ebx
mov dword[rbp-24], eax

mov eax, dword[rbp-24]
mov dword[rbp-4], eax

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, dword[rbp-4]
mov rax, 0
call printf
mov rsp, r13

mov rsp, rbp
pop rbp

jmp label0

label1:

mov eax, dword[rbp-0]
mov ebx, 2
cmp eax, ebx

jnz label2

push rbp
mov rbp, rsp

mov eax, 6
mov dword[rbp-4], eax


mov eax, dword[rbp-4]
mov ebx, 3
mul ebx
mov dword[rbp-8], eax


mov eax, dword[rbp-8]
mov ebx, dword[rbp-8]
sub eax, ebx
mov dword[rbp-12], eax

mov eax, dword[rbp-12]
mov dword[rbp-8], eax

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, dword[rbp-8]
mov rax, 0
call printf
mov rsp, r13

mov rsp, rbp
pop rbp

jmp label0

label2:

push rbp
mov rbp, rsp

sub rsp, 4

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, dword[rbp-0]
mov rax, 0
call printf
mov rsp, r13

mov rsp, rbp
pop rbp

jmp label0

label0:

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
movsx rsi, dword[rbp-0]
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

mov rsp, rbp
pop rbp

mov rax, 60
mov rdi, 0
syscall
