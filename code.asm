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

sub rsp, 168


mov ax, 3
neg ax
mov word[rbp-27], ax

mov ax, word[rbp-27]
mov bx, -3
cmp ax, bx

jl label0

mov ax, word[rbp-27]
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


mov ax, word[rbp-27]
mov bx, -3
sub ax, bx
mov word[rbp-29], ax


mov ax, word[rbp-29]
mov bx, 1
mul bx
mov word[rbp-31], ax

mov ax, word[rbp-31]
mov bx, 0
add ax, bx
mov word[rbp-31], ax



movsx rax, word[rbp-31]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-35], rbx


mov rax, qword[rbp-35]
mov bl, byte[rax]
mov byte[rbp-36], bl

mov rax, qword[rbp-35]
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
mov word[rbp-39], ax

mov ax, word[rbp-39]
mov bx, -3
cmp ax, bx

jl label4

mov ax, word[rbp-39]
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


mov ax, word[rbp-39]
mov bx, -3
sub ax, bx
mov word[rbp-41], ax


mov ax, word[rbp-41]
mov bx, 1
mul bx
mov word[rbp-43], ax

mov ax, word[rbp-43]
mov bx, 0
add ax, bx
mov word[rbp-43], ax



movsx rax, word[rbp-43]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-47], rbx


mov rax, qword[rbp-47]
mov bl, byte[rax]
mov byte[rbp-48], bl

mov rax, qword[rbp-47]
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
mov byte[rbp-59], bl

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
mov word[rbp-61], ax


mov ax, word[rbp-61]
mov bx, 1
mul bx
mov word[rbp-63], ax

mov ax, word[rbp-63]
mov bx, 0
add ax, bx
mov word[rbp-63], ax



movsx rax, word[rbp-63]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-67], rbx


mov rax, qword[rbp-67]
mov bl, byte[rax]
mov byte[rbp-68], bl

mov rax, qword[rbp-67]
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
mov word[rbp-70], ax


mov ax, word[rbp-70]
mov bx, 1
mul bx
mov word[rbp-72], ax

mov ax, word[rbp-72]
mov bx, 0
add ax, bx
mov word[rbp-72], ax



movsx rax, word[rbp-72]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-76], rbx


mov rax, qword[rbp-76]
mov bl, byte[rax]
mov byte[rbp-77], bl

mov rax, qword[rbp-76]
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
mov word[rbp-79], ax


mov ax, word[rbp-79]
mov bx, 1
mul bx
mov word[rbp-81], ax

mov ax, word[rbp-81]
mov bx, 0
add ax, bx
mov word[rbp-81], ax



movsx rax, word[rbp-81]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-85], rbx


mov rax, qword[rbp-85]
mov bl, byte[rax]
mov byte[rbp-86], bl

mov rax, qword[rbp-85]
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
mov byte[rbp-7], al

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_bool
mov al, byte[rbp-7]
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
mov word[rbp-9], ax

mov r13, rsp
sub rsp, 8
and rsp, -16
mov rdi, print_num
movsx rsi, word[rbp-9]
mov rax, 0
call printf
mov rsp, r13


mov ax, 3
neg ax
mov word[rbp-96], ax

mov ax, word[rbp-96]
mov bx, -3
cmp ax, bx

jl label14

mov ax, word[rbp-96]
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


mov ax, word[rbp-96]
mov bx, -3
sub ax, bx
mov word[rbp-98], ax


mov ax, word[rbp-98]
mov bx, 2
mul bx
mov word[rbp-100], ax

mov ax, word[rbp-100]
mov bx, 9
add ax, bx
mov word[rbp-100], ax



movsx rax, word[rbp-100]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-104], rbx


mov rax, qword[rbp-104]
mov bx, word[rax]
mov word[rbp-106], bx

mov rax, qword[rbp-104]
mov bx, 1
mov word[rax], bx


mov ax, 2
neg ax
mov word[rbp-108], ax

mov ax, word[rbp-108]
mov bx, -3
cmp ax, bx

jl label16

mov ax, word[rbp-108]
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


mov ax, word[rbp-108]
mov bx, -3
sub ax, bx
mov word[rbp-110], ax


mov ax, word[rbp-110]
mov bx, 2
mul bx
mov word[rbp-112], ax

mov ax, word[rbp-112]
mov bx, 9
add ax, bx
mov word[rbp-112], ax



movsx rax, word[rbp-112]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-116], rbx


mov rax, qword[rbp-116]
mov bx, word[rax]
mov word[rbp-118], bx

mov rax, qword[rbp-116]
mov bx, 2
mov word[rax], bx


mov ax, 1
neg ax
mov word[rbp-120], ax

mov ax, word[rbp-120]
mov bx, -3
cmp ax, bx

jl label18

mov ax, word[rbp-120]
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


mov ax, word[rbp-120]
mov bx, -3
sub ax, bx
mov word[rbp-122], ax


mov ax, word[rbp-122]
mov bx, 2
mul bx
mov word[rbp-124], ax

mov ax, word[rbp-124]
mov bx, 9
add ax, bx
mov word[rbp-124], ax



movsx rax, word[rbp-124]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-128], rbx


mov rax, qword[rbp-128]
mov bx, word[rax]
mov word[rbp-130], bx

mov rax, qword[rbp-128]
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
mov word[rbp-132], ax


mov ax, word[rbp-132]
mov bx, 2
mul bx
mov word[rbp-134], ax

mov ax, word[rbp-134]
mov bx, 9
add ax, bx
mov word[rbp-134], ax



movsx rax, word[rbp-134]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-138], rbx


mov rax, qword[rbp-138]
mov bx, word[rax]
mov word[rbp-140], bx

mov rax, qword[rbp-138]
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
mov word[rbp-142], ax


mov ax, word[rbp-142]
mov bx, 2
mul bx
mov word[rbp-144], ax

mov ax, word[rbp-144]
mov bx, 9
add ax, bx
mov word[rbp-144], ax



movsx rax, word[rbp-144]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-148], rbx


mov rax, qword[rbp-148]
mov bx, word[rax]
mov word[rbp-150], bx

mov rax, qword[rbp-148]
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
mov word[rbp-152], ax


mov ax, word[rbp-152]
mov bx, 2
mul bx
mov word[rbp-154], ax

mov ax, word[rbp-154]
mov bx, 9
add ax, bx
mov word[rbp-154], ax



movsx rax, word[rbp-154]
mov rbx, rbp
sub rbx, rax
mov qword[rbp-158], rbx


mov rax, qword[rbp-158]
mov bx, word[rax]
mov word[rbp-160], bx

mov rax, qword[rbp-158]
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
