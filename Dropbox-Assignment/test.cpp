#include <unordered_map>

int main(void)
{
	unsigned char blocks[10][256];
	std::unordered_map<int, int> map;
	&map = (void *) &blocks[2][0];

	return 0;

}