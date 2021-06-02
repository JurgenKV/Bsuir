#include <dos.h>

#include <conio.h>

#include <stdio.h>

const unsigned char QUIT_CHAR = 0x81;

const int TARGET_INTERRUPT = 9;

const int true = 1;

const int false = 0;

union REGS i, o;

int check_alt();

void call_int_16(unsigned char, unsigned char );

void interrupt NewInterrupt(void);

void interrupt (*oldInterrupt)(void);

void SaveOldInterrupt();

void SetNewInterrupt();

void RestoreOldInterrupt();

void WaitInputFree();

void keyboard_input(void);

void EndInterrupt();

int commandIsExecuted;

int quitFlag;

int keyboard_input_flag;

void main()

{

unsigned far *fp;

FP_SEG(fp) = _psp;

FP_OFF(fp) = 0x2c;

_dos_freemem(*fp);

commandIsExecuted = false;

quitFlag = 0;

keyboard_input_flag = false;

clrscr();

SaveOldInterrupt();

SetNewInterrupt();

while(!quitFlag)

{

if (keyboard_input_flag)

{

char buf[10];

//keyboard_input();

delay(1000);

scanf("%s", buf);

printf("%s", buf);

if (buf[0] = 27)

break;

keyboard_input_flag = 0;

}

}

//RestoreOldInterrupt();

clrscr();

_dos_keep(0, (_DS - _CS) + (_SP / 16) + 1);

return;

}

void SaveOldInterrupt()

{

oldInterrupt = getvect(TARGET_INTERRUPT);

}

void SetNewInterrupt()

{

setvect(TARGET_INTERRUPT, NewInterrupt);

}

void RestoreOldInterrupt()

{

setvect(TARGET_INTERRUPT, oldInterrupt);

}

void interrupt NewInterrupt()

{

unsigned char value = 0;

oldInterrupt();

value = inp(0x60);

if (value == QUIT_CHAR)

{

quitFlag = 1;

}

if (value == 0x02+0x80)

{

if (check_alt())

{

keyboard_input_flag = 1;

keyboard_input();

}

}

printf("%x ", value);

EndInterrupt();

}

int check_alt() //проверка на нажатие alt

{

i.h.ah = 0x12;//для 102 клавиш

int86(0x16, &i, &o);

if (o.h.al == 0x8)

return 1;

else

return 0;

}

void call_int_16(unsigned char scan , unsigned char ASCII)

{

i.h.ah = 0x5;

i.h.ch = scan;

i.h.cl = ASCII;

int86(0x16, &i, &o);

}

void EndInterrupt()

{

outp(0x20, 0x20);

}

void WaitInputFree()

{

while((inp(0x64) & 0x02) != 0x00);

}

void keyboard_input()

{

call_int_16(0x10, 'q');

delay(1000);

call_int_16(0x11, 'w');

delay(1000);

call_int_16(0x12, 'e');

delay(1000);

call_int_16(0, ' ');

}