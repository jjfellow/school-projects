#define BLOCK_SIZE 8192

#include <iostream>
#include <cstdlib>

class block
{
	public:
		void * address;
		block();
		void * getAddress();
		~block();
};