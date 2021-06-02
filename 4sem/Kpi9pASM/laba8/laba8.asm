.model tiny            
.CODE        
org 100h
start:    
jmp install_handler 


int_handler proc far       
pushf         
pusha  
push es
push ds
push cs
pop ds
          
push 0B800h
pop es   
mov si,19h  ;выс cmd
mov bx,0    ;буфф   
str_search:

mov cx,50h  ;ширина cmd    
mov di,0    ; первый символ 
search_scobok:  
mov al,07h       ;цвет
mov es:[bx+1],al ;прим цвет
mov al,28h       ; '('
cmp es:[bx],al   ; проверяем наличие (
jne not_(      ; если нет , чекаем )

push bx      ; буффер в стек
inc di       
     
not_(:
  
mov al,29h       ; ')'
cmp es:[bx],al   ; чек если не )
jne not_)      

cmp di,0     
je wrong_)
pop ax
dec di
jmp not_)

wrong_):
mov al,03h ; цвет меняем
mov es:[bx+1],al
        
not_):
add bx,2  ; след char в bx
loop search_scobok  
mov cx,di    

cmp di,0
je no_wrong_(
mov dx,bx

paint_(:                ; покраска незакрытых скб
pop bx
mov al,03h   ; color
mov es:[bx+1],al
    
loop paint_(

mov bx,dx
no_wrong_(:   
sub si,1
cmp si,0
jne str_search
    
pop ds
pop es
popa

popf         
db 0eah   ; jmp far на мтк после нее
          ; Суть.при выходе из обраб прер происходит возврат флагов
          ; пред прер , а потом делаем прыжок jmp far
          ; ввводит в итоге где бло вызвано прерывание (far)
 
 
old_int dw ?
old_int1 dw ?
int_handler endp
 
install_handler:  
mov ah,35h   ; возвращ адрес int handl
mov al,09h   ; № вектора 
int 21h
mov word ptr old_int,bx
mov word ptr old_int1,es

mov dx,offset int_handler
mov ah,25h ;  установка адреса int_handl
mov al,09h ; вектор
int 21h     
    
mov ax,3100h ; оствляем резедентной 

  
mov dx,0FFFFh/16 ; размер рез пр
int 21h  

mov ah,4ch
mov al,00      
int 21h 
  
end start             
