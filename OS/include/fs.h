/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	fs.h
 * Synopsis	:	Implements the filesystem for tm4c_os
 * Date			:	September 11th, 2019
 *****************************************************************************/
#include <types.h>

#define PATHSIZE 256u
#define NAMESIZE 16u
/* Size of the file system in flash in bytes*/
#define FSSIZE 32768u
/* Block size in bytes */
#define BSIZE 512u

//struct dinode;
//struct inode;

/* File index node. */
struct inode {
	char name[NAMESIZE];
	word *location;
};

/* Maximum files that can be stored in a directory. Every block will have a */
/* dinode, so the max number of inodes is however many can fit in what */
/* remains. */
#define MAXFILES ((BSIZE / sizeof(struct inode)))

/* Directory index node. */
struct dinode {
	char path[PATHSIZE];
	char name[NAMESIZE];
	struct inode *files[MAXFILES];
	word size;
};

/* Superblock contains general info about the entire file system. */
struct superblock {
	word *freeblocks[FSSIZE/BSIZE];
	struct dinode *root;
};

struct dinode * create(char *, struct dinode *);
