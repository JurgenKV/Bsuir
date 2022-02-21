#include "Matrix.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <intrin.h>

#pragma intrinsic(__rdtsc)
using namespace std;

int main() {
	Matrix matrix;

	T**** matrixA = matrix.allocation(MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);
	T**** matrixB = matrix.allocation(MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);
	T**** matrixC;
	T**** matrixD;

	matrix.generateValues(matrixA, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);
	matrix.generateValues(matrixB, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);

	ULONGLONG resultTimeSSE;
	ULONGLONG resultTime;
	ULONGLONG resultTimeNotVectorized;

	ULONGLONG endTime;

	ULONGLONG startTime = __rdtsc();
	matrixC = matrix.multiply(matrixA, matrixB, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);
	endTime = GetTickCount64();

	resultTime = endTime - startTime;

	startTime = GetTickCount64();
	matrixD = matrix.multiplySSE(matrixA, matrixB, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);
	endTime = GetTickCount64();


	resultTimeSSE = endTime - startTime;

	startTime = GetTickCount64();
	matrixD = matrix.multiplyNotVectorized(matrixA, matrixB, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);
	endTime = GetTickCount64();

	resultTimeNotVectorized = endTime - startTime;

	cout << "Victorization Ticks: " << resultTime << "." << endl;
	cout << "Not Victorized Ticks: " << resultTimeNotVectorized << "." << endl;
	cout << "SSE Ticks: " << resultTimeSSE << ".\n" << endl;

	cout << "Vectorized speed up: x" << (double)resultTimeNotVectorized / (double)resultTime << "." << endl;
	cout << "SSE speed up: x" << (double)resultTimeNotVectorized / (double)resultTimeSSE << ".\n" << endl;

	if (matrix.equals(matrixC, matrixD, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH)) {
		cout << "Matrices by ASM and by Victorization are equal." << endl;
	}
	else {
		cout << "Matrices by ASM and by Victorization are not equal." << endl;
	}

	matrix.destroy(matrixA, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);
	matrix.destroy(matrixB, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);
	matrix.destroy(matrixC, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);
	matrix.destroy(matrixD, MATRIX_HEIGHT, MATRIX_WIDTH, CELL_HEIGHT, CELL_WIDTH);

	system("pause");
	return 0;
}