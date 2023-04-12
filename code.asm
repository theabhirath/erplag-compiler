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

push rbp
mov rbp, rsp

sub rsp, 174


mov ax, 3
neg ax
mov word[rbp-26], ax

mov ax, word[rbp-26]
mov bx, -3
cmp ax, bx

jl label0

mov ax, word[rbp-26]
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


mov ax, word[rbp-26]
mov bx, -3
sub ax, bx
mov word[rbp-28], ax


mov ax, word[rbp-28]
mov bx, 1
mul bx
mov word[rbp-30], ax

mov ax, word[rbp-30]
mov bx, 0
add ax, bx
mov word[rbp-30], ax



movsx rax, word[rbp-30]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-34], rbx


mov rax, qword[rbp-34]
mov bl, byte[rax]
mov byte[rbp-36], bl

mov rax, qword[rbp-34]
mov bl, byte[cnst_false]
mov byte[rax], bl

mov ax, 1
mov bx, 2
cmp ax, bx

jl label2


mov byte[rbp-37], 0

jmp label3

label2:

mov byte[rbp-37], 1

label3:


mov ax, 2
neg ax
mov word[rbp-38], ax

mov ax, word[rbp-38]
mov bx, -3
cmp ax, bx

jl label4

mov ax, word[rbp-38]
mov bx, 2
cmp ax, bx

jg label4

jmp label5

label4:

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

label5:


mov ax, word[rbp-38]
mov bx, -3
sub ax, bx
mov word[rbp-40], ax


mov ax, word[rbp-40]
mov bx, 1
mul bx
mov word[rbp-42], ax

mov ax, word[rbp-42]
mov bx, 0
add ax, bx
mov word[rbp-42], ax



movsx rax, word[rbp-42]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-46], rbx


mov rax, qword[rbp-46]
mov bl, byte[rax]
mov byte[rbp-48], bl

mov rax, qword[rbp-46]
mov bl, byte[rbp-37]
mov byte[rax], bl


mov ax, 1
neg ax
mov word[rbp-50], ax

mov ax, word[rbp-50]
mov bx, -3
cmp ax, bx

jl label6

mov ax, word[rbp-50]
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


mov ax, word[rbp-50]
mov bx, -3
sub ax, bx
mov word[rbp-52], ax


mov ax, word[rbp-52]
mov bx, 1
mul bx
mov word[rbp-54], ax

mov ax, word[rbp-54]
mov bx, 0
add ax, bx
mov word[rbp-54], ax



movsx rax, word[rbp-54]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-58], rbx


mov rax, qword[rbp-58]
mov bl, byte[rax]
mov byte[rbp-60], bl

mov rax, qword[rbp-58]
mov bl, byte[cnst_false]
mov byte[rax], bl

mov ax, 0
mov bx, -3
cmp ax, bx

jl label8

mov ax, 0
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


mov ax, 0
mov bx, -3
sub ax, bx
mov word[rbp-62], ax


mov ax, word[rbp-62]
mov bx, 1
mul bx
mov word[rbp-64], ax

mov ax, word[rbp-64]
mov bx, 0
add ax, bx
mov word[rbp-64], ax



movsx rax, word[rbp-64]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-68], rbx


mov rax, qword[rbp-68]
mov bl, byte[rax]
mov byte[rbp-70], bl

mov rax, qword[rbp-68]
mov bl, byte[cnst_false]
mov byte[rax], bl

mov ax, 1
mov bx, -3
cmp ax, bx

jl label10

mov ax, 1
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


mov ax, 1
mov bx, -3
sub ax, bx
mov word[rbp-72], ax


mov ax, word[rbp-72]
mov bx, 1
mul bx
mov word[rbp-74], ax

mov ax, word[rbp-74]
mov bx, 0
add ax, bx
mov word[rbp-74], ax



movsx rax, word[rbp-74]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-78], rbx


mov rax, qword[rbp-78]
mov bl, byte[rax]
mov byte[rbp-80], bl

mov rax, qword[rbp-78]
mov bl, byte[cnst_false]
mov byte[rax], bl

mov ax, 2
mov bx, -3
cmp ax, bx

jl label12

mov ax, 2
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


mov ax, 2
mov bx, -3
sub ax, bx
mov word[rbp-82], ax


mov ax, word[rbp-82]
mov bx, 1
mul bx
mov word[rbp-84], ax

mov ax, word[rbp-84]
mov bx, 0
add ax, bx
mov word[rbp-84], ax



movsx rax, word[rbp-84]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-88], rbx


mov rax, qword[rbp-88]
mov bl, byte[rax]
mov byte[rbp-90], bl

mov rax, qword[rbp-88]
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
mov rdi, print_single_bool
mov al, byte[rbp -0]
sub al, 1
jz label26
mov rsi, false_str
jmp label27
label26:
mov rsi, true_str
label27:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_bool
mov al, byte[rbp -1]
sub al, 1
jz label28
mov rsi, false_str
jmp label29
label28:
mov rsi, true_str
label29:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_bool
mov al, byte[rbp -2]
sub al, 1
jz label30
mov rsi, false_str
jmp label31
label30:
mov rsi, true_str
label31:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_bool
mov al, byte[rbp -3]
sub al, 1
jz label32
mov rsi, false_str
jmp label33
label32:
mov rsi, true_str
label33:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_bool
mov al, byte[rbp -4]
sub al, 1
jz label34
mov rsi, false_str
jmp label35
label34:
mov rsi, true_str
label35:
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_bool
mov al, byte[rbp -5]
sub al, 1
jz label36
mov rsi, false_str
jmp label37
label36:
mov rsi, true_str
label37:
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

