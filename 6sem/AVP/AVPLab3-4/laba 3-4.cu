#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>

using namespace std;


__device__ unsigned int makeInt(const unsigned short a, const unsigned short b) {
	return (a << 16) | (b & 0xFFFF);
}

template<unsigned index>
__device__ unsigned short extract(const unsigned int a) {
	if (index == 0) {
		return (unsigned short)(a >> 16);
	}
	else {
		return (unsigned short)(a & 0xffff);
	}
}

void print(char* m, int height, int width) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			cout << (unsigned int)m[i * width + j] << " ";
		}
		cout << "\n";
	}
}

__global__ void flatMatrixKernel(const char* src, char* dst, int height, int width) {

	const uint2 offset = make_uint2(blockIdx.x * blockDim.x + threadIdx.x, blockIdx.y * blockDim.y + threadIdx.y);

	if ((offset.x / 2) % 2 == 0) {
		if (offset.x == 0 || offset.x == 1) {

			dst[offset.y * width + offset.x] = src[offset.y * width + offset.x];
		}
		else {
			dst[offset.y * width + offset.x - (offset.x / 2)] = src[offset.y * width + offset.x];
		}
	}
	else {
		dst[(offset.y * 2 + 1) * width / 2 + offset.x - (offset.x / 2 + 1)] = src[offset.y * width + offset.x];
	}
}


__global__ void flatMatrixKernelShared(uint32_t* src, uint32_t* dst, int src_pitch, int dst_pitch, int height, int width) {

	__shared__ uint32_t shared[32 * 32];

	const uint2 offset = make_uint2(blockIdx.x * blockDim.x + threadIdx.x, blockIdx.y * blockDim.y + threadIdx.y);

	int src_width = src_pitch / 4;
	int dst_width = dst_pitch / 4;

	if (offset.x < width && offset.y < height) {
		shared[threadIdx.y * blockDim.x + threadIdx.x] = src[offset.y * src_width + offset.x];
	}
	__syncthreads();

	unsigned short firstPart = extract<1>(shared[threadIdx.y * blockDim.x + threadIdx.x]);
	unsigned short secondPart = extract<0>(shared[threadIdx.y * blockDim.x + threadIdx.x]);

	if (offset.x < width && offset.y < height) {
		if (offset.x % 2 == 0) {
			unsigned short firstPartResult = extract<0>(
				dst[offset.y * 2 * dst_width + (offset.x / 2)]);
			unsigned short firstPartResultB = extract<0>(
				dst[(offset.y * 2 + 1) * dst_width + (offset.x / 2)]);
			dst[offset.y * 2 * dst_width + (offset.x / 2)] = makeInt(firstPartResult, firstPart);
			dst[(offset.y * 2 + 1) * dst_width + (offset.x / 2)] = makeInt(firstPartResultB, secondPart);
		}
		else {
			unsigned short firstPartResult = extract<1>(
				dst[offset.y * 2 * dst_width + (offset.x / 2)]);
			unsigned short firstPartResultB = extract<1>(
				dst[(offset.y * 2 + 1) * dst_width + (offset.x / 2)]);
			dst[offset.y * 2 * dst_width + (offset.x / 2)] = makeInt(firstPart, firstPart);
			dst[(offset.y * 2 + 1) * dst_width + (offset.x / 2)] = makeInt(secondPart, firstPartResultB);
		}
	}
}

void initialize(char* src, int height, int width) {
	for (int i = 0; i < height * width; i++) {
		src[i] = i;
	}
}

bool compare(char* firstMatrix, char* secondMatrix, int size) {
	for (int i = 0; i < size; i++) {
		if (firstMatrix[i] != secondMatrix[i]) {
			return false;
		}
	}
	return true;
}

void flatMatrix(char* src, char* dst, int width, int height) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if ((j / 2) % 2 == 0) {
				if (j == 0 || j == 1) {
					dst[i * width + j] = src[i * width + j];
				}
				else {
					dst[i * width + j - (j / 2)] = src[i * width + j];
				}
			}
			else {
				dst[(i * 2 + 1) * width / 2 + j - (j / 2 + 1)] = src[i * width + j];
			}
		}
	}
}

