;Laba 5, variant 1
; (c) 2010 Maxvale, maxvale.na.by
; (c) 2012 Byteman
; (c) 2012 evgeny3000
; BSUIR, VMSIS
;

 .model tiny 
     .386p
     .code
     org     100h

;структуры (by maxvale)
     segment_descriptor struc    ;Сегментный дискриптор
     limit_low   dw     0    ; Младшие 2 байта поля Segment limit
     base_low    dw     0    ; Младшие 2 байта поля Base Adress
     base_high0  db     0    ; Второй байт поля Base Address
     access_rights db   0    ; тип сегмента, тип дескриптора, DPL, бит присутствия сегмента
     flags       db     0    ; биты 19-16 лимита, бит разрядности, бит гранулярности
     base_high1  db     0    ; Старший байт поля Base Address
     segment_descriptor ends

     ; Дескриптор шлюза
     gate_descriptor struc
     offset_low    dw   0    ; Младшая часть смещения
     selector      dw   0    ; Селектор сегмента кода
     zero          db   0
     access_rights db   0    ; Права доступа 
     offset_high   dw   0    ; Старшая часть смещения 
     gate_descriptor ends

     ; Регистр, описывающий таблицу
     table_register struc
     limit       dw      0    ; Table Limit
     base        dd      0    ; Linear Base Address
     table_register ends

 start:  
     ; Initialize timer to generate 50Hz
	 mov al, 00110110b
     out 43h, al  
     mov ax, 05D2Eh 
     out 40h, al 
     shr ax, 8   
     out 40h, al 
     
     ; Clear screen
     mov ax,3
     int 10h
    
     lea dx, str_1   
     mov ah, 09h 
     int 21h 
     
     ; Ввод времени в секундах
     lea dx, delay_str;
     mov ah,0Ah          
     int 21h         
     
     mov si, offset delay_str+2 
     mov bl, 10 
     mov cl, delay_str[1] 
     call StrToInt  

     mov time, ax     
    

    
     push 0b800h
     pop  es
     xor  si,si 

    
     mov ax, cs
     mov cs:return_cs, ax 

     
     mov eax, 0
     mov ax, cs
     shl eax, 4   
     mov  CS64.base_low, ax
     shr  eax, 16
     mov  CS64.base_high0, al

     ; Сохраняем IDTR реального режима
     sidt fword ptr old_idtr 

     ; Запрещаем прерывания
     call disable_interrupts 
     
     ; Инициализируем GDT
     call i_gdt
     ; Инициализируем IDT
     call i_idt
     
     ;Переключаем режим
     mov     eax, cr0 ; прочитать регистр CR0
     or      al, 00000001b    ; установить бит PE,
     mov     cr0, eax ; с этого момента мы в защищенном режиме
     
     ; Загрузка селектора в CS для защищённого режима
     db      0EAh   ; код команды дальнего перехода
     dw      $ + 4  ; Смещение
     dw      16      ; Селектор 

     ; Сохранение регистров маски контроллеров
     in      al, 021h
     mov     old_mask1, al   ; запоминаем старые обработчики с ведущего
     in      al, 0A1h
     mov     old_mask2, al   ; и с ведомого
     
     ; Инициализация контроллеров прерываний
     mov     bl, 20h  ; номер обработчика для IRQ0
     mov     bh, 28h  ; номер обработчика для IRQ8    
     call    init_pic        

     mov     al, 11111100b ; Разрешаем прерывания IRQ0 (таймер) и IRQ1 (клавиатура)      
     out     021h, al
     mov     al, 11111111b ; На SLAVE запрещаем все прерывания      
     out     0A1h, al

     ;/////////////////////////////////////////////////////////////
     
     mov di, 160          ; Позиция вывода строки на экране
     lea si, str_scan
     call showString      ; Вывод сообщения "Скан-код: "
     
     mov di, 320          ; Позиция вывода строки на экране
     lea si, str_time
     call showString      ; Вывод сообщения "Осталось до перехода..."
     
     
     xor ecx, ecx       ; обнуление ecx
     mov edx, 50        ; счётчик прерываний таймера (как только 50 - прошла секунда)
     mov cx, intsNum    ; количество прерываний таймера общей длительностью введённого времени
     
     mov bp, 0 ; Если 1 - флаг выхода по Esc
     
     call enable_interrupts ; Включить перывания
     
  wait_:           ; Ожидание, пока не пройдёт нужное количество прерываний таймера
     cmp time, -1   
     jnz test_quit
     jmp toRealMode
  test_quit:
     cmp bp, 0     ; или не будет установлен флаг выхода
     je wait_
     
  toRealMode:    
     call disable_interrupts  ; выключить  прерывания
     
     mov     bl, 08h  ; номер обработчика для IRQ0
     mov     bh, 70h  ; номер обработчика для IRQ8
     call    init_pic ; инициализация обоих контроллеров
     
     mov     al, old_mask1 ; восстановление сохранённых регистров масок
     out     021h, al
     mov     al, old_mask2 
     out     0A1h, al
     
     ;Возвращение в реальный режим
     mov     eax, cr0 ; прочитать CR0
     and     al, 11111110b ; сбросить бит PE
     mov     cr0, eax ; с этого момента мы в реальном режиме   

     ; Загрузка в CS адреса сегмента для реального режима
     db      0EAh   ; код команды дальнего перехода
     dw      $ + 4  ; смещение
     return_cs   dw      0 ; адрес сегмента
     
     lidt fword ptr old_idtr ; восстанавливаем IDTR реального режима
     call enable_interrupts  ; включение прежних прерываний
     
    
     ; выход
         mov        ah,4Ch
         int        21h
     ret

