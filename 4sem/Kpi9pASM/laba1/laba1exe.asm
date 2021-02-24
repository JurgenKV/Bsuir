; laba 1 .EXE
.model small
.stack 100h
.code

start:
mov ax, @data
mov ds,ax
mov dx,offset messg
mov ah,9
int 21h
mov ax,4C00h
int 21h

.data   
messg db "Hello World!!! =) (.exe)", 0Dh,0Ah, '$' 
end start
                           