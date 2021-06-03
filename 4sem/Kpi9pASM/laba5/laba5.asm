.model tiny
.code
org 80h            ;�������� 80h �� ������ PSP (������� ����� DTA)
cmd_length db ?    ;����� ��������� ������
cmd_line db ?      ;���� ��������� ������
org 100h           ;������ ��������� 100h �� ������ PSP

start:
        mov ah, 1Ah              ;���������� ����� DTA (��������������� DTA), ����������� ��������� ������ ������
        mov dx, offset DTA       ;����� ��� DTA
        int 21h

        cmp cmd_length, 07Eh     ;���������� ����� ��������� ������ � 126
        je cmd_length_error
        jmp extension

cmd_length_error:
        push offset cmd_length_error_msg  ;��������� 1-�� �������� � ����
        push cmd_length_error_msg_length  ;��������� 2-�� �������� � ����
        call putstr
        ret

extension:
        mov cx, -1                ;��� ������ ������ �� ��������
        mov di, offset cmd_line   ;������ ��������� ������ � ES:DI
        
find_param:                   
        mov al, ' '               ;AL = ' '
        repz scasb                ;������ �� ' '
        dec di                    ;DI = ����� ������ ���������   
        mov params, di            ;��������� ����� � params
        mov si, di                ;SI = DI ��� ��������� ������� lodsb
        
scan_param:                   
        lodsb                     ;��������� ������ �� ���������  
        inc counter
        cmp al, 0Dh               ;���� 0Dh - ��������� ��������
        je params_error           ;��������� ����������� 
        cmp al, 20h               ;��������� � ' '
        jne scan_param            ;������� �������� �� ���������� 
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
        dec si                    ;SI = ������ ���� ����� ���������
        mov byte ptr [si], 0      ;�������� � ���� 0

;next:
        mov di, si                ;DI = SI ��� ������� scasb
        inc di                    ;DI = ��������� ����� 0 ������
        mov al, ' '               ;AL = ' '
        repz scasb                ;������ �� ' '
        dec di                    ;DI = ����� ������ 2-��� ���������
        cmp byte ptr [di], '"'
        jne params_error
        inc di
        mov params + 2, di
        mov si, di                ;SI = DI ��� ��������� ������� lodsb
        
scan_param_str:
        lodsb
        cmp al, 0Dh               ;���� 0Dh - ��������� ��������
        je params_error
        cmp al, '"'               ;��������� �� ��������� "
        jne scan_param_str
        dec si                    ;SI = ������ ���� ����� ���������
        mov byte ptr [si], 0      ;�������� � ���� 0
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
        repnz scasb                  ;������ 0
        mov ax, params + 2           ;������ 2-��� ���������
        sub di, ax                   ;������ ���������
        dec di
        mov ax, di
        mov str_size, al

        mov ah, 47h                  ;���������� ������� �������
        mov dl, 00h                  ;������� ����
        mov si, offset current_dir   ;����� 64-�������� ������ ��� �������� ����
        int 21h
        jc current_dir_error         ;���� CF = 1, ������ �� ������������ ����
        jmp not_current_dir_error
        
current_dir_error:
        push offset current_dir_error_msg
        push current_dir_error_msg_length
        call putstr    
        
not_current_dir_error:
        mov ah, 3Bh                  ;����� 64-�������� ������, ������� ������ ������� ���������
        mov di, offset params
        mov dx, [di]
        int 21h
        jc dir_error                 ;���� �� ������
        jmp not_dir_error   
        
dir_error:
        push offset dir_error_msg
        push dir_error_msg_length
        call putstr
        ret     
        
not_dir_error:
        mov ah, 4Eh                  ;����� ������� �����
        xor cx, cx                   ;�� ���������, �� ������� � �.�.
        mov dx, offset filespec      ;����� ��� ������ � DS:DX
        
file_open:
        int 21h
        jnc next_file_found          
        jmp no_more_files            ;���� CF = 1 - ����� ���������

