#include "filtratomatic.h"
#include "hff.cuh"

int main()
{
	const char srcImage[] = "xsb1.ppm";
	const char imageCPU[] = "xsb-cpu.ppm";
	const char imageGPU[] = "xsb-gpu.ppm";

	auto fil = Filtratomatic(
		srcImage, imageCPU, imageGPU
	);

	fil.generateImagePair();
	fil.saveAllImages();
	if (fil.testResults())
	{
		fil.saveAllImages();
	}

}