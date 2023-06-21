#include "filesystem.h"

unsigned char blocks[NUM_BLOCKS][BLOCK_SIZE];
//This is the directory map, it maps file numbers to an I-node as an array
uint8_t directory[NUM_INODES];
//This is the free I-Node map, that maps I-Node numbers to a boolean representing if it is free
bool freeInodes[NUM_INODES];
//The free blocks map, works the same way the free I-Node map does
bool freeBlocks[NUM_BLOCKS - NUM_INODES - 4];

//The working file, holds the file that the filesystem is reading/writing to
std::fstream * workingFile;

//Creating a new filesystem given the name
filesystem::filesystem(std::string newName)
{
	//Open a file that we can save to later
	workingFile->open(newName, std::ios::out | std::ios::binary);

	//Initialize every value of the free I-node map and free block map to true for free
	for(int i = 0; i < NUM_INODES; i++)
	{
		freeInodes[i] = true;
	}
	for(int i = 0; i < NUM_BLOCKS - BLOCK_OFFSET; i++)
	{
		freeBlocks[i] = true;
	}
}

//Second constructor, for loading a saved file system. Assumes the file has already been found and has been opened
//ASSUMPTION: MUST BE OPENED IN ios::out, ios::in, AND ios::binary MODES
filesystem::filesystem(std::fstream& inputFile)
{
	//set the working file as the supplied input file
	workingFile = &inputFile;
	//read the entire file into the blocks array
	//seek the get pointer to the beginning of the file
	workingFile->seekg(0);
	//Read the first two blocks, and store the result into the directory map
	workingFile->read(directory, BLOCK_SIZE * 2);
	//Read the next block, and store the result in the free Inodes map
	workingFile->read(freeInodes, BLOCK_SIZE);
	//Read the next block, and store the result in the free Blocks map
	workingFile->read(freeBlocks, BLOCK_SIZE);
	//Read the rest of the blocks and fill in the data and i node blocks
	workingFile->read(blocks, (NUM_BLOCKS - 4) * BLOCK_SIZE);
}

filesystem::~filesystem()
{
	if(workingFile.isOpen())
	{
		workingFile.close();
	}
}
void filesystem::put(std::ifstream inputFile);
std::ofstream filesystem::get(std::string name);
std::ofstream filesystem::get(std::string name, std::string newName);
void filesystem::del(std::string name);
void filesystem::undel(std::string name);
std::string filesystem::list();
int filesystem::df();
//Open() will be implemented by the shell class, it just verifies the file exists and uses the
//second constructor
void filesystem::close();

//Creates a new file system in the current working directory
//Will be handled by the shell

void filesystem::savefs()
{
	//Need to save the state of the directory and maps to the first 4 blocks
	workingFile.seekp(0);
	workingFile.write(directory, BLOCK_SIZE * 2);
	workingFile.write(freeInodes, BLOCK_SIZE);
	workingFile.write(freeBlocks, BLOCK_SIZE);

	workingFile.write(blocks, (NUM_BLOCKS - 4) * BLOCK_SIZE);
}