#pragma once
#include <stdlib.h>
#include <time.h>
#include <emmintrin.h>

#define MATRIX_HEIGHT	2
#define MATRIX_WIDTH 	5

#define CELL_HEIGHT	300
#define CELL_WIDTH 	300

typedef double T;

class Matrix {
public:
	Matrix() {}
	~Matrix() {}

	T**** allocation(int, int, int, int);
	bool generateValues(T****, int, int, int, int);
	T**** multiply(T****, T****, int, int, int, int);
	T**** multiplyNotVectorized(T****, T****, int, int, int, int);
	T**** multiplySSE(T****, T****, int, int, int, int);
	bool equals(T****, T****, int, int, int, int);
	bool destroy(T****, int, int, int, int);
};