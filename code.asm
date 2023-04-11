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

sub rsp, 124


mov eax, 2
neg eax
mov dword[rbp-48], eax


mov eax, dword[rbp-48]
mov ebx, -3
sub eax, ebx
mov dword[rbp-52], eax


mov eax, dword[rbp-52]
mov ebx, 4
mul ebx
mov dword[rbp-56], eax

mov eax, dword[rbp-56]
mov ebx, dword[rbp-24]
add eax, ebx
mov dword[rbp-56], eax



movsx rax, dword[rbp-56]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-64], rbx

mov rax, qword[rbp-64]
mov ebx, dword[rax]
mov dword[rbp-64], ebx

mov eax, 5
mov dword[rbp-64], eax


mov eax, 2
neg eax
mov dword[rbp-68], eax


mov eax, dword[rbp-68]
mov ebx, -3
sub eax, ebx
mov dword[rbp-72], eax


mov eax, dword[rbp-72]
mov ebx, 4
mul ebx
mov dword[rbp-76], eax

mov eax, dword[rbp-76]
mov ebx, dword[rbp-24]
add eax, ebx
mov dword[rbp-76], eax



movsx rax, dword[rbp-76]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-84], rbx

mov rax, qword[rbp-84]
mov ebx, dword[rax]
mov dword[rbp-84], ebx

mov eax, dword[rbp-84]
mov dword[rbp-12], eax

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, dword[rbp-12]
mov rax, 0
call printf
mov rsp, r13

mov eax, 9
mov dword[rbp-16], eax

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
sub rsi, 4
mov rax, 0
call scanf
mov rsp, r13


mov eax, dword[rbp-4]
mov ebx, dword[rbp-16]
mul ebx
mov dword[rbp-88], eax


mov eax, dword[rbp-0]
mov ebx, dword[rbp-88]
add eax, ebx
mov dword[rbp-92], eax


mov eax, dword[rbp-12]
mov ebx, dword[rbp-16]
sub eax, ebx
mov dword[rbp-96], eax


mov eax, dword[rbp-96]
mov ebx, dword[rbp-4]
mul ebx
mov dword[rbp-100], eax


mov eax, dword[rbp-92]
mov ebx, dword[rbp-100]
add eax, ebx
mov dword[rbp-104], eax


mov eax, dword[rbp-12]
mov ebx, 2
mul ebx
mov dword[rbp-108], eax


mov eax, dword[rbp-104]
mov ebx, dword[rbp-108]
add eax, ebx
mov dword[rbp-112], eax


mov eax, dword[rbp-16]
mov ebx, dword[rbp-0]
mul ebx
mov dword[rbp-116], eax


mov eax, dword[rbp-112]
mov ebx, dword[rbp-116]
sub eax, ebx
mov dword[rbp-120], eax

mov eax, dword[rbp-120]
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

mov rax, 60
mov rdi, 0
syscall
