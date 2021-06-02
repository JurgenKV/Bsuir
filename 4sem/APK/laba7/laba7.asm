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

;��������� (by maxvale)
     segment_descriptor struc    ;���������� ����������
     limit_low   dw     0    ; ������� 2 ����� ���� Segment limit
     base_low    dw     0    ; ������� 2 ����� ���� Base Adress
     base_high0  db     0    ; ������ ���� ���� Base Address
     access_rights db   0    ; ��� ��������, ��� �����������, DPL, ��� ����������� ��������
     flags       db     0    ; ���� 19-16 ������, ��� �����������, ��� �������������
     base_high1  db     0    ; ������� ���� ���� Base Address
     segment_descriptor ends

     ; ���������� �����
     gate_descriptor struc
     offset_low    dw   0    ; ������� ����� ��������
     selector      dw   0    ; �������� �������� ����
     zero          db   0
     access_rights db   0    ; ����� ������� 
     offset_high   dw   0    ; ������� ����� �������� 
     gate_descriptor ends

     ; �������, ����������� �������
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
     
     ; ���� ������� � ��������
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

     ; ��������� IDTR ��������� ������
     sidt fword ptr old_idtr 

     ; ��������� ����������
     call disable_interrupts 
     
     ; �������������� GDT
     call i_gdt
     ; �������������� IDT
     call i_idt
     
     ;����������� �����
     mov     eax, cr0 ; ��������� ������� CR0
     or      al, 00000001b    ; ���������� ��� PE,
     mov     cr0, eax ; � ����� ������� �� � ���������� ������
     
     ; �������� ��������� � CS ��� ����������� ������
     db      0EAh   ; ��� ������� �������� ��������
     dw      $ + 4  ; ��������
     dw      16      ; �������� 

     ; ���������� ��������� ����� ������������
     in      al, 021h
     mov     old_mask1, al   ; ���������� ������ ����������� � ��������
     in      al, 0A1h
     mov     old_mask2, al   ; � � ��������
     
     ; ������������� ������������ ����������
     mov     bl, 20h  ; ����� ����������� ��� IRQ0
     mov     bh, 28h  ; ����� ����������� ��� IRQ8    
     call    init_pic        

     mov     al, 11111100b ; ��������� ���������� IRQ0 (������) � IRQ1 (����������)      
     out     021h, al
     mov     al, 11111111b ; �� SLAVE ��������� ��� ����������      
     out     0A1h, al

     ;/////////////////////////////////////////////////////////////
     
     mov di, 160          ; ������� ������ ������ �� ������
     lea si, str_scan
     call showString      ; ����� ��������� "����-���: "
     
     mov di, 320          ; ������� ������ ������ �� ������
     lea si, str_time
     call showString      ; ����� ��������� "�������� �� ��������..."
     
     
     xor ecx, ecx       ; ��������� ecx
     mov edx, 50        ; ������� ���������� ������� (��� ������ 50 - ������ �������)
     mov cx, intsNum    ; ���������� ���������� ������� ����� ������������� ��������� �������
     
     mov bp, 0 ; ���� 1 - ���� ������ �� Esc
     
     call enable_interrupts ; �������� ���������
     
  wait_:           ; ��������, ���� �� ������ ������ ���������� ���������� �������
     cmp time, -1   
     jnz test_quit
     jmp toRealMode
  test_quit:
     cmp bp, 0     ; ��� �� ����� ���������� ���� ������
     je wait_
     
  toRealMode:    
     call disable_interrupts  ; ���������  ����������
     
     mov     bl, 08h  ; ����� ����������� ��� IRQ0
     mov     bh, 70h  ; ����� ����������� ��� IRQ8
     call    init_pic ; ������������� ����� ������������
     
     mov     al, old_mask1 ; �������������� ���������� ��������� �����
     out     021h, al
     mov     al, old_mask2 
     out     0A1h, al
     
     ;����������� � �������� �����
     mov     eax, cr0 ; ��������� CR0
     and     al, 11111110b ; �������� ��� PE
     mov     cr0, eax ; � ����� ������� �� � �������� ������   

     ; �������� � CS ������ �������� ��� ��������� ������
     db      0EAh   ; ��� ������� �������� ��������
     dw      $ + 4  ; ��������
     return_cs   dw      0 ; ����� ��������
     
     lidt fword ptr old_idtr ; ��������������� IDTR ��������� ������
     call enable_interrupts  ; ��������� ������� ����������
     
    
     ; �����
         mov        ah,4Ch
         int        21h
     ret