;////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 IRQ0_handler proc   ; обработчик прерываний таймера
     push ax 
     push bx
     push di
         
     inc edx
     cmp edx, 50
     jb continue_count   ; Если меньше 50
     
    second_dec:
     mov di, 352          ; Начало вывода в видеобуфер
     mov edx, 0f200f20h  ; Заменяем пробелами предыдущее значение 
     mov dword ptr es:[di], edx
     mov dword ptr es:[di+4], edx
     mov ax, time ; оставшееся время в секундах
     mov bx, 10 ; система счисления
     call IntToVideoMemory
     dec time

     mov edx, 0
     
    continue_count:  
     ;заврешение обработчика
     mov al, 020h      
     out 020h, al      ;ведущий
     out 0A0h, al      ;ведомый
     
     pop di
     pop bx
     pop ax
     iretd     ;возвращение из обработчика прерываний в защищённом режиме                 
 IRQ0_handler endp

 IRQ1_handler proc   ; обработчик прерываний от клавиатуры
     push ax           ; это аппаратное прерывание - сохранить регистры
     push bx
     push edx
     push cx
     push di
     in   al, 60h           ; прочитать скан-код нажатой клавиши,
     cmp  al, 1             ; если это Esc,
     je   set_Qflag      ; выйти в реальный режим,    
     
     mov di, 182          ; Начало вывода в видеобуфер
     mov edx, 0f200f20h  ; Заменяем пробелами предыдущее значение 
     mov dword ptr es:[di], edx
     
     xor ah, ah
     mov bx, 16 ; система счисления
     call IntToVideoMemory
     jmp handler_quit
     
   set_Qflag:    
     mov bp, 1
     
   handler_quit:
     ; подтверждение приёма скан-кода
     push ax
     in al, 61h
     or al, 80h  ; старший бит байта из порта 61h в 1
     out 61h, al ; подтверждаем факт приема скан-кода
     pop ax
     out 61h, al  ;восстановили байт в порту 61h
     
     ; послать EOI контроллеру прерываний
     mov al, 20h     ;начало заврешения обработчика
     out 20h, al     ;в ведущий
     out 0ah, al     ;в ведомый
     ; восстановить регистры и выйти   
     pop di
     pop cx
     pop edx
     pop bx
     pop ax
     iretd   
 IRQ1_handler endp


; вычислить линейный адрес IDT и запись IDTR  
 i_idt proc  
      mov eax, 0
     mov ax, cs
     shl eax, 4     ; Вычисляем линейный адрес начала массива дескрипторов
     add  eax, offset IDT
     ; Записываем его в структуру
     mov  idtr.base, eax

     ; Загружаем IDTR
     lidt fword ptr idtr
     ret
 i_idt endp

; вычислить линейный адрес GDT и запись GDTR
 i_gdt proc  
     mov eax, 0
     mov ax, cs
     shl eax, 4    ; Вычисляем линейный адрес начала массива дескрипторов
     add  eax, offset GDT
     ; Записываем его в структуру
     mov  gdtr.base, eax

     ; Загружаем GDTR
     lgdt fword ptr gdtr
     ret
 i_gdt endp


     ; Инициализация обоих контроллеров прерываний (тут всё просто)
 init_pic proc       
     push eax
     mov al, 11h  ; ICW1
     out 20h, al  ; в MASTER
     out 0A0h, al ; в SLAVE
     
     mov al, bl   ; ICW2 - номер обработчика для IRQ0
     out 21h, al  ; в MASTER    
     mov al, bh   ; ICW2 - номер обработчика для IRQ8             
     out 0A1h, al ; в SLAVE
     
     mov al, 04h  ; ICW3 - выход 2, к которому подключен SLAVE PIC
     out 21h, al  ; в MASTER    
     mov al, 02h  ; ICW3 - номер выхода ведущего контроллера (2) 
     out 0A1h, al ; в SLAVE
     
     mov al, 01h  ; ICW4 для первого и второго контроллера
     out 021h, al
     out 0A1h, al
     pop eax
     ret
 init_pic endp
     
 disable_interrupts proc  
     push eax
     cli  
     in al, 70h  
     or al, 80h  
     out 70h, al
     pop eax
     ret
 disable_interrupts endp
     
 enable_interrupts proc
     push eax
     in  al, 70h 
     and al, 7Fh  
     out 70h, al
     sti             
     pop eax
     ret
 enable_interrupts endp
     

