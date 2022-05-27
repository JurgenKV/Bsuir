#ifndef KERNELS_H
#define KERNELS_H

#include "filtratomatic.h"
#include "hff.cuh"

__global__ void kernel(
	unsigned char* input,
	unsigned char* output,
	int width,
	int dimY,
	int dimX
);

__global__ void getChannelsGPU(
	unsigned char* sourceImageData,
	int dimX, // px
	int dimY, // px
	unsigned char* rs,
	unsigned char* gs,
	unsigned char* bs
);

__global__ void compileChannelsGPU(
	unsigned char* r,
	unsigned char* g,
	unsigned char* b,
	int dimX,
	int dimY,
	unsigned char* output
);

#endif