;////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 IRQ0_handler proc   ; ���������� ���������� �������
     push ax 
     push bx
     push di
         
     inc edx
     cmp edx, 50
     jb continue_count   ; ���� ������ 50
     
    second_dec:
     mov di, 352          ; ������ ������ � ����������
     mov edx, 0f200f20h  ; �������� ��������� ���������� �������� 
     mov dword ptr es:[di], edx
     mov dword ptr es:[di+4], edx
     mov ax, time ; ���������� ����� � ��������
     mov bx, 10 ; ������� ���������
     call IntToVideoMemory
     dec time

     mov edx, 0
     
    continue_count:  
     ;���������� �����������
     mov al, 020h      
     out 020h, al      ;�������
     out 0A0h, al      ;�������
     
     pop di
     pop bx
     pop ax
     iretd     ;����������� �� ����������� ���������� � ���������� ������                 
 IRQ0_handler endp

 IRQ1_handler proc   ; ���������� ���������� �� ����������
     push ax           ; ��� ���������� ���������� - ��������� ��������
     push bx
     push edx
     push cx
     push di
     in   al, 60h           ; ��������� ����-��� ������� �������,
     cmp  al, 1             ; ���� ��� Esc,
     je   set_Qflag      ; ����� � �������� �����,    
     
     mov di, 182          ; ������ ������ � ����������
     mov edx, 0f200f20h  ; �������� ��������� ���������� �������� 
     mov dword ptr es:[di], edx
     
     xor ah, ah
     mov bx, 16 ; ������� ���������
     call IntToVideoMemory
     jmp handler_quit
     
   set_Qflag:    
     mov bp, 1
     
   handler_quit:
     ; ������������� ����� ����-����
     push ax
     in al, 61h
     or al, 80h  ; ������� ��� ����� �� ����� 61h � 1
     out 61h, al ; ������������ ���� ������ ����-����
     pop ax
     out 61h, al  ;������������ ���� � ����� 61h
     
     ; ������� EOI ����������� ����������
     mov al, 20h     ;������ ���������� �����������
     out 20h, al     ;� �������
     out 0ah, al     ;� �������
     ; ������������ �������� � �����   
     pop di
     pop cx
     pop edx
     pop bx
     pop ax
     iretd   
 IRQ1_handler endp


; ��������� �������� ����� IDT � ������ IDTR  
 i_idt proc  
      mov eax, 0
     mov ax, cs
     shl eax, 4     ; ��������� �������� ����� ������ ������� ������������
     add  eax, offset IDT
     ; ���������� ��� � ���������
     mov  idtr.base, eax

     ; ��������� IDTR
     lidt fword ptr idtr
     ret
 i_idt endp

; ��������� �������� ����� GDT � ������ GDTR
 i_gdt proc  
     mov eax, 0
     mov ax, cs
     shl eax, 4    ; ��������� �������� ����� ������ ������� ������������
     add  eax, offset GDT
     ; ���������� ��� � ���������
     mov  gdtr.base, eax

     ; ��������� GDTR
     lgdt fword ptr gdtr
     ret
 i_gdt endp


     ; ������������� ����� ������������ ���������� (��� �� ������)
 init_pic proc       
     push eax
     mov al, 11h  ; ICW1
     out 20h, al  ; � MASTER
     out 0A0h, al ; � SLAVE
     
     mov al, bl   ; ICW2 - ����� ����������� ��� IRQ0
     out 21h, al  ; � MASTER    
     mov al, bh   ; ICW2 - ����� ����������� ��� IRQ8             
     out 0A1h, al ; � SLAVE
     
     mov al, 04h  ; ICW3 - ����� 2, � �������� ��������� SLAVE PIC
     out 21h, al  ; � MASTER    
     mov al, 02h  ; ICW3 - ����� ������ �������� ����������� (2) 
     out 0A1h, al ; � SLAVE
     
     mov al, 01h  ; ICW4 ��� ������� � ������� �����������
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
     