float flatMatrixBasic(char* src, char* dst, int height, int width) {
	
	const int size = height * width;
	char* d_src = nullptr;
	char* d_dst = nullptr;
	cudaStream_t stream = 0;
	cudaEvent_t eventStart, eventStop;
	cudaEventCreate(&eventStart);
	cudaEventCreate(&eventStop);

	if (cudaMalloc((void**)&d_src, size * sizeof(char)) != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixBasic() 1");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	if (cudaMalloc((void**)&d_dst, size * sizeof(char)) != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixBasic() 2");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	if (cudaMemcpy(d_src, src, size * sizeof(char), cudaMemcpyHostToDevice) != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixBasic() 3");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	dim3 blockDimension(32, 32);
	dim3 gridDimension(width / blockDimension.x, height / blockDimension.y);

	cudaEventRecord(eventStart, stream);
	flatMatrixKernel << < gridDimension, blockDimension >> > (d_src, d_dst, height, width);

	if (cudaDeviceSynchronize() != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixBasic() 4");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	if (cudaMemcpy(dst, d_dst, size * sizeof(char), cudaMemcpyDeviceToHost) != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixBasic() 5");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	cudaEventRecord(eventStop, stream);
	cudaEventSynchronize(eventStop);
	float occupiedTime;
	cudaEventElapsedTime(&occupiedTime, eventStart, eventStop);

	cudaFree(d_src);
	cudaFree(d_dst);

	return occupiedTime;
}

float flatMatrixShared(char* src, char* dst, int height, int width) {

	const int size = height * width;
	uint32_t* d_src = nullptr;
	uint32_t* d_dst = nullptr;
	cudaStream_t stream = 0;
	size_t src_pitch = 0;
	size_t dst_pitch = 0;
	cudaEvent_t eventStart, eventStop;
	cudaEventCreate(&eventStart);
	cudaEventCreate(&eventStop);

	if (cudaMallocPitch((void**)&d_src, &src_pitch, width * sizeof(char), height) != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixShared() 1");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	if (cudaMallocPitch((void**)&d_dst, &dst_pitch, width * sizeof(char) / 2, height * 2) != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixShared() 2");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	if (cudaMemcpy2D(d_src, src_pitch, src, width * sizeof(char), width * sizeof(char), height,
		cudaMemcpyHostToDevice) != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixShared() 3");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	dim3 blockDimension(32, 32);
	dim3 gridDimension((width + blockDimension.x - 1) / blockDimension.x,
		(height + blockDimension.y - 1) / blockDimension.y);
	cudaEventRecord(eventStart, stream);
	flatMatrixKernelShared << < gridDimension, blockDimension >> > (d_src, d_dst, src_pitch, dst_pitch, height, width / 4);

	if (cudaDeviceSynchronize() != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixShared() 4");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	if (cudaMemcpy2D(dst, width * sizeof(char) / 2, d_dst, dst_pitch, width * sizeof(char) / 2, height * 2,
		cudaMemcpyDeviceToHost) != cudaSuccess) {
		fprintf(stderr, "Error flatMatrixShared() 5");
		cudaFree(d_src);
		cudaFree(d_dst);
		return -1;
	}

	cudaEventRecord(eventStop, stream);
	cudaEventSynchronize(eventStop);

	float occupiedTime;
	cudaEventElapsedTime(&occupiedTime, eventStart, eventStop);

	cudaFree(d_src);
	cudaFree(d_dst);

	return occupiedTime;
}

int main() {

	int n = 7;
	int m = 3;
	int n_new = n;
	if (n % 4 != 0) {
		for (n_new = n;; n_new++) {
			if (n_new % 4 == 0) {
				break;
			}
		}
	}
	cout << n_new - n;
	char* h_src = (char*)malloc(n_new * m * sizeof(char));
	char* d_dst = (char*)malloc(n_new * m * sizeof(char));
	char* d_dst_for_basic = (char*)malloc(n_new * m * sizeof(char));
	char* d_dst_for_not_full = (char*)malloc(n_new * m * sizeof(char));
	char* d_dst_for_shared = (char*)malloc(n_new * m * sizeof(char));

	initialize(h_src, m, n_new);

	int od = (n_new - n) * m;
	cout << od;
	for (int i = 0; i < od; i++) {
		h_src[m * n_new - i - 1] = 0;
	}
	auto t0 = std::chrono::high_resolution_clock::now();
	flatMatrix(h_src, d_dst, n, m);
	auto t1 = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> time = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);

	print(h_src, m, n_new);

	cout << "=======================================" << endl;

	cout << "CPU time -  " << time.count() * 100<< endl;
	cout << "Basic GPU time - " << flatMatrixBasic(h_src, d_dst_for_basic, m, n_new) / 100 << endl;
	cout << "Shared memory GPU -  " << flatMatrixShared(h_src, d_dst_for_shared, m, n_new) / 100 << endl;
	
	cout << "=======================================" << endl;

		if (compare(d_dst, d_dst_for_shared, n_new * m)) {

			cout << "Done correctly\n\n\n" << endl;

			
			
		}else {
		cout << "\n\n\n" << endl;
		}

	print(d_dst_for_shared, m * 2, n_new / 2);
	cout << "=======================================" << endl;
	
	free(h_src);
	free(d_dst);
	free(d_dst_for_basic);
	free(d_dst_for_shared);

	return 0;
}