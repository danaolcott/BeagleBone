/*
10/4/17
Memory.c
Function defintions for init, access, etc
memory buffers, framebuffers, etc

*/

#include <stdio.h>
#include <string.h>
#include "memory.h"

static unsigned char memory_writeBuffer1[MEMORY_WRITE_BUFFER_SIZE];
static unsigned char memory_writeBuffer2[MEMORY_WRITE_BUFFER_SIZE];
static unsigned char memory_activeBuffer = 1;

void memory_init()
{
	memset(memory_writeBuffer1, 0x00, MEMORY_WRITE_BUFFER_SIZE);
	memset(memory_writeBuffer2, 0x00, MEMORY_WRITE_BUFFER_SIZE);
}


//////////////////////////////////////////
//Memory_getActiveBuffer
//returns a pointer to available write buffer
//Continuously flipflops, but this could be extened
//to using more than one buffer at a time.
//
unsigned char* memory_getActiveBuffer(void)
{
	//currently 1, make it 2 and pass ptr to 2
	if (memory_activeBuffer == 1)
	{
		printf("memory buffer - 2\n");
		memory_activeBuffer = 2;
		memset(memory_writeBuffer2, 0x00, MEMORY_WRITE_BUFFER_SIZE);
		return memory_writeBuffer2;
	}

	else if (memory_activeBuffer == 2)
	{
		printf("memory buffer - 1\n");
		memory_activeBuffer = 1;
		memset(memory_writeBuffer1, 0x00, MEMORY_WRITE_BUFFER_SIZE);
		return memory_writeBuffer1;
	}
	else
	{
		//error- clear both and reset to 1
		memory_activeBuffer = 1;
		memset(memory_writeBuffer1, 0x00, MEMORY_WRITE_BUFFER_SIZE);
		memset(memory_writeBuffer2, 0x00, MEMORY_WRITE_BUFFER_SIZE);
		return memory_writeBuffer1;
	}

	//should never make it here
	return memory_writeBuffer1;
}

