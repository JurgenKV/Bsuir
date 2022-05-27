#include "filtratomatic.h"

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <iomanip>

#include "helper_image.h"
#include "kernels.cuh"

void 
initSourceImage (
	unsigned char* sourceImageData,
	unsigned char* r,
	unsigned char* g,
	unsigned char* b,
	int dimX, 
	int dimY
) {
	int dataIdx = 0;
	int pxIdx = 0;
	while (pxIdx < dimX * dimY)
	{
		r[pxIdx] = sourceImageData[dataIdx++];

		g[pxIdx] = sourceImageData[dataIdx++];

		b[pxIdx] = sourceImageData[dataIdx++];
		pxIdx++;

	}
}

unsigned char* 
compileChannels (
	unsigned char* r, 
	unsigned char* g, 
	unsigned char* b, 
	int dimX, 
	int dimY
) {
	auto* image = new unsigned char[dimX * dimY * 3];

	int dataIdx = 0;
	int pxIdx = 0;
	while (pxIdx < dimX * dimY)
	{
		image[dataIdx++] = r[pxIdx];
		image[dataIdx++] = g[pxIdx];
		image[dataIdx++] = b[pxIdx];
		pxIdx++;
	}
	return image;
}

Filtratomatic::Filtratomatic (
	const char* source,
	const char* cpuPath,
	const char* gpuPath
) : sourcePath(source), cpuPath(cpuPath), gpuPath(gpuPath)
{
	 dimX = 0;
	 dimY = 0;
	channels = 0;
	 unsigned char* sourceImageData = NULL;
	__loadPPM(sourcePath, &sourceImageData, &dimX, &dimY, &channels);
	//std::cout << channels << std::endl;
	padding = 4 - (dimX % 4);
	paddedX = dimX + padding;
	auto* paddedSourceImageData = new unsigned char[paddedX * dimY * 3];

	for (int i = 0; i < dimY; i++)
	{
		for (int j = 0; j < dimX; j++)
		{
			for (int colorIdx = 0; colorIdx < channels; colorIdx++)
			{
				paddedSourceImageData[(j + i * paddedX) * channels + colorIdx] = 
					sourceImageData[(j + i * dimX) * channels + colorIdx];
			}
		}
	}
	dimX = paddedX;

	sourceR = new unsigned char[dimX * dimY];
	sourceG = new unsigned char[dimX * dimY];
	sourceB = new unsigned char[dimX * dimY];
	/*int k = 0;
	while (k < paddedX * dimY * 3 ) {
		std::cout << paddedSourceImageData[k];
		k++;
	}*/

	initSourceImage(paddedSourceImageData, sourceR, sourceG, sourceB, dimX, dimY);


	sourceImageRawFreakingDataBecauseSomeoneJustHatesAllOfHumanity = paddedSourceImageData;
}

bool 
Filtratomatic::testResults()
{
	for (int i = 0; i < dimY * dimX * 3; i++)
	{
		if (abs(reference[i] - gpuImage[i]) > 1) {
			std::cerr << "[ERR] gpu result does not match reference, delta " << reference[i] - gpuImage[i] << std::endl;
			return false;
		}
	}
	std::cerr << "[AOK] gpu result matches reference" << std::endl;
	return true;
}


MeasuredResult*
generateReferenceChannel (
	int dimX, 
	int dimY, 
	unsigned char* source
) {
	std::chrono::high_resolution_clock::time_point tpStart;
	std::chrono::high_resolution_clock::time_point tpEnd;
	auto* result = new MeasuredResult;
	auto* reference = new unsigned char[dimX * dimY];
	int refPosition = 0;

	if (reference == nullptr)
	{
		throw std::runtime_error("[ERR] unable to allocate reference memory");
	}

	tpStart = std::chrono::high_resolution_clock::now();
	for (int y = 0; y < dimY; y++)
	{
		for (int x = 0; x < dimX; x++)
		{
			unsigned char result = 0;
			for (int j = 0; j < 3; j++)
			{
				for (int i = 0; i < 3; i++)
				{
					int adjustedX = x + i - 1;
					if (adjustedX < 0)
					{
						adjustedX = 0;
					}
					if (adjustedX == dimX)
					{
						adjustedX = dimX - 1;
					}

					int adjustedY = y + j - 1;
					if (adjustedY < 0)
					{
						adjustedY = 0;
					}
					if (adjustedY == dimY)
					{
						adjustedY = dimY - 1;
					}

					refPosition = dimX * adjustedY + adjustedX;
					result += source[refPosition] * HF_H1_KERNEL[j][i];
				}
			}
			reference[y * dimX + x] = result;
		}
	}
	tpEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = tpEnd - tpStart;

	result->ptr = reference;
	result->elapsedTime = diff.count();

	return result;
}

void 
Filtratomatic::generateReference()
{
	auto* rResult = generateReferenceChannel(dimX, dimY, sourceR);
	auto* gResult = generateReferenceChannel(dimX, dimY, sourceG);
	auto* bResult = generateReferenceChannel(dimX, dimY, sourceB);

	reference = compileChannels(
		rResult->ptr,
		gResult->ptr,
		bResult->ptr,
		dimX, dimY
	);

	float combinedTime = rResult->elapsedTime + gResult->elapsedTime + bResult->elapsedTime;
	std::cout
		<< "[INF] reference generated in "
		<< combinedTime
		<< "s " << std::endl;
}

