.model tiny
.code
org 80h            ;смещение 80h от начала PSP (готовый буфер DTA)
cmd_length db ?    ;длина командной строки
cmd_line db ?      ;сама командная строка
org 100h           ;начало программы 100h от начала PSP

start:
        mov ah, 1Ah              ;установить адрес DTA (переопределение DTA), заполняемая функциями поиска файлоа
        mov dx, offset DTA       ;адрес для DTA
        int 21h

        cmp cmd_length, 07Eh     ;сравниваем длину командной строки с 126
        je cmd_length_error
        jmp extension

cmd_length_error:
        push offset cmd_length_error_msg  ;поместить 1-ый параметр в стек
        push cmd_length_error_msg_length  ;поместить 2-ой параметр в стек
        call putstr
        ret

extension:
        mov cx, -1                ;для команд работы со строками
        mov di, offset cmd_line   ;начало командной строки в ES:DI
        
find_param:                   
        mov al, ' '               ;AL = ' '
        repz scasb                ;искать не ' '
        dec di                    ;DI = адрес начала параметра   
        mov params, di            ;сохранить адрес в params
        mov si, di                ;SI = DI для следующей команды lodsb
        
scan_param:                   
        lodsb                     ;прочитать символ из параметра  
        inc counter
        cmp al, 0Dh               ;если 0Dh - последний параметр
        je params_error           ;параметры закончились 
        cmp al, 20h               ;сравнение с ' '
        jne scan_param            ;текущий параметр не закончился 
        ;cmp counter, 4
        ;jne not_three
        ;sub si, 2
        ;cmp si, '\'
        ;jne not_three
        ;mov byte ptr [si], 0
        ;inc si
        ;mov byte ptr [si], ' '
        ;jmp next   
        
;not_three:
        dec si                    ;SI = первый байт после параметра
        mov byte ptr [si], 0      ;записать в него 0

;next:
        mov di, si                ;DI = SI для команды scasb
        inc di                    ;DI = следующий после 0 символ
        mov al, ' '               ;AL = ' '
        repz scasb                ;искать не ' '
        dec di                    ;DI = адрес начала 2-ого параметра
        cmp byte ptr [di], '"'
        jne params_error
        inc di
        mov params + 2, di
        mov si, di                ;SI = DI для следующей команды lodsb
        
scan_param_str:
        lodsb
        cmp al, 0Dh               ;если 0Dh - последний параметр
        je params_error
        cmp al, '"'               ;сравнение на последнюю "
        jne scan_param_str
        dec si                    ;SI = первый байт после параметра
        mov byte ptr [si], 0      ;записать в него 0
        jmp find_f   
        
params_error:
        push offset params_error_msg
        push params_error_length
        call putstr
        ret    
        
find_f:
        mov cx, -1
        mov al, 0
        mov di, params + 2         
        repnz scasb                  ;искать 0
        mov ax, params + 2           ;начало 2-ого параметра
        sub di, ax                   ;размер подстроки
        dec di
        mov ax, di
        mov str_size, al

        mov ah, 47h                  ;определить текущий каталог
        mov dl, 00h                  ;текущий диск
        mov si, offset current_dir   ;адрес 64-байтного буфера для текущего пути
        int 21h
        jc current_dir_error         ;если CF = 1, указан не существующий диск
        jmp not_current_dir_error
        
current_dir_error:
        push offset current_dir_error_msg
        push current_dir_error_msg_length
        call putstr    
        
not_current_dir_error:
        mov ah, 3Bh                  ;адрес 64-байтного буфера, который станет текущим каталогом
        mov di, offset params
        mov dx, [di]
        int 21h
        jc dir_error                 ;путь не найден
        jmp not_dir_error   
        
dir_error:
        push offset dir_error_msg
        push dir_error_msg_length
        call putstr
        ret     
        
not_dir_error:
        mov ah, 4Eh                  ;поиск первого файла
        xor cx, cx                   ;не системный, не каталог и т.д.
        mov dx, offset filespec      ;маска для поиска в DS:DX
        
file_open:
        int 21h
        jnc next_file_found          
        jmp no_more_files            ;если CF = 1 - файлы кончились

next_file_found:
        mov cx, 0FFh                 ;255 - конец PSP
        mov di, offset DTA + 1Eh     ;смещение DTA + смещение имени файла от начала DTA
        mov al, 0
        repnz scasb                  ;поиск 0
        dec di
        sub di, offset DTA + 1Eh
        mov file_name_length, di     ;длина имени файла

        mov ax, 3D02h                ;открыть файл для чтения и записи
        mov dx, offset DTA + 1Eh     ;смещение DTA + смещение имени файла от начала DTA
        int 21h
        jnc init_reading
        jmp file_open_error          ;если файл не открылся - перейти к следующему
        
init_reading:
        mov bx, ax                   ;идентификатор файла в ВХ
        
read_next:
        mov al, str_size
        mul two
        mov cx, ax                    ;число байт для чтения
        mov dx, offset buffer         ;адрес буфера для приема данных
        cld
                                      
        mov ah, 3Fh                   ;чтение файла
        int 21h
        jnc check_input_length        ;операция выполнена успешно

        push offset unable_read_msg
        push unable_read_msg_length
        call putstr
        push offset DTA + 1Eh
        push file_name_length
        call putstr
        push offset enter
        push 1
        call putstr                   ;переход на следующую строку

