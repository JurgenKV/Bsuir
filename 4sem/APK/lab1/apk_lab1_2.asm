.model tiny   
org 100h 
.data
info db "Sended $"
.code 
start:
mov ah,0
mov al,11100011b
mov dx,1
int 14h
mov ah,9
mov dx, offset info
int 21h

main_loop:
mov ah,2 
int 14h 
test ah,ah
jnz no_input
int 29h

no_input:
mov ah,1
int 16h
jz main_loop
mov ah,8
int 21h
test al,al
jnz send_char
int 21h
cmp al,2Dh
jne send_char
ret      

send_char:
mov ah,1
int 14h 
jmp short main_loop  

end start
