#include <io.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

char data[6]; // ������ �����
unsigned int delayTime = 0;
unsigned int registerArray[] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09 };

void interrupt newTime(...);  // ����� ���������� ���������� �����
void interrupt newAlarm(...); // ����� ���������� ���������� ����������
void interrupt(*lastTime)(...); // ������ ���������� �����
void interrupt(*lastAlarm) (...); // ������ ���������� ����������

char* months[] =
{
    "JANUARY",
    "FEBRUARY",
    "MARCH",
    "APRIL",
    "MAY",
    "JUNE",
    "JULY",
    "AUGUST",
    "SEPTEMBER",
    "OCTOBER",
    "NOVEMBER",
    "DECEMBER"
};

void menu();
void setTime();
void showTime();
void setAlarm();
void enterTime();
void resetAlarm();
int convertToBCD(int);
void delay(unsigned int);
int convertToDecimal(int);

int main()
{
    while (1)
    {
        menu();
        switch (getch())
        {
        case '1':
            system("cls");
            showTime();
            break;

        case '2':
            system("cls");
            int delayms;
            printf("INPUT DELAY (MS): ");
            scanf("%d", &delayms);
            delay(delayms);
            break;

        case '3':
            system("cls");
            setAlarm();
            break;

        case '4':
            system("cls");
            setTime();
            break;

        case '5':
            return 0;

        default:
            system("cls");
            break;
        }
    }
}

void menu()
{
    puts("----------------------");
    puts("1. SHOW TIME");
    puts("----------------------");
    puts("2. SET DELAY");
    puts("----------------------");
    puts("3. SET ALARM");
    puts("----------------------");
    puts("4. SET TIME");
    puts("----------------------");
    puts("5. EXIT");
    puts("----------------------");
}

void showTime()
{

    int i = 0;
    for (i = 0; i < 6; i++)
    {
        outp(0x70, registerArray[i]); // ����� ������ � ������ CMOS
        data[i] = inp(0x71); // ���������� �������� �� ������ � ������
    }

    int decimalData[6]; // ������� �������� � ���������� ���
    for (i = 0; i < 6; i++)
    {
        decimalData[i] = convertToDecimal(data[i]);
    }

    // ����� �� �����
    printf("%2d:%2d:%2d\n", decimalData[2], decimalData[1], decimalData[0]); // ���, ������, �������
    printf("%2d %s 20%2d\n", decimalData[3], months[decimalData[4] - 1], decimalData[5]); // ����, �����, ���
}

void setTime()
{
    enterTime(); // ���� ������ �������
    disable(); // ������ �� ����������

    // �������� �� ����������� �������� ��� ������/������
    unsigned int check;
    do
    {
        outp(0x70, 0xA); // ����� �������� �
        check = inp(0x71) & 0x80; // 0x80 - 1000 0000
        // 7-� ��� � 1 ��� ���������� �������
    } while (check);

    // ���������� ���������� ����� ��������� �������
    outp(0x70, 0xB); // ����� �������� B
    outp(0x71, inp(0x71) | 0x80); // 0x80 - 1000 0000
    // 7-� ��� � 1 ��� ������� ���������� �����

    for (int i = 0; i < 6; i++)
    {
        outp(0x70, registerArray[i]); // ����� ������� �������� ������
        outp(0x71, data[i]); // ������ � ������� ������� ��������
    }

    // ��������� ���������� ����� ��������� �������
    outp(0x70, 0xB); // ����� �������� �
    outp(0x71, inp(0x71) & 0x7F); // 0x7F - 0111 1111
    // 7-� ��� � 0 ��� ���������� ���������� �����

    enable(); // ���������� �� ����������
    system("cls");
}

void delay(unsigned int delayms)
{
    disable(); // ������ �� ����������

    // ��������� ������ ����������� ����������
    lastTime = getvect(0x70);
    setvect(0x70, newTime);

    enable(); // ���������� �� ����������

    // ������������� ����� ������� ������� �� ���
    // 0xA1 - ����� �������� �������� ��� ���������� �������
    outp(0xA1, inp(0xA1) & 0xFE); // 0xFE = 1111 1110
    // 0-� ��� � 0 ��� ���������� ���������� �� ��� 

    outp(0x70, 0xB); // ����� �������� �
    outp(0x71, inp(0x71) | 0x40); // 0x40 = 0100 0000
    // 6-� ��� �������� B ���������� � 1 ��� �������������� ����������

    delayTime = 0;
    while (delayTime <= delayms);
    puts("DELAY IS OVER");
    setvect(0x70, lastTime);
    return;
}