check_input_length:
        cmp al, str_size              ;если число фактически считанных байт меньше, чем заказанное число
        jl find_next
        cmp ax, cx
        je start_cycle
        mov di, offset buffer
        add di, ax
        sub cx, ax
        mov al, 0
        rep stosb  
        
start_cycle:
        xor cx, cx
cycle:
        mov di, offset params + 2
        mov si, [di]
        mov di, offset buffer
        add di, cx
        push cx
        xor ch, ch
        mov cl, str_size
        repz cmpsb         ;пока равны
        pop cx
        jz found           ;найден
        inc cx
        cmp cl, str_size
        jle cycle          ;<=
        push cx
        cmp str_size, 1
        je no_offset
        mov ah, 42h        ;переместить указатель файла 
        mov cx, 0FFFFh     ;cld
        xor dh, dh
        mov dl, str_size
        dec dl
        neg dx
        mov al, 1          ;перемещение относительно текущей позиции 
        int 21h   
        
no_offset:
        pop cx
        jmp read_next  
        
found:
        cmp result_files, 0
        jnz not_first            ;не первый файл
        push offset result_msg
        push result_msg_length
        call putstr   
        
not_first:
        inc result_files
        call out  
        
find_next:
        mov ah, 3Eh              ;закрыть файл
        int 21h
        jmp next_file  
        
file_open_error:
        push offset open_file_error_msg
        push open_file_error_msg_length
        call putstr
        push offset DTA + 1Eh    ;смещение DTA + смещение имени файла от начала DTA
        push file_name_length
        call putstr
        push offset enter
        push 1
        call putstr 
        
next_file:
        mov ah, 4Fh          ;найти следующий файл
        ;mov dx, 80h
        jmp file_open

no_more_files:
        cmp result_files, 0
        jnz something_found
        push offset no_files_msg
        push no_files_msg_length
        call putstr
     
something_found:
        mov ah, 3Bh          ;сменить каталог
        mov dx, offset disk  ;адрес 64-байтного буфера с путём, который станет текущим каталогом
        int 21h
        jc to_current_dir_error
        ret    
        
to_current_dir_error:
        push offset to_current_dir_error_msg
        push to_current_dir_error_msg_length
        call putstr
        ret

putstr proc				  
line_length equ [bp + 4] ;2-ой параметр стека (при выполнении call в стек поместили адрес возврата (2 байта) + в bp - 2 байта)
line equ [bp + 6]        ;1-ый параметр стека

        push bp
        mov bp, sp       ;сохранить текущую вершину стека в bp

        push ax
        push bx
        push cx
        push dx

        mov ah, 40h              ;вывод данных
        mov bx, 01h              ;STDOUT
        mov cx, line_length      ;число байт для записи
        mov dx, line             ;адрес буфера с данными
        int 21h
       
        pop dx
        pop cx
        pop bx
        pop ax
        mov sp, bp
	pop bp
	ret 4                          ;возврат из процедуры и освобожение стека от параметров
endp

proc out 
        push bx
        mov ah, 40h                ;вывод
        mov bx, 01h                ;STDOUT
        mov cx, file_name_length   
        mov dx, offset DTA + 1Eh   ;адрес смещения DTA + смещения имени файла от начала DTA
        int 21h
        mov ah, 40h
        mov cx, 1
        mov dx, offset enter
        int 21h
        pop bx
        ret
endp 

filespec db "*.txt",0    ;маска для поиска
params dw ?              ;параметры командной строки
       dw ?    
buffer db 252 dup (?)
str_size db ?
disk db "A:\"
current_dir db 64 dup (?)   
DTA db 128 dup (?)

file_name_length dw ?
result_files db 0 
counter db 0

two db 2
enter db 0Ah
cmd_length_error_msg db "Command line length exceeds the required length!", 0Ah, 0Dh, 0
cmd_length_error_msg_length dw $ - cmd_length_error_msg
params_error_msg db "Wrong command line format",0Ah, 0Dh, 0
params_error_length dw $ - params_error_msg    
dir_error_msg db "Can not open directory",0Ah, 0Dh, 0
dir_error_msg_length dw $ - dir_error_msg
current_dir_error_msg db "Can't check current directory",0Ah, 0Dh, 0
current_dir_error_msg_length dw $ - current_dir_error_msg
open_file_error_msg db "Can not open file!"
open_file_error_msg_length dw $ - open_file_error_msg
to_current_dir_error_msg db "Can not return to execution directory",0Ah, 0Dh, 0
to_current_dir_error_msg_length dw $ - to_current_dir_error_msg
unable_read_msg db "Unable to read file"
unable_read_msg_length dw $ - unable_read_msg
result_msg db "The resulting files containing the specified substring:", 0Ah, 0Dh
result_msg_length dw $ - result_msg
no_files_msg db "There is no files found!", 0Dh
no_files_msg_length dw $ - no_files_msg

end start