MeasuredResult* 
generateGPUChannels (
	int dimX, 
	int dimY, 
	unsigned char* rawSourceImageData
) {
	auto* result = new MeasuredResult;
	 cudaStream_t streamR, streamG, streamB;
	 cudaStreamCreate(&streamR);
	 cudaStreamCreate(&streamG);
	 cudaStreamCreate(&streamB);

	long long int imgSz = dimX * dimY;
	cudaEvent_t ceStart, ceEnd;
	unsigned char* gpuResultR, *gpuResultG, *gpuResultB;
	unsigned char* gpuSourceR, *gpuSourceG, *gpuSourceB, *gpuSourceImage;

	cudaMalloc((void**)&gpuResultR, imgSz);
	cudaMalloc((void**)&gpuResultG, imgSz);
	cudaMalloc((void**)&gpuResultB, imgSz);
	cudaMalloc((void**)&gpuSourceR, imgSz);
	cudaMalloc((void**)&gpuSourceG, imgSz);
	cudaMalloc((void**)&gpuSourceB, imgSz);
	cudaMalloc((void**)&gpuSourceImage, imgSz*3);

	cudaMemcpy(gpuSourceImage, rawSourceImageData, imgSz * 3, cudaMemcpyHostToDevice);

	dim3 dimGridColorSep(
		(int)ceil((double)dimX / (double)BLOCK_DIM_X),
		(int)ceil((double)dimY / (double)BLOCK_DIM_Y)
	);
	dim3 dimBlockColorSep(BLOCK_DIM_X, BLOCK_DIM_Y);

	int width = dimX / 4;
	dim3 dimGrid(
		(int)ceil((double)width / (double)BLOCK_DIM_X),
		(int)ceil((double)dimY / (double)BLOCK_DIM_Y)
	);
	dim3 dimBlock(BLOCK_DIM_X, BLOCK_DIM_Y);

	dim3 dimGridColorJoin(
		(int)ceil((double)dimX / (double)BLOCK_DIM_X),
		(int)ceil((double)dimY / (double)BLOCK_DIM_Y)
	);
	dim3 dimBlockColorJoin(BLOCK_DIM_X, BLOCK_DIM_Y);

	auto* gpuImageResult = new unsigned char[imgSz * 3];
	cudaMalloc((void**)&gpuImageResult, imgSz * 3);

	cudaEventCreate(&ceStart);
	cudaEventCreate(&ceEnd);
	cudaEventRecord(ceStart, nullptr);

	getChannelsGPU <<< dimGridColorSep, dimBlockColorSep >>> (gpuSourceImage, dimX, dimY, gpuSourceR, gpuSourceG, gpuSourceB);
	kernel <<< dimGrid, dimBlock, 0, streamR >>> (gpuSourceR, gpuResultR, width, dimY, dimX);
	kernel <<< dimGrid, dimBlock, 0, streamG >>> (gpuSourceG, gpuResultG, width, dimY, dimX);
	kernel <<< dimGrid, dimBlock, 0, streamB >>> (gpuSourceB, gpuResultB, width, dimY, dimX);	
	compileChannelsGPU <<< dimGridColorJoin, dimBlockColorJoin >>> (gpuResultR, gpuResultG, gpuResultB, dimX, dimY, gpuImageResult);
	
	cudaEventRecord(ceEnd, nullptr);
	cudaEventSynchronize(ceEnd);

	float duration;
	duration = 0;
	cudaEventElapsedTime(&duration, ceStart, ceEnd);

	unsigned char* res = new unsigned char[imgSz * 3];
	cudaMemcpy(res, gpuImageResult, imgSz * 3, cudaMemcpyDeviceToHost);

	result->elapsedTime = duration;
	result->ptr = res;

	cudaFree(gpuResultR);
	cudaFree(gpuResultG);
	cudaFree(gpuResultB);

	cudaFree(gpuSourceR);
	cudaFree(gpuSourceG);
	cudaFree(gpuSourceB);

	cudaDeviceReset();

	return result;
}


void 
Filtratomatic::generateGPU () 
{
	auto* result = generateGPUChannels(dimX, dimY, sourceImageRawFreakingDataBecauseSomeoneJustHatesAllOfHumanity);
	gpuImage = result->ptr;

	std::cout
		<< "[INF] gpu image generated in "
		<< result->elapsedTime / 1000
		<< "s " << std::endl;
}

void 
Filtratomatic::saveAllImages()
{
	dimX -= padding;
	auto* unpaddedRef = new unsigned char[dimX * dimY * 3];
	auto* unpaddedGPU = new unsigned char[dimX * dimY * 3];

	for (int i = 0; i < dimY; i++)
	{
		for (int j = 0; j < dimX; j++)
		{
			for (int colorIdx = 0; colorIdx < channels; colorIdx++)
			{
				unpaddedRef[(j + i * dimX) * channels + colorIdx] =
					reference[(j + i * paddedX) * channels + colorIdx];
				unpaddedGPU[(j + i * dimX) * channels + colorIdx] =
					gpuImage[(j + i * paddedX) * channels + colorIdx];
			}
		}
	}

	  __savePPM(cpuPath, unpaddedRef, dimX, dimY, channels);
	  __savePPM(gpuPath, unpaddedGPU, dimX, dimY, channels);
}


void 
Filtratomatic::generateImagePair () 
{
	std::chrono::high_resolution_clock::time_point tpStart;
	std::chrono::high_resolution_clock::time_point tpEnd;
	tpStart = std::chrono::high_resolution_clock::now();
	generateReference();
	generateGPU();
	tpEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = tpEnd - tpStart;
	std::cout
		<< "[INF] total time "
		<< diff.count()
		<< "s " << std::endl;
}

