#define NUM_BLOCKS 4226
#define BLOCK_SIZE 8192
#define NUM_INODES 125
//Defines how many blocks to offset the start of the INodes
#define INODE_OFFSET 4
//Defines how many blocks to offset the start of the data blocks
#define BLOCK_OFFSET 129

#include <iostream>
#include <map>
#include <cstdint>
#include <fstream>

enum attributes : uint8_t
{
	//These first three attributes are file permissions
	READ  = 0b00000001,
	WRITE = 0b00000010,
	EXEC  = 0b00000100,
	//hidden file flag
	HIDDEN= 0b00001000,
	//Last attribute is whether this node is free. A value of 1 denotes an allocated I-Node
	//a value of 0 denotes a free I-Node that can be allocated to
	ALLOC = 0b10000000,
};

struct INODE
{
	char name[32];
	uint8_t fileNum;
	enum attributes attributes;
	uint32_t size;
	uint16_t lastBlockOffset;
	uint16_t blocks[125];

};



class filesystem
{
	public:
		filesystem(std::string);
		filesystem( std::fstream&);
		void put(std::ifstream);
		std::ofstream get(std::string);
		std::ofstream get(std::string, std::string);
		void del(std::string);
		void undel(std::string);
		std::string list();
		int df();
		void close();
		void createfs(std::string);
		void savefs();
	private:
		unsigned char blocks[NUM_BLOCKS][BLOCK_SIZE];
		std::fstream * workingFile;
		bool freeInodes[NUM_INODES];
		bool freeBlocks[NUM_BLOCKS - NUM_INODES - 4];
};