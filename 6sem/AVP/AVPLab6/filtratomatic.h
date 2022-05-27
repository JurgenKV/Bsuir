#ifndef FILTRATOMATIC_H
#define FILTRATOMATIC_H

#include "hff.cuh"

struct MeasuredResult
{
	float elapsedTime;
	unsigned char* ptr;
};

class Filtratomatic
{
	const char* sourcePath;
	const char* cpuPath;
	const char* gpuPath;

	unsigned char* sourceImageRawFreakingDataBecauseSomeoneJustHatesAllOfHumanity;
	unsigned char* sourceR;
	unsigned char* sourceG;
	unsigned char* sourceB;

	unsigned char* reference;
	unsigned char* gpuImage;


	unsigned int dimX = 0;
	unsigned int paddedX = 0;
	unsigned int padding = 0;
	unsigned int dimY = 0;
	unsigned int channels = 0;

	void generateReference();
	void generateGPU();

public:
	Filtratomatic(const char* source, const char* cpuPath, const char* gpuPath);

	bool testResults();

	void saveAllImages();

	void generateImagePair();
};

#endif