next_file_found:
        mov cx, 0FFh                 ;255 - ����� PSP
        mov di, offset DTA + 1Eh     ;�������� DTA + �������� ����� ����� �� ������ DTA
        mov al, 0
        repnz scasb                  ;����� 0
        dec di
        sub di, offset DTA + 1Eh
        mov file_name_length, di     ;����� ����� �����

        mov ax, 3D02h                ;������� ���� ��� ������ � ������
        mov dx, offset DTA + 1Eh     ;�������� DTA + �������� ����� ����� �� ������ DTA
        int 21h
        jnc init_reading
        jmp file_open_error          ;���� ���� �� �������� - ������� � ����������
        
init_reading:
        mov bx, ax                   ;������������� ����� � ��
        
read_next:
        mov al, str_size
        mul two
        mov cx, ax                    ;����� ���� ��� ������
        mov dx, offset buffer         ;����� ������ ��� ������ ������
        cld
                                      
        mov ah, 3Fh                   ;������ �����
        int 21h
        jnc check_input_length        ;�������� ��������� �������

        push offset unable_read_msg
        push unable_read_msg_length
        call putstr
        push offset DTA + 1Eh
        push file_name_length
        call putstr
        push offset enter
        push 1
        call putstr                   ;������� �� ��������� ������

check_input_length:
        cmp al, str_size              ;���� ����� ���������� ��������� ���� ������, ��� ���������� �����
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
        repz cmpsb         ;���� �����
        pop cx
        jz found           ;������
        inc cx
        cmp cl, str_size
        jle cycle          ;<=
        push cx
        cmp str_size, 1
        je no_offset
        mov ah, 42h        ;����������� ��������� ����� 
        mov cx, 0FFFFh     ;cld
        xor dh, dh
        mov dl, str_size
        dec dl
        neg dx
        mov al, 1          ;����������� ������������ ������� ������� 
        int 21h   
        
no_offset:
        pop cx
        jmp read_next  
        
found:
        cmp result_files, 0
        jnz not_first            ;�� ������ ����
        push offset result_msg
        push result_msg_length
        call putstr   
        
not_first:
        inc result_files
        call out  
        
find_next:
        mov ah, 3Eh              ;������� ����
        int 21h
        jmp next_file  
        
file_open_error:
        push offset open_file_error_msg
        push open_file_error_msg_length
        call putstr
        push offset DTA + 1Eh    ;�������� DTA + �������� ����� ����� �� ������ DTA
        push file_name_length
        call putstr
        push offset enter
        push 1
        call putstr 
        
next_file:
        mov ah, 4Fh          ;����� ��������� ����
        ;mov dx, 80h
        jmp file_open

no_more_files:
        cmp result_files, 0
        jnz something_found
        push offset no_files_msg
        push no_files_msg_length
        call putstr
     
something_found:
        mov ah, 3Bh          ;������� �������
        mov dx, offset disk  ;����� 64-�������� ������ � ����, ������� ������ ������� ���������
        int 21h
        jc to_current_dir_error
        ret    
        
to_current_dir_error:
        push offset to_current_dir_error_msg
        push to_current_dir_error_msg_length
        call putstr
        ret

putstr proc				  
line_length equ [bp + 4] ;2-�� �������� ����� (��� ���������� call � ���� ��������� ����� �������� (2 �����) + � bp - 2 �����)
line equ [bp + 6]        ;1-�� �������� �����

        push bp
        mov bp, sp       ;��������� ������� ������� ����� � bp

        push ax
        push bx
        push cx
        push dx

        mov ah, 40h              ;����� ������
        mov bx, 01h              ;STDOUT
        mov cx, line_length      ;����� ���� ��� ������
        mov dx, line             ;����� ������ � �������
        int 21h
       
        pop dx
        pop cx
        pop bx
        pop ax
        mov sp, bp
	pop bp
	ret 4                          ;������� �� ��������� � ����������� ����� �� ����������
endp

proc out 
        push bx
        mov ah, 40h                ;�����
        mov bx, 01h                ;STDOUT
        mov cx, file_name_length   
        mov dx, offset DTA + 1Eh   ;����� �������� DTA + �������� ����� ����� �� ������ DTA
        int 21h
        mov ah, 40h
        mov cx, 1
        mov dx, offset enter
        int 21h
        pop bx
        ret
endp 

filespec db "*.txt",0    ;����� ��� ������
params dw ?              ;��������� ��������� ������
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