.model tiny

.code

org 100h
main proc

start:
mov cl, ds:[80h] ;обработка командной строки - уточнение
cmp cl, 1        ; кол-во ост копий
ja ProcessParamStr


EnterN:
mov ah,09h        ; ввод стр 
lea dx,[asPrompt  ; буффер - dta (область параметров)0 
int 21h           ; после ввода из 81 в 80 увеличиваем на 1
                  ; тк(space), который учитывается в стр парамс     
mov ah,0Ah        ; (все чтобы был 1 обработчик для обоих случаев)  
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

; обрабатываем параметры строки, берем кол-во запусков
ProcessParamStr:
xor ch,ch
dec cx
mov si,82h ; значимый символ (первый)   
; ДТА+2 в ПСП 
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

; создаем строку параметров для вызываемой прогрм
;(длина,пробел, строка, код 0Dh)
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
loop _store ; ищем строку с именем программы                
mov byte ptr [di],0Dh ; ищем два '0' в среде окруж     
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
; es:di - указывает на имя проги
mov word ptr[lpChildProcess],di
mov word ptr[lpChildProcess+2],es
pop es


mov ah,4Ah ; делаем меньше объем памяти                  
mov bx,offset EndOfProgram ; который выделила OS при запуске 
mov cl,4                   ; для родителя
shr bx,cl
inc bx   ; es - указ на псп
int 21h

; уменьшим стек для дос прогрм, для уменьшения образа проги

cli
mov sp,(offset EndOfProgram)-2
sti

; подготовка к EXEC, сохранили значения
; ss,sp,ds,es,dta в переменные
; которые адресованы через рег CS
push es
push ds
mov [stkseg],ss
mov [stkptr],sp
;- подготовка блока параметров
mov ax, ds:[2Ch]
mov [wEnvSeg],ax
mov ax,cs         ; psp в ax
mov word ptr[pfrFCB_1],005Ch
mov word ptr[pfrFCB_1+2], ax
mov word ptr[pfrFCB_2],006Ch
mov word ptr[pfrFCB_2+2], ax
mov word ptr[pfCmdTail], offset ChildParams
mov word ptr[pfCmdTail+2],ax


;exec для  внеш прогр
mov ax,4B00h
lea bx,[ExecParamRec]          ;es:bx
lds dx,cs:[lpChildProcess]     ;ds:dx
int 21h
; восстановление локальных значений ss and sp
cli
mov ss,cs:[stkseg]
mov sp,cs:[stkptr]
sti
; восстан-ие ds es и лок DTA
pop ds
pop es
; чекаем флаг CF, на ошибку exec
jnc ExecOk

mov ah,09h
lea dx,[asErrExec] ; ошибка
int 21h
ExecOk:

;выход
exit:
mov ah,09h
lea dx,[asBye]
int 21h
mov al,[N]
call ShowUInt8
; возвращ управл OS
int 20h
main endp

; вывод на экран (al)
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
stkseg dw ? ; стек сегмент   
stkptr dw ? ; указатель на стек   
ExecParamRec label byte
wEnvSeg dw ?
pfCmdTail dd ?
pfrFCB_1 dd ?
pfrFCB_2 dd ?
N db ?
;новый стек
align dw 2
NewStack db 200h dup(?)

EndOfProgram label byte ; последний знач байт проги

end main
