extern printf
extern scanf
global main
section .data
cnst_true dq 1
cnst_false dq 0


section .text
main:

push rbp
mov rbp, rsp

label0:

mov rax, cnst_true
mov rbx, cnst_false
cmp rax, rbx

jz label1

push rbp
mov rbp, rsp

mov rax, 5
mov qword[rbp-0], rax

mov rsp, rbp
pop rbp

jmp label0

label1:

label2:

mov rax, 5
mov rbx, 6
cmp rax, rbx

jl label3

mov qword[rbp-8], 0

jmp label4

label3:

mov qword[rbp-8], 1

label4:

mov rax, qword[rbp-8]
mov rbx, cnst_false
cmp rax, rbx

jz label5

push rbp
mov rbp, rsp

mov rax, 5
mov qword[rbp-0], rax

mov rax, qword[rbp-0]
mov rbx, 1
add rax, rbx
mov qword[rbp-0], rax

mov rax, qword[rbp-0]
mov qword[rbp-0], rax

mov rax, qword[rbp-0]
mov rbx, 1
sub rax, rbx
mov qword[rbp-4], rax

mov rax, qword[rbp-4]
mov qword[rbp-0], rax

mov rsp, rbp
pop rbp

jmp label2

label5:

push rbp
mov rbp, rsp

label6:

mov rax, qword[rbp-8]
mov rbx, qword[rbp-4]
cmp rax, rbx

jge label7

mov rax, 5
mov qword[rbp-12], rax

mov rax, 6
mov qword[rbp-16], rax

mov rax, qword[rbp-12]
mov rbx, qword[rbp-16]
add rax, rbx
mov qword[rbp-24], rax

mov rax, qword[rbp-24]
mov rbx, 5
sub rax, rbx
mov qword[rbp-28], rax

mov rax, qword[rbp-28]
mov qword[rbp-20], rax

dec qword[rbp-8]

jmp label6

label7:

mov rsp, rbp
pop rbp

push rbp
mov rbp, rsp

label8:

mov rax, qword[rbp-8]
mov rbx, qword[rbp-4]
cmp rax, rbx

jle label9

mov rax, 5
mov qword[rbp-12], rax

mov rax, 6
mov qword[rbp-16], rax

mov rax, qword[rbp-12]
mov rbx, qword[rbp-16]
add rax, rbx
mov qword[rbp-28], rax

mov rax, qword[rbp-28]
mov rbx, 5
sub rax, rbx
mov qword[rbp-32], rax

mov rax, qword[rbp-32]
mov qword[rbp-20], rax

mov rax, 3
mov rbx, 5
cmp rax, rbx

jl label10

mov qword[rbp-36], 0

jmp label11

label10:

mov qword[rbp-36], 1

label11:

mov rax, cnst_false
mov rbx, qword[rbp-36]
or rax, rbx
mov qword[rbp-40], rax

mov rax, cnst_true
mov rbx, qword[rbp-40]
and rax, rbx
mov qword[rbp-44], rax

mov rax, qword[rbp-44]
mov qword[rbp-24], rax

mov rax, qword[rbp-24]
mov rbx, cnst_false
cmp rax, rbx

push rbp
mov rbp, rsp

mov rax, 5
mov qword[rbp-0], rax

mov rsp, rbp
pop rbp

jmp label12

label14:

label12:

inc qword[rbp-8]

jmp label8

label9:

mov rsp, rbp
pop rbp

mov rax, 2
mov qword[rbp-4], rax

jmp label15

label15:

mov rsp, rbp
pop rbp

mov rax, 60
mov rdi, 0
syscall