mov al, byte[cnst_true]
mov byte[rbp-6], al

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_bool
mov al, byte[rbp-6]
sub al, 1
jz label38
mov rsi, false_str
jmp label39
label38:
mov rsi, true_str
label39:
mov rax, 0
call printf
mov rsp, r13

mov ax, 5
mov word[rbp-7], ax

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-7]
mov rax, 0
call printf
mov rsp, r13


mov ax, 3
neg ax
mov word[rbp-100], ax

mov ax, word[rbp-100]
mov bx, -3
cmp ax, bx

jl label14

mov ax, word[rbp-100]
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


mov ax, word[rbp-100]
mov bx, -3
sub ax, bx
mov word[rbp-102], ax


mov ax, word[rbp-102]
mov bx, 2
mul bx
mov word[rbp-104], ax

mov ax, word[rbp-104]
mov bx, 9
add ax, bx
mov word[rbp-104], ax



movsx rax, word[rbp-104]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-108], rbx


mov rax, qword[rbp-108]
mov bx, word[rax]
mov word[rbp-110], bx

mov rax, qword[rbp-108]
mov bx, 1
mov word[rax], bx


mov ax, 2
neg ax
mov word[rbp-112], ax

mov ax, word[rbp-112]
mov bx, -3
cmp ax, bx

jl label16

mov ax, word[rbp-112]
mov bx, 2
cmp ax, bx

jg label16

jmp label17

label16:

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

label17:


mov ax, word[rbp-112]
mov bx, -3
sub ax, bx
mov word[rbp-114], ax


mov ax, word[rbp-114]
mov bx, 2
mul bx
mov word[rbp-116], ax

mov ax, word[rbp-116]
mov bx, 9
add ax, bx
mov word[rbp-116], ax



movsx rax, word[rbp-116]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-120], rbx


mov rax, qword[rbp-120]
mov bx, word[rax]
mov word[rbp-122], bx

mov rax, qword[rbp-120]
mov bx, 2
mov word[rax], bx


mov ax, 1
neg ax
mov word[rbp-124], ax

mov ax, word[rbp-124]
mov bx, -3
cmp ax, bx

jl label18

mov ax, word[rbp-124]
mov bx, 2
cmp ax, bx

jg label18

jmp label19

label18:

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

label19:


mov ax, word[rbp-124]
mov bx, -3
sub ax, bx
mov word[rbp-126], ax


mov ax, word[rbp-126]
mov bx, 2
mul bx
mov word[rbp-128], ax

mov ax, word[rbp-128]
mov bx, 9
add ax, bx
mov word[rbp-128], ax



movsx rax, word[rbp-128]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-132], rbx


mov rax, qword[rbp-132]
mov bx, word[rax]
mov word[rbp-134], bx

mov rax, qword[rbp-132]
mov bx, 3
mov word[rax], bx

mov ax, 0
mov bx, -3
cmp ax, bx

jl label20

mov ax, 0
mov bx, 2
cmp ax, bx

jg label20

jmp label21

label20:

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

label21:


mov ax, 0
mov bx, -3
sub ax, bx
mov word[rbp-136], ax


mov ax, word[rbp-136]
mov bx, 2
mul bx
mov word[rbp-138], ax

mov ax, word[rbp-138]
mov bx, 9
add ax, bx
mov word[rbp-138], ax



movsx rax, word[rbp-138]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-142], rbx


mov rax, qword[rbp-142]
mov bx, word[rax]
mov word[rbp-144], bx

mov rax, qword[rbp-142]
mov bx, 4
mov word[rax], bx

mov ax, 1
mov bx, -3
cmp ax, bx

jl label22

mov ax, 1
mov bx, 2
cmp ax, bx

jg label22

jmp label23

label22:

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

label23:


mov ax, 1
mov bx, -3
sub ax, bx
mov word[rbp-146], ax


mov ax, word[rbp-146]
mov bx, 2
mul bx
mov word[rbp-148], ax

mov ax, word[rbp-148]
mov bx, 9
add ax, bx
mov word[rbp-148], ax



movsx rax, word[rbp-148]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-152], rbx


mov rax, qword[rbp-152]
mov bx, word[rax]
mov word[rbp-154], bx

mov rax, qword[rbp-152]
mov bx, 5
mov word[rax], bx

mov ax, 2
mov bx, -3
cmp ax, bx

jl label24

mov ax, 2
mov bx, 2
cmp ax, bx

jg label24

jmp label25

label24:

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

label25:


mov ax, 2
mov bx, -3
sub ax, bx
mov word[rbp-156], ax


mov ax, word[rbp-156]
mov bx, 2
mul bx
mov word[rbp-158], ax

mov ax, word[rbp-158]
mov bx, 9
add ax, bx
mov word[rbp-158], ax



movsx rax, word[rbp-158]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-162], rbx


mov rax, qword[rbp-162]
mov bx, word[rax]
mov word[rbp-164], bx

mov rax, qword[rbp-162]
mov bx, 6
mov word[rax], bx

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
movsx rsi, word[rbp -9]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -11]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -13]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -15]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -17]
mov rax, 0
call printf
mov rsp, r13

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_single_num
movsx rsi, word[rbp -19]
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
