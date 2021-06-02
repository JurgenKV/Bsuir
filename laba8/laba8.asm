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
mov si,19h  ;��� cmd
mov bx,0    ;����   
str_search:

mov cx,50h  ;������ cmd    
mov di,0    ; ������ ������ 
search_scobok:  
mov al,07h       ;����
mov es:[bx+1],al ;���� ����
mov al,28h       ; '('
cmp es:[bx],al   ; ��������� ������� (
jne not_(      ; ���� ��� , ������ )

push bx      ; ������ � ����
inc di       
     
not_(:
  
mov al,29h       ; ')'
cmp es:[bx],al   ; ��� ���� �� )
jne not_)      

cmp di,0     
je wrong_)
pop ax
dec di
jmp not_)

wrong_):
mov al,03h ; ���� ������
mov es:[bx+1],al
        
not_):
add bx,2  ; ���� char � bx
loop search_scobok  
mov cx,di    

cmp di,0
je no_wrong_(
mov dx,bx

paint_(:                ; �������� ���������� ���
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
db 0eah   ; jmp far �� ��� ����� ���
          ; ����.��� ������ �� ����� ���� ���������� ������� ������
          ; ���� ���� , � ����� ������ ������ jmp far
          ; ������� � ����� ��� ��� ������� ���������� (far)
 
 
old_int dw ?
old_int1 dw ?
int_handler endp
 
install_handler:  
mov ah,35h   ; ������� ����� int handl
mov al,09h   ; � ������� 
int 21h
mov word ptr old_int,bx
mov word ptr old_int1,es

mov dx,offset int_handler
mov ah,25h ;  ��������� ������ int_handl
mov al,09h ; ������
int 21h     
    
mov ax,3100h ; �������� ����������� 

  
mov dx,0FFFFh/16 ; ������ ��� ��
int 21h  

mov ah,4ch
mov al,00      
int 21h 
  
end start             
