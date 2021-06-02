.model small
.stack 100h
.data  
    
size equ 30           ;������������ ������ �������    
array dw size dup(?)  ;������
amount dw ?           ;������ ������� 
oldmin dw 7FFFh       ;����������� ����� � �������
oldmax dw 8000h       ;������������ ����� � �������  

newmin dw ?           ;����� ����������� ��������
newmax dw ?           ;����� ������������ ��������
olddif dw ?           ;������ ������� ���� � ��� ��������
newdif dw ?           ;����� �������
quot dw ?             ;������� ���� ������

buffer db 7,7 dup('$') ;������ ��� ������ 
sizeint db ?  
         
message db "Enter array: $" 
messageAm db "Enter amount of array:", 0Ah, 0Dh, '$'
messageNum db 0Ah, 0Dh, "Old array", 0Ah, 0Dh, '$'   
mesNewEl db 0Ah, 0Dh,"Enter new element:$"     
mesNewMin db 0Ah, 0Dh, "Enter Min:", 0Ah, 0Dh, '$'
mesNewMax db 0Ah, 0Dh, "Enter Max:", 0Ah, 0Dh, '$'
mesMaxMin db 0Ah, 0Dh, "Max can't be less then Min" , 0Ah, 0Dh, '$'  
mesOutput db 0Ah, 0Dh, "Result:" , 0Ah, 0Dh, '$' 
 
error db 0Ah, 0Dh,"The number is overflowing", 0Ah, 0Dh," or entered bad symbols", 0Ah, 0Dh, '$'
errorL db 0Ah, 0Dh, "Amount should be more than 0", 0Ah, 0Dh,'$' 
errorM db 0Ah, 0Dh, "Amount should be less than 31", 0Ah, 0Dh,'$'
  
.code
  
printstr macro str      
    mov dx,offset str
    mov ah, 09h
    int 21h   
    endm
            
            
getamount proc           ;���� ����� �������
    xor si,si
    call number
    mov amount, ax       ;��������� ������
    cmp ax, 1            ;��������� �� ������� 
    jg Min:
    mov ax, 0900h                   
    mov dx, offset errorL
    int 21h
    mov ax, 4c00h 
    int 21h
    Min:
    cmp ax, 31           ;��������� �� ��������
    jl Max: 
    mov ax, 0900h   
    mov dx, offset errorM
    int 21h
    mov ax, 4c00h 
    int 21h
    Max:
    mov oldmax, ax 
    ret      
getamount endp            
            
            
number proc              ;���� �����
input: 
    mov dx, offset buffer;������ ������ � �����
    mov ah,0ah
    int 21h 
    mov si,1             ;������ �������� �������
    mov di,2             
    xor ax,ax
    xor cx,cx
    xor bx,bx
    mov cl, buffer+1     ;��������� � ������� ����� �������  
buf:    
    cmp cl, buffer+1
    jne digit            ;���� �� �����  
minus:                   ;��������� ������ ���� ������ �� �����
    mov bl, buffer+2   
    cmp bl, '-'
    jne digit            ;���� �� �����
    mov si,-1            
    inc di
    loop buf             ;� ������ 
digit:                   ;���� �� �����
    mov bl, buffer[di]   
    inc di
    sub bl, '0'          ;��������� ��������� �� �������
    jl errorinp          ;���� ������ - ������
    add bl,'0'
    sub bl,'9' 
    jg errorinp          ;���� ������ - ������
    add bl,'9'     
num_translate:           ;������� � �����
    mov dx,10
    imul dx              ;��������� �������� � ax �� 10
    jo errorinp          ;���� ������������ - ������
    sub bx,'0'           ;��� �������-��� 0=�����
    add ax, bx           ;���������� �����+����� � ��
    jo errorinp          ;���� ������������ - ������
    loop buf             
    cmp si, 1
    jne notpos           
    ret      
notpos:                  ;�������� �� ������������� �����
    not ax               ;�������� �����
    inc ax
    ret    
errorinp:             
    printstr error
    jmp main            
    ret
number endp  
            
          
massiv proc              ;���� ������� �����
    mov cx,amount        ;�������� � ������� ����� �������
    xor si,si
