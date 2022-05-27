#ifndef HFF_CUH
#define HFF_CUH

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "helper_image.h"

#include <windows.h>

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <chrono>


#define BLOCK_DIM_X 128
#define BLOCK_DIM_Y 8


const char HF_H1_KERNEL[3][3] =
{
	{ 1, -2, 1 },
	{ -2, 5, -2 },
	{1, -2, 1 },
};

__device__ __constant__ char HF_H1_DEVICE_KERNEL[3][3] =
{
	{ 1, -2, 1 },
	{ -2, 5, -2 },
	{1, -2, 1 },
};

#endif