; Перевод строки в число (by Evgeny3000)
 StrToInt proc near  ; адрес строки в SI, основание сс в BL, длина в CL, результат в AX, флаг ошибки в BP 
     push si 
     push bx
     push dx 
     push cx  
     xor bh, bh    
     xor ax, ax
     mov bp, 0 ; Флаг ошибки по умолчанию = 0
     xor dh, dh
     xor ch, ch      
    next_char:
     mul bx    ; dx:ax=ax*bx
     jb error_  ; Если переполнение AX, то ошибка
     mov dl,[si]  ; очередная цифра
     cmp dl,48  ; (меньше 48) Если введена не цифра
     jb error_ 
     cmp dl,71  ; (больше или равно 71) Если введена буква больше F
     jae testLittle   ; Проверка на маленькие буквы a...f    
     jmp test_  ; Проверка на цифры 0..9 и буквы A..F
     
    testLittle:
     cmp dl,97 ; (меньше 97) Если введен символ, меньший маленькой букве    
     jb error_
     cmp dl,103 ; (больше или равно 103) Если введена маленькая буква больше f
     jae error_
     jmp m_ab ; Если тест удался и введена маленькая буква
     
    test_:    
     cmp dl,58 ; (меньше 58) Если цифры 0..9
     jb m_1    
     cmp dl,65  ; (больше или равно 65) если цифры A...F
     jae m_A
  
  ; В промежутке между 58 и 64 - error
    error_:
       mov ax,0 ; Результат = 0 при ошибке
       mov bp,1 ; флаг ошибки = 1   
       jmp to_exit
     
    m_1:  sub dl,48 
          jmp next_
    m_A:  sub dl,55 
         jmp next_
    m_ab: sub dl,87
   
    next_: 
         cmp dl,bl ; dl - очередная цифра, bl - основание сс
         jae error_ ; Если цифра больше либо равна основанию СС
         add ax,dx  
         jb error_ ; Если возникает переполнение, то ошибка
         inc si
    loop next_char         
    to_exit:    
       pop cx 
       pop dx 
       pop bx  
       pop si
       ret
 StrToInt endp 

; Число в виде строки в видеопамять (by Evgeny3000)
 IntToVideoMemory proc   ; число в ax
     push cx
     push dx
     push ax 
     xor cx,cx
   again2:
     sub dx,dx
     div bx ; dx:ax/bx -  результат деления в ax, остаток в dx
     inc cx ; считаем цифры
     push dx ; остаток в стек
     cmp ax,0 ; результат равен нулю?
     jne again2 ; если нет, то повтор 
   toBuffer:
     pop dx ; извлекаем цифру из стека
     add dx,30h
     cmp dx,39h
     jbe no_more_9
     add dx,7 ; добавляем для отображения букв
     
   no_more_9:
     mov dh, 01001110b ; атрибут символа
     mov word ptr es:[di], dx    
     inc di
     inc di  
     loop toBuffer

     pop ax
     pop dx
     pop cx  
     ret
 IntToVideoMemory endp


; Вывод строки в видеопамять    
 showString proc near   ; в si - указатель на строку, в di - позиция на экране  ; в es - ук. на видеобуфер
     ; возвр. значения: в di - позиция конца строки на экране
     cmp byte ptr [si],'$'
     je exit_
     push si
     push dx
     
   again:     
     mov dh, 15 ; атрибут символа
     mov dl, byte ptr [si]    
     mov es:[di], dx     
     inc si    
     inc di
     inc di     
     cmp byte ptr [si],'$'
     jne again
     
     pop dx
     pop si  
    exit_:  
     ret
 showString endp  

;////////////////////////////////////////////////////////////////////////////////
     ; Глобальная таблица дескрипторов
     GDT     label byte
      ;нулевой дескриптор, cелектор 0
     segment_descriptor <>
	 segment_descriptor <>
     ; Дескриптор сегмента кода, лимит FFFFh = 64 Kb, селектор 16
	 CS64 segment_descriptor <0ffffh, 0, 0, 9Ah, 0, 0>
     ; Данные для загрузки в GDTR
     gdtr    table_register <$ - GDT - 1, 0>  ; инициализируем лимит GDT 

     ; Таблица дескрипторов прерываний
     IDT     label byte
     db 32 dup (  8 dup (0)) ; Зарезервированные прерывания защищённого режима (0 - 1Fh) 
     gate_descriptor<IRQ0_handler, 16, 0, 10001110b, 0>  ; 32-битный шлюз прерывания, 20h 
     gate_descriptor<IRQ1_handler, 16, 0, 10001110b, 0>  ; 32-битный шлюз прерывания, 21h

     ; Данные для загрузки в IDTR
     idtr    table_register <$ - IDT - 1, 0>

     ; Место для хранения IDTR реального режима
     old_idtr table_register <>
     old_mask1 db 0  
     old_mask2 db 0  

     time    dw  ?   
     intsNum dw ?
     str_1   db  "Set delay to: ", '$'
     str_scan db "Scan-code: $"
     str_time db "Remaining time: $"
     delay_str   db 30, ?, 30 dup('$')

end start