void setAlarm()
{
    enterTime(); // ���� ������ �������
    disable(); // ������ �� ����������

    // �������� �� ����������� �������� ��� ������/������
    unsigned int check;
    do
    {
        outp(0x70, 0xA); // ����� �������� A
        check = inp(0x71) & 0x80; // 0x80 - 1000 0000
        // 7-� ��� � 1 ��� ���������� �������
    } while (check);

    // ��������� ����� � ������� ����������
    outp(0x70, 0x05);
    outp(0x71, data[2]);

    // ��������� ����� � ������� ����������
    outp(0x70, 0x03);
    outp(0x71, data[1]);

    // ��������� ������ � ������� ����������
    outp(0x70, 0x01);
    outp(0x71, data[0]);

    outp(0x70, 0xB); // ����� �������� B
    outp(0x71, (inp(0x71) | 0x20)); // 0x20 - 0010 0000
    // 5-� ��� �������� B ���������� � 1 ��� ���������� ���������� ����������

    // ��������������� ���������� ����������
    lastAlarm = getvect(0x4A); // 0x4A - 1001 010 (���������� �������)
    setvect(0x4A, newAlarm); // 0x4A - ������� ���� � ����� � ������� BCD
    outp(0xA1, (inp(0xA0) & 0xFE)); // 0xFE - 1111 1110
    // 0-� ��� � 0 ��� ���������� ���������� �� ���

    enable(); // ���������� �� ����������
    puts("ALARM IS ON");
}

void resetAlarm()
{
    // �������� �� ������� �������������� ����������
    if (lastAlarm == NULL)
        return;

    disable(); // ������ �� ����������

    // ������� ������� ����������
    setvect(0x4A, lastAlarm);  // 0x4A - ������� ���� � ����� � ������� BCD
    outp(0xA1, (inp(0xA0) | 0x01)); // 0x01 - 0000 0001 (�������� ������� ����������)

    // �������� �� ����������� �������� ��� ������/������
    unsigned int check;
    do
    {
        outp(0x70, 0xA); // ����� �������� A
        check = inp(0x71) & 0x80;  // 0x80 - 1000 0000
        // 7-� ��� � 1 ��� ���������� �������
    } while (check);

    // ������ ������� �������� � ������� ����������
    outp(0x70, 0x05); // 0x05 - ����
    outp(0x71, 0x00);

    outp(0x70, 0x03); // 0x03 - ������
    outp(0x71, 0x00);

    outp(0x70, 0x01); // 0x01 - �������
    outp(0x71, 0x00);

    outp(0x70, 0xB); // ����� �������� B
    outp(0x71, (inp(0x71) & 0xDF)); // 0xDF - 1101 1111
    // 5-� ��� � 0 ��� ������� ���������� ����������

    enable(); // ���������� �� ����������
}

void enterTime()
{
    int enter;

    do
    {
        rewind(stdin);
        printf("ENTER YEAR: ");
        scanf("%i", &enter);
    } while ((enter > 2100 || enter < 0));
    data[5] = convertToBCD(enter);
    do
    {
        rewind(stdin);
        printf("ENTER MONTH: ");
        scanf("%i", &enter);
    } while ((enter > 12 || enter < 1));
    data[4] = convertToDecimal(enter);

    do
    {
        rewind(stdin);
        printf("ENTER DAY: ");
        scanf("%i", &enter);
    } while ((enter > 365 || enter < 1));
    data[3] = convertToBCD(enter);

    do
    {
        rewind(stdin);
        printf("ENTER HOURS: ");
        scanf("%i", &enter);
    } while ((enter > 23 || enter < 0));
    data[2] = convertToBCD(enter);

    do
    {
        rewind(stdin);
        printf("ENTER MINUTES: ");
        scanf("%i", &enter);
    } while (enter > 59 || enter < 0);
    data[1] = convertToBCD(enter);

    do
    {
        rewind(stdin);
        printf("ENTER SECONDS: ");
        scanf("%i", &enter);
    } while (enter > 59 || enter < 0);
    data[0] = convertToBCD(enter);
}

int convertToDecimal(int BCD)
{
    return ((BCD / 16 * 10) + (BCD % 16));
}

int convertToBCD(int decimal)
{
    return ((decimal / 10 * 16) + (decimal % 10));
}

void interrupt newTime(...) // ����� ���������� ���������� �����
{
    delayTime++;
    outp(0x70, 0x0C); // ����� ������ � ������ CMOS (������)
    inp(0x71); // ������ �� ����� ������ (������/������)
    // ����� ������� ������������ ���������� �� ��������� ����������
    outp(0x20, 0x20);
    outp(0xA0, 0x20);
}

void interrupt newAlarm(...) // ����� ���������� ���������� ����������
{
    puts("ALARM! ALARM! ALARM!");
    lastAlarm();
    resetAlarm();
}