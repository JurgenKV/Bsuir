.model tiny

.code

org 100h
main proc

start:
mov cl, ds:[80h] ;��������� ��������� ������ - ���������
cmp cl, 1        ; ���-�� ��� �����
ja ProcessParamStr


EnterN:
mov ah,09h        ; ���� ��� 
lea dx,[asPrompt  ; ������ - dta (������� ����������)0 
int 21h           ; ����� ����� �� 81 � 80 ����������� �� 1
                  ; ��(space), ������� ����������� � ��� ������     
mov ah,0Ah        ; (��� ����� ��� 1 ���������� ��� ����� �������)  
mov dx,80h           
mov word ptr ds:[80h],4  
int 21h                   
mov ah,02h          
mov dl,0Dh
int 21h
mov dl,0Ah
int 21h
mov cl,ds:[81h]
inc cl
mov ds:[80h],cl
cmp cl,1
jbe EnterN

; ������������ ��������� ������, ����� ���-�� ��������
ProcessParamStr:
xor ch,ch
dec cx
mov si,82h ; �������� ������ (������)   
; ���+2 � ��� 
mov bh,10
xor al,al
getc:
mov bl,[si]
inc si
sub bl,'0'
jb  EnterN
cmp bl,9
ja  EnterN
mul bh
add al,bl
adc ah,0
test ah,ah
jnz  EnterN
loop getc
mov [N],al
; hi
mov ah,09h
lea dx,[asHello]
int 21h
mov al,[N]
call ShowUInt8

; ������� ������ ���������� ��� ���������� ������
;(�����,������, ������, ��� 0Dh)
CreateParamStr:
xor cx,cx
mov al,[N]
cmp al, 0
ja _1
jmp exit
_1:
dec al
mov bh,10
_div:
inc cx
xor ah,ah
div bh
add ah,'0'
push ax
test al,al
jnz _div
mov [ChildParams],  cl
inc [ChildParams]
mov byte ptr[ChildParams+1], ' '
lea di,[ChildParams+2]
_store:
pop ax
mov [di],ah
inc di
loop _store ; ���� ������ � ������ ���������                
mov byte ptr [di],0Dh ; ���� ��� '0' � ����� �����     
push es
mov ax,ds:[2Ch]
mov es,ax
cld
xor ax,ax
xor di,di
jmp _test
while_env:
xor cx, cx
dec cx
repnz scasb
_test:
cmp al,byte ptr es:[di]
jne while_env
add di,3
; es:di - ��������� �� ��� �����
mov word ptr[lpChildProcess],di
mov word ptr[lpChildProcess+2],es
pop es


mov ah,4Ah ; ������ ������ ����� ������                  
mov bx,offset EndOfProgram ; ������� �������� OS ��� ������� 
mov cl,4                   ; ��� ��������
shr bx,cl
inc bx   ; es - ���� �� ���
int 21h

; �������� ���� ��� ��� ������, ��� ���������� ������ �����

cli
mov sp,(offset EndOfProgram)-2
sti

; ���������� � EXEC, ��������� ��������
; ss,sp,ds,es,dta � ����������
; ������� ���������� ����� ��� CS
push es
push ds
mov [stkseg],ss
mov [stkptr],sp
;- ���������� ����� ����������
mov ax, ds:[2Ch]
mov [wEnvSeg],ax
mov ax,cs         ; psp � ax
mov word ptr[pfrFCB_1],005Ch
mov word ptr[pfrFCB_1+2], ax
mov word ptr[pfrFCB_2],006Ch
mov word ptr[pfrFCB_2+2], ax
mov word ptr[pfCmdTail], offset ChildParams
mov word ptr[pfCmdTail+2],ax


;exec ���  ���� �����
mov ax,4B00h
lea bx,[ExecParamRec]          ;es:bx
lds dx,cs:[lpChildProcess]     ;ds:dx
int 21h
; �������������� ��������� �������� ss and sp
cli
mov ss,cs:[stkseg]
mov sp,cs:[stkptr]
sti
; �������-�� ds es � ��� DTA
pop ds
pop es
; ������ ���� CF, �� ������ exec
jnc ExecOk

mov ah,09h
lea dx,[asErrExec] ; ������
int 21h
ExecOk:

;�����
exit:
mov ah,09h
lea dx,[asBye]
int 21h
mov al,[N]
call ShowUInt8
; ������� ������ OS
int 20h
main endp

; ����� �� ����� (al)
ShowUInt8       proc
xor cx,cx
mov bh,10
div8:
inc cx
xor ah,ah
div bh
add ah,'0'
push ax
test al,al
jnz div8
mov ah,02h
show8:
pop dx
mov dl,dh
int 21h
loop show8
mov ah,02h
mov dl,0Dh
int 21h
mov dl,0Ah
int 21h
ret
ShowUInt8 endp


asHello db "Start - ", '$'
asBye db "End - ", '$'
asPrompt db "Num : ", '$'
asErrExec db 0Dh, 0Ah, "Exec error", 0Dh, 0Ah, '$'
lpChildProcess dd ?
ChildParams db 4, ' 255', 0Dh
stkseg dw ? ; ���� �������   
stkptr dw ? ; ��������� �� ����   
ExecParamRec label byte
wEnvSeg dw ?
pfCmdTail dd ?
pfrFCB_1 dd ?
pfrFCB_2 dd ?
N db ?
;����� ����
align dw 2
NewStack db 200h dup(?)

EndOfProgram label byte ; ��������� ���� ���� �����

end main
