//COSC 361 Spring 2017
//FUSE Project block template
//Stephen Marz (2017)
#pragma once
#include <stdint.h>
#include <errno.h>

#define DEBUG

//Structure of block:
//BLOCK_HEADER
//NODE
//BLOCK LIST (if NODE->size > 0, there are NODE->size / BLOCK_SIZE + 1 blocks, otherwise there are none (directories))
//NODE
//BLOCK LIST
//...
//BLOCKS (each 1024 bytes)

//The hard drive file will be hardcoded into your program
//using the string HARD_DRIVE
const char *HARD_DRIVE = "hard_drive";
//The size of the name for each file/directory in the filesystem.
//since every name contains the parent directories, it is fairly large.
const int NAME_SIZE = 512;
//The maximum size of your drive. If anything tries to go above this,
//return -ENOSPC (No space available). This is around 2MB.
const int MAX_DRIVE_SIZE = (1 << 21);
//The first 8 bytes of the hard drive file. If the magic doesn't match
//this, then consider it an incorrect file
const char MAGIC[] = {'C', 'O', 'S', 'C', '_', '3', '6', '1'};

//The first structure in the file. This presents the magic, the number
//of nodes, and the number of data blocks.
typedef struct {
	char		magic[8]; // COSC_361
	char		unused[16];
	uint64_t	block_size; //size of a block
	uint64_t	nodes; // # of nodes
	uint64_t	blocks; // # of blocks
} BLOCK_HEADER, *PBLOCK_HEADER;


//This structure describes a node. When declaring memory space, declare
//space for the entire structure.
typedef struct {
	char 		name[NAME_SIZE]; // \0 terminated

	uint64_t	id; // Unique ID for this node
	uint64_t	link_id; // If this is a S_IFLNK, this points to the id it is linked to

	uint64_t	mode; //S_IFDIR, S_IFREG, S_IFLNK, OR'ed w/ permissions (S_IFDIR = directory, S_IFREG = regular file, S_IFLNK = symb. link)

	uint64_t	ctime; // Creation time filled with time(NULL)
	uint64_t	atime; // Accessed time filled with time(NULL)
	uint64_t	mtime; // Modified time filled with time(NULL)

	uint32_t	uid; // User ID
	uint32_t	gid; // Group ID

	uint64_t	size; // Size (size / block_header.block_size + 1 = num blocks), if this is 0, this is S_IFDIR or S_IFLNK

 	uint64_t	*blocks; // Offsets to the blocks starting with 1
} NODE, *PNODE;

//This is here because on the actual disk, the uint64_t *blocks is not there.
//instead, that is there to help the structure. Use ONDISK_NODE_SIZE when
//reading from or writing to the hard_disk.
const unsigned int ONDISK_NODE_SIZE = sizeof(NODE) - sizeof(uint64_t*);

//A structure that describes a block of data, which is the size of
//block_header.block_size
typedef struct {
	char	*data;
} BLOCK, *PBLOCK;
