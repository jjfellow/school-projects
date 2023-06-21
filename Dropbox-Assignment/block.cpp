#include "block.h"

//Blocks have an next address pointer, all other more complicated things are handled in filesystem
void * address;

//Constructor. When called, it allocates BLOCK_SIZE bytes of memory and stores the pointer to that in address
block::block()
{
	address = malloc(BLOCK_SIZE);
}

void * block::getAddress()
{
	return address;
}

block::~block()
{
	free(address);
}