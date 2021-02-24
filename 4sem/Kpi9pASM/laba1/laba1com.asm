; laba 1 .COM
.model small
.code
org 100h

start:
mov ah,9
mov dx, offset messageHello
int 21h

ret
   
messageHello db "Hello World!!! =) (.COM)", 0Dh,0Ah, '$' 
end start
                           