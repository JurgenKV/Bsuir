#include "stdlib.h"
#include "signal.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <aio.h>
#include <unistd.h>

int has_initialized = 0;
void *managed_memory_start;
void *last_valid_address;

struct mem_control_block
{
	int is_available;
	int size;
};

void malloc_init()
{
	last_valid_address = sbrk(0);
	managed_memory_start = last_valid_address;
	has_initialized = 1;
}

void myFree(void *firstbyte)
{
	struct mem_control_block *mcb;
	mcb = firstbyte - sizeof(struct mem_control_block);
	mcb->is_available = 1;
	return;
}

void *myMalloc(long numbytes)
{
	void *current_location;

	struct mem_control_block *current_location_mcb;

	void *memory_location;

	if (!has_initialized)
	{
		malloc_init();
	}

	numbytes = numbytes + sizeof(struct mem_control_block);

	memory_location = 0;

	current_location = managed_memory_start;

	while (current_location != last_valid_address)
	{
		current_location_mcb = (struct mem_control_block *)current_location;

		if (current_location_mcb->is_available)
		{
			if (current_location_mcb->size >= numbytes)
			{
				current_location_mcb->is_available = 0;

				memory_location = current_location;

				break;
			}
		}
		current_location = current_location + current_location_mcb->size;
	}

	if (!memory_location)
	{
		sbrk(numbytes);

		memory_location = last_valid_address;

		last_valid_address = last_valid_address + numbytes;

		current_location_mcb = memory_location;
		current_location_mcb->is_available = 0;
		current_location_mcb->size = numbytes;
	}
	memory_location = memory_location + sizeof(struct mem_control_block);

	return memory_location;
}

void *myRealloc(void *ptr, size_t size)
{
	void *nptr = myMalloc(size);
	if (nptr = NULL)
	{
		return NULL;
	}
}

int main()
{
	int n;
	malloc_init();

	scanf("%d", &n);

	int *testPtr = (int *)myMalloc(5 * sizeof(int));
	int i, j;

	for (i = 0; i < n; i++)
		testPtr[i] = i;

	for (i = 0; i < n; i++)
		printf("num %d\n", testPtr[i]);

	printf("\nmyRealloc\n");
	scanf("%d", &n);
	myRealloc(testPtr, n);

	for (i = 0; i < n; i++)
		testPtr[i] = i;

	for (i = 0; i < n; i++)
		printf("num %d\n", testPtr[i]);

	printf("\nMyFree()\n");
	myFree(testPtr);

	return 0;
}