.model small;   
.stack 100h;    
.data
i dw 0h
String db ': $' 
Stg db 100h dup(0h);     ������ ��� ������   
.code   
Start: 

Sort proc
mov ax, @data;  
mov ds, ax;
mov ah, 00h;     ������� ������
mov al, 2h; 
int 10h
mov ah, 09h
Lea dx, String
int 21h 
mov ah, 1h;  �-� ����� �������
mov si, 0h
mov bx, 0h

Input:   ;���� �������
int 21h
mov cx, si
mov Stg[bx], cl;     ����� �����
cmp al, 32;  �������� �� ������
jne Skip1
mov si, 0h
add bx, 10h;     ������ ���������� �����
jmp Input

Skip1:
inc si
mov Stg[bx+si], al;  ��������� ������� � ������  
cmp al, 13
jne Input
mov Stg[bx+si], 0h;  �������� Enter'�
mov i, bx;   ���-�� ����
mov bx, 0h

Sort1:   ;���������� ����������
mov di, bx;  ������ ����������� �����
mov ax, bx
add ax, 10h

Sort2:
mov si, ax 
mov cl, Stg[di]
cmp cl, Stg[si]

jae Skip2
mov di, si;  ���� ������ 

Skip2: 
add ax, 10h
cmp ax, i
jbe Sort2
mov si, 0h

Sort3:
moV cl, Stg[bx+si]; ����� ����
mov al, Stg[di]
mov Stg[bx+si], al
mov Stg[di], cl
inc si
inc di
cmp si, 10h
jb Sort3
add bx, 10h
cmp bx, i  

jb Sort1
mov ah, 02h; �-� ��������� ������� �������:
mov bh, 0h;  � ��������   
mov dh, 2h; � ������
mov dl, 0h; � �������
int 10h
mov bx, 0h
mov si, 0h
mov ah, 2h;  �-� ������ �������

Output:  ;����� �������
inc si
mov dx, word ptr Stg[bx+si]
cmp dx, 0h
jne Skip3
cmp bx, i
je Exit
mov si, 0h
add bx, 10h
mov dx, ' ' 

Skip3:
int 21h
cmp bx, i
jbe Output

Exit:
mov ah, 4ch;
int 21h 

Sort endp
End Start