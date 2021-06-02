.model small
.stack 100h
.data  
    
size equ 30           ;максимальный размер массива    
array dw size dup(?)  ;массив
amount dw ?           ;размер массива 
oldmin dw 7FFFh       ;минимальное число в массиве
oldmax dw 8000h       ;максимальное число в массиве  

newmin dw ?           ;новое минимальное значение
newmax dw ?           ;новое максимальное значение
olddif dw ?           ;старая разница макс и мин значений
newdif dw ?           ;новая разница
quot dw ?             ;частное двух разниц

buffer db 7,7 dup('$') ;буффер для строки 
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
            
            
getamount proc           ;ввод длины массива
    xor si,si
    call number
    mov amount, ax       ;запомнить размер
    cmp ax, 1            ;проверить на минимум 
    jg Min:
    mov ax, 0900h                   
    mov dx, offset errorL
    int 21h
    mov ax, 4c00h 
    int 21h
    Min:
    cmp ax, 31           ;проверить на максимум
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
            
            
number proc              ;ввод числа
input: 
    mov dx, offset buffer;ввести строку в буфер
    mov ah,0ah
    int 21h 
    mov si,1             ;индекс элемента массива
    mov di,2             
    xor ax,ax
    xor cx,cx
    xor bx,bx
    mov cl, buffer+1     ;поместить в счетчик длину буффера  
buf:    
    cmp cl, buffer+1
    jne digit            ;если не равно  
minus:                   ;проверить первый знак строки на минус
    mov bl, buffer+2   
    cmp bl, '-'
    jne digit            ;если не равно
    mov si,-1            
    inc di
    loop buf             ;к началу 
digit:                   ;если не минус
    mov bl, buffer[di]   
    inc di
    sub bl, '0'          ;проверить правильны ли символы
    jl errorinp          ;если меньше - ошибка
    add bl,'0'
    sub bl,'9' 
    jg errorinp          ;если больше - ошибка
    add bl,'9'     
num_translate:           ;перевод в число
    mov dx,10
    imul dx              ;умножение значения в ax на 10
    jo errorinp          ;если переполнение - ошибка
    sub bx,'0'           ;код символа-код 0=число
    add ax, bx           ;полученное число+число в ах
    jo errorinp          ;если переполнение - ошибка
    loop buf             
    cmp si, 1
    jne notpos           
    ret      
notpos:                  ;проверка на положительное число
    not ax               ;инверсия битов
    inc ax
    ret    
errorinp:             
    printstr error
    jmp main            
    ret
number endp  
            
          
massiv proc              ;ввод массива чисел
    mov cx,amount        ;передать в счетчик длину массива
    xor si,si
loopmas:
    push cx
    push si
    printstr mesNewEl
    call number  
    pop si
    pop cx
    mov array[si],ax     ;число в массив
    cmp ax, oldmin       ;сравнить с минимальным числом
    jg findmin:
    mov oldmin, ax       ;если меньше - обновляем мин  
    findmin:
    cmp ax, oldmax       ;сравнить с максимальным числом
    jl findmax:      
    mov oldmax, ax       ;если больше - обновляем макс   
    findmax:
    inc si               ;шаг дальше
    inc si                   
    loop loopmas         
    ret
massiv endp
   
   
getmin proc              ;ввод нового минимума
    mov ax, 0900h                   
    mov dx, offset mesNewMin      
    int 21h
    call number  
    mov newmin, ax  
    ret
getmin endp 
   
   
getmax proc              ;ввод нового максимума
    mov ax, 0900h                   
    mov dx, offset mesNewMax
    int 21h
    call number 
    mov newmax, ax 
    cmp ax, newmin       ;сравниваем мин и макс
    jge Transform:
        mov ax, 0900h    ;если мин больше макс
        mov dx, offset mesMaxMin
        int 21h
        mov ax, 4C00h
        int 21h         
    Transform:           ;преобразование массива
    ;(число-старый мин. массива)*(новая разница/старая разница) + новый мин. массива
    mov ax, newmax	     ;найти новую разницу
    sbb ax, newmin
    jo   errorinp	
    mov newdif, ax	
    mov quot, ax	    
    mov ax, oldmax	     ;найти старую разницу     
    jo   errorinp
    sbb ax, oldmin    
    mov olddif, ax         
    mov ax, newdif
    mov dx, 0
    idiv olddif          ;найти частное двух разниц
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
    mov ax, array[si]    ;запомнить число из массива	
    sub ax, oldmin       ;отнять старый мин. массива
    imul quot            ;умножить на частное двух разниц 
    jo   errorinp
    add ax, newmin       ;прибавить новый мин. массива   
    jo   errorinp
    cmp cx, amount	     ;проверить на конец 
    jg endmas:           
        mov array[si], ax;вернуть число в массив	
        inc si           
        inc si
        inc cx          
        cmp cx, amount   
        jne newmassiv: 		   
    endmas:					
	ret  
getmax endp
  
         
printarray proc          ;вывод массива
    mov ax, 0900h                   
    mov dx, offset mesOutput 
	int 21h
    pusha
    mov si, 0
    mov di, 0
    mov bx, dx
    mov cx, 0
printelement:
    mov ax, array[si]    ;поместить число
    call printnum        ;вывод числа
    mov ah, 02h
    mov dl, ' '
    int 21h
    mov di, 0
    inc si               ;шаг дальше
    inc si
    inc cx
    cmp cx, amount       ;проверить на конец
    jb printelement      ;если меньше
    popa
    ret
printarray endp         
         
               
printnum proc            ;вывод числа
    pusha
    xor di, di
    or ax, ax            ;проверить на отриц.
    jns interpret        ;если нет знака - переводим
    push ax
    mov dx, '-'          ;вывести минус
    mov ah, 02h    
    int 21h
    pop ax
    neg ax               ;поменять знак  
interpret:
    xor dx, dx 
    mov bx,10
    div bx               ;остаток в dl   
    add dl, '0'          ;добавить код нуля для перевода в символ
    inc di               ;шаг дальше
    push dx
    or ax, ax
    jnz interpret        ;если не ноль - продолжить перевод
print:
    pop dx
    mov ah, 02h          ;вывести
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