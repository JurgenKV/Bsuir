#include "stdio.h"
#include "windows.h"
#include "locale.h"
#define SIZE 4

int main() {
	setlocale(LC_ALL, "russian");
	int ms1[SIZE][SIZE], ms2[SIZE][SIZE], ms3[SIZE][SIZE];
	int time0, time1, time2, time3, time4;
	long cnt = 16;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			ms1[i][j] = i + j + 1;
		}
	}
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			ms2[i][j] = i + j + 2;
		}
	}

	printf("Матрицы: \n");
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%d ", ms1[i][j]);
		}
		printf("\n");
	}
	printf("\n===============================\n");

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%d ", ms2[i][j]);
		}
		printf("\n");
	}

	time0 = GetTickCount();
	printf("Time = %d ms\n", time0);

	printf("\nC:\n");
	for (int i = 0; i < 10000000; i++)
	{
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				ms3[i][j] = ms1[i][j] * ms2[i][j];
			}
		}
	}

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%d ", ms3[i][j]);
		}
		printf("\n");
	}


	printf("Матрицы умножены\n");
	time1 = GetTickCount() - time0;
	printf("Time = %d ms\n", time1);
	
	int res = 0;
	printf("\nMMX:\n");
	for (int i = 0; i < 10000000; i++)
	{
		
		cnt = 16;
		__asm
		{
			
			push esi
			mov ecx, [cnt]
			lea esi, [ms1 + ecx * 2]
			neg ecx
			pcmpeqd mm2, mm2
			psrlw mm2, 15; mm2 = (1, 1, 1, 1)
			pxor mm0, mm0
			b :
			movq mm1, [esi + ecx * 2]
				pmaddwd mm1, mm2
				pmullw mm0, mm1
				add ecx, 4
				jnz b
				movq mm1, mm0
				psrlq mm1, 32
				pmullw mm0, mm1
				movd[res], mm0
				emms
				pop esi
		}

	}
	
	printf("Матрицы умножены\n");
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%d ", ms3[i][j]);
		}
		printf("\n");
	}
	time2 = GetTickCount() - time0 - time1;
	printf("Time = %d ms\n", time2);

	int ms4[SIZE][SIZE];
	printf("\nAssembly:\n");
	for (int i = 0; i < 10000000; i++)
	{
		cnt = 16;
		__asm
		{
				push eax
				push ecx
				push esi
				xor esi, esi
				xor ecx, ecx
				loop2:
				movsx eax, ms1[esi]
					mov ms4[esi], eax
					imul ecx, eax
					add esi, 2
					sub cnt, 1
					jnz loop2
					pop esi
					pop ecx
					pop eax 
		}
		
	}

	printf("Матрицы умножены\n");
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%d ", ms3[i][j]);
		}
		printf("\n");
	}

	time3 = GetTickCount() - time0 - time1 - time2;
	printf("Time = %d ms\n", time3);

	printf("\nXMM:\n");
	for (int i = 0; i < 10000000; i++)
	{
		cnt = 16;
		__asm
		{

			push esi
			mov ecx, [cnt]
			lea esi, [ms1 + ecx * 2]
			neg ecx
			pcmpeqd xmm2, xmm2
			psrlw xmm2, 15; xmm2 = (1, 1, 1, 1)
			pxor xmm0, xmm0
			b1 :
			movups xmm1, [esi + ecx * 2]
				pmaddwd xmm1, xmm2
				pmullw xmm0, xmm1
				add ecx, 4
				jnz b1
				movq xmm1, xmm0
				psrlq xmm1, 32
				pmullw xmm0, xmm1
				movd[res], xmm0
				emms
				pop esi
		}
	}
	printf("Матрицы умножены\n");
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%d ", ms3[i][j]);
		}
		printf("\n");
	}
	time4 = GetTickCount() - time0 - time1 - time2 - time3;
	printf("Time = %d ms\n", time4);
	return 0;
}