; ������� ������ � ����� (by Evgeny3000)
 StrToInt proc near  ; ����� ������ � SI, ��������� �� � BL, ����� � CL, ��������� � AX, ���� ������ � BP 
     push si 
     push bx
     push dx 
     push cx  
     xor bh, bh    
     xor ax, ax
     mov bp, 0 ; ���� ������ �� ��������� = 0
     xor dh, dh
     xor ch, ch      
    next_char:
     mul bx    ; dx:ax=ax*bx
     jb error_  ; ���� ������������ AX, �� ������
     mov dl,[si]  ; ��������� �����
     cmp dl,48  ; (������ 48) ���� ������� �� �����
     jb error_ 
     cmp dl,71  ; (������ ��� ����� 71) ���� ������� ����� ������ F
     jae testLittle   ; �������� �� ��������� ����� a...f    
     jmp test_  ; �������� �� ����� 0..9 � ����� A..F
     
    testLittle:
     cmp dl,97 ; (������ 97) ���� ������ ������, ������� ��������� �����    
     jb error_
     cmp dl,103 ; (������ ��� ����� 103) ���� ������� ��������� ����� ������ f
     jae error_
     jmp m_ab ; ���� ���� ������ � ������� ��������� �����
     
    test_:    
     cmp dl,58 ; (������ 58) ���� ����� 0..9
     jb m_1    
     cmp dl,65  ; (������ ��� ����� 65) ���� ����� A...F
     jae m_A
  
  ; � ���������� ����� 58 � 64 - error
    error_:
       mov ax,0 ; ��������� = 0 ��� ������
       mov bp,1 ; ���� ������ = 1   
       jmp to_exit
     
    m_1:  sub dl,48 
          jmp next_
    m_A:  sub dl,55 
         jmp next_
    m_ab: sub dl,87
   
    next_: 
         cmp dl,bl ; dl - ��������� �����, bl - ��������� ��
         jae error_ ; ���� ����� ������ ���� ����� ��������� ��
         add ax,dx  
         jb error_ ; ���� ��������� ������������, �� ������
         inc si
    loop next_char         
    to_exit:    
       pop cx 
       pop dx 
       pop bx  
       pop si
       ret
 StrToInt endp 

; ����� � ���� ������ � ����������� (by Evgeny3000)
 IntToVideoMemory proc   ; ����� � ax
     push cx
     push dx
     push ax 
     xor cx,cx
   again2:
     sub dx,dx
     div bx ; dx:ax/bx -  ��������� ������� � ax, ������� � dx
     inc cx ; ������� �����
     push dx ; ������� � ����
     cmp ax,0 ; ��������� ����� ����?
     jne again2 ; ���� ���, �� ������ 
   toBuffer:
     pop dx ; ��������� ����� �� �����
     add dx,30h
     cmp dx,39h
     jbe no_more_9
     add dx,7 ; ��������� ��� ����������� ����
     
   no_more_9:
     mov dh, 01001110b ; ������� �������
     mov word ptr es:[di], dx    
     inc di
     inc di  
     loop toBuffer

     pop ax
     pop dx
     pop cx  
     ret
 IntToVideoMemory endp


; ����� ������ � �����������    
 showString proc near   ; � si - ��������� �� ������, � di - ������� �� ������  ; � es - ��. �� ����������
     ; �����. ��������: � di - ������� ����� ������ �� ������
     cmp byte ptr [si],'$'
     je exit_
     push si
     push dx
     
   again:     
     mov dh, 15 ; ������� �������
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
     ; ���������� ������� ������������
     GDT     label byte
      ;������� ����������, c������� 0
     segment_descriptor <>
	 segment_descriptor <>
     ; ���������� �������� ����, ����� FFFFh = 64 Kb, �������� 16
	 CS64 segment_descriptor <0ffffh, 0, 0, 9Ah, 0, 0>
     ; ������ ��� �������� � GDTR
     gdtr    table_register <$ - GDT - 1, 0>  ; �������������� ����� GDT 

     ; ������� ������������ ����������
     IDT     label byte
     db 32 dup (  8 dup (0)) ; ����������������� ���������� ����������� ������ (0 - 1Fh) 
     gate_descriptor<IRQ0_handler, 16, 0, 10001110b, 0>  ; 32-������ ���� ����������, 20h 
     gate_descriptor<IRQ1_handler, 16, 0, 10001110b, 0>  ; 32-������ ���� ����������, 21h

     ; ������ ��� �������� � IDTR
     idtr    table_register <$ - IDT - 1, 0>

     ; ����� ��� �������� IDTR ��������� ������
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