loopmas:
    push cx
    push si
    printstr mesNewEl
    call number  
    pop si
    pop cx
    mov array[si],ax     ;����� � ������
    cmp ax, oldmin       ;�������� � ����������� ������
    jg findmin:
    mov oldmin, ax       ;���� ������ - ��������� ���  
    findmin:
    cmp ax, oldmax       ;�������� � ������������ ������
    jl findmax:      
    mov oldmax, ax       ;���� ������ - ��������� ����   
    findmax:
    inc si               ;��� ������
    inc si                   
    loop loopmas         
    ret
massiv endp
   
   
getmin proc              ;���� ������ ��������
    mov ax, 0900h                   
    mov dx, offset mesNewMin      
    int 21h
    call number  
    mov newmin, ax  
    ret
getmin endp 
   
   
getmax proc              ;���� ������ ���������
    mov ax, 0900h                   
    mov dx, offset mesNewMax
    int 21h
    call number 
    mov newmax, ax 
    cmp ax, newmin       ;���������� ��� � ����
    jge Transform:
        mov ax, 0900h    ;���� ��� ������ ����
        mov dx, offset mesMaxMin
        int 21h
        mov ax, 4C00h
        int 21h         
    Transform:           ;�������������� �������
    ;(�����-������ ���. �������)*(����� �������/������ �������) + ����� ���. �������
    mov ax, newmax	     ;����� ����� �������
    sbb ax, newmin
    jo   errorinp	
    mov newdif, ax	
    mov quot, ax	    
    mov ax, oldmax	     ;����� ������ �������     
    jo   errorinp
    sbb ax, oldmin    
    mov olddif, ax         
    mov ax, newdif
    mov dx, 0
    idiv olddif          ;����� ������� ���� ������
    mov quot, ax
    xor bp, bp		
    mov ch, 00h		
    mov cl, sizeint
    mov ax, amount
    mov al, 0
    mov si, ax
    mov cx, 0	
newmassiv:
    mov ax, 0	
    mov ax, array[si]    ;��������� ����� �� �������	
    sub ax, oldmin       ;������ ������ ���. �������
    imul quot            ;�������� �� ������� ���� ������ 
    jo   errorinp
    add ax, newmin       ;��������� ����� ���. �������   
    jo   errorinp
    cmp cx, amount	     ;��������� �� ����� 
    jg endmas:           
        mov array[si], ax;������� ����� � ������	
        inc si           
        inc si
        inc cx          
        cmp cx, amount   
        jne newmassiv: 		   
    endmas:					
	ret  
getmax endp
  
         
printarray proc          ;����� �������
    mov ax, 0900h                   
    mov dx, offset mesOutput 
	int 21h
    pusha
    mov si, 0
    mov di, 0
    mov bx, dx
    mov cx, 0
printelement:
    mov ax, array[si]    ;��������� �����
    call printnum        ;����� �����
    mov ah, 02h
    mov dl, ' '
    int 21h
    mov di, 0
    inc si               ;��� ������
    inc si
    inc cx
    cmp cx, amount       ;��������� �� �����
    jb printelement      ;���� ������
    popa
    ret
printarray endp         
         
               
printnum proc            ;����� �����
    pusha
    xor di, di
    or ax, ax            ;��������� �� �����.
    jns interpret        ;���� ��� ����� - ���������
    push ax
    mov dx, '-'          ;������� �����
    mov ah, 02h    
    int 21h
    pop ax
    neg ax               ;�������� ����  
interpret:
    xor dx, dx 
    mov bx,10
    div bx               ;������� � dl   
    add dl, '0'          ;�������� ��� ���� ��� �������� � ������
    inc di               ;��� ������
    push dx
    or ax, ax
    jnz interpret        ;���� �� ���� - ���������� �������
print:
    pop dx
    mov ah, 02h          ;�������
    int 21h
    dec di
    jnz print
    popa
    ret
printnum endp
     
     
main:
    mov ax, @data
    mov ds, ax
    mov dx, offset messageAm
    mov ah,9
    int 21h  
    call getamount  
    mov ax, 0900h
    mov dx, offset messageNum
    int 21h
    call massiv
    call getmin
    call getmax
    call printarray
    mov ax, 4c00h 
    int 21h    
ends

end main