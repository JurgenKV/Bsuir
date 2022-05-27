#include "kernels.cuh"


__global__ void kernel(
	unsigned char* input,
	unsigned char* output,
	int width,
	int dimY,
	int dimX
) {

	__shared__ uint32_t* iInput;

	const int xIdx = blockIdx.x * BLOCK_DIM_X + threadIdx.x;
	const int yIdx = blockIdx.y * BLOCK_DIM_Y + threadIdx.y;

	if (xIdx >= width || yIdx >= dimY)
	{
		return;
	}

	iInput = (uint32_t*)input;
	uint32_t q = 0, res = 0, offset = 0;
	int absoluteX, absoluteY, intPos = 0;
	for (int byteIdx = 0; byteIdx < 4; byteIdx++)
	{
		int positionBytes = xIdx * 4 + byteIdx;
		unsigned char subres = 0;
		for (int j = 0; j < 3; j++)
		{
			absoluteY = yIdx + j - 1;
			if (absoluteY < 0 || absoluteY >= dimY)
			{
				absoluteY = yIdx;
			}

			for (int i = 0; i < 3; i++)
			{
				absoluteX = positionBytes + i - 1;
				if (absoluteX < 0 || absoluteX >= dimX)
				{
					absoluteX = positionBytes;
				}
				offset = absoluteX / 4;
				intPos = absoluteX % 4;
				q = iInput[offset + absoluteY * width];
				subres += ((unsigned char*)&q)[intPos] * HF_H1_DEVICE_KERNEL[j][i];
			}
		}
		((unsigned char*)&res)[byteIdx] = subres;
	}
	__syncthreads();
	((uint32_t*)output)[xIdx + yIdx * width] = res;

}

__global__ void getChannelsGPU(
	unsigned char* sourceImageData,
	int dimX, // px
	 int dimY, // px
	unsigned char* rs,
	unsigned char* gs,
	unsigned char* bs
) {

	 const int xIndex = blockIdx.x * BLOCK_DIM_X + threadIdx.x; // reference in px
	 const int yIndex = blockIdx.y * BLOCK_DIM_Y + threadIdx.y; // line # in px
	if (xIndex >= dimX * 3 || yIndex >= dimY)
	{
		return;
	}
	int colorOffset = 0;
	rs[xIndex + yIndex * dimX] = sourceImageData[xIndex * 3 + colorOffset + yIndex * dimX * 3];
	colorOffset++;
	gs[xIndex + yIndex * dimX] = sourceImageData[xIndex * 3 + colorOffset + yIndex * dimX * 3];
	colorOffset++;
	bs[xIndex + yIndex * dimX] = sourceImageData[xIndex * 3 + colorOffset + yIndex * dimX * 3];
}

__global__ void compileChannelsGPU(
	unsigned char* r,
	unsigned char* g,
	unsigned char* b,
	int dimX,
	int dimY,
	unsigned char* output
) {

	const int xIndex = blockIdx.x * BLOCK_DIM_X + threadIdx.x; // reference in px
	const int yIndex = blockIdx.y * BLOCK_DIM_Y + threadIdx.y; // line # in px
	if (xIndex >= dimX * 3 || yIndex >= dimY)
	{
		return;
	}

	int colorOffset;
	int colorOffset1;
	colorOffset1 = 0;
	colorOffset = colorOffset1;

	output[xIndex * 3 + colorOffset + yIndex * dimX * 3] = r[xIndex + yIndex * dimX];
	colorOffset++;
	output[xIndex * 3 + colorOffset + yIndex * dimX * 3] = g[xIndex + yIndex * dimX];
	colorOffset++;
	output[xIndex * 3 + colorOffset + yIndex * dimX * 3] = b[xIndex + yIndex * dimX];
	colorOffset++;
}