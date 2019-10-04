/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	fs.h
 * Synopsis	:	Implements the filesystem for tm4c_os
 * Date			:	September 11th, 2019
 *****************************************************************************/
#include <types.h>

#define PATHSIZE 256u
#define NAMESIZE 16u
/* Size of the file system in flash in bytes */
#define FSSIZE 32768u
/* Block size in bytes */
#define BSIZE 512u


/* File index node. */
struct inode {
	int used; /* Flags to signal that the file is being used. */
	char name[NAMESIZE];
	word *address;
	word *parent;
};

/* Maximum files that can be stored in a directory. Every block will have a */
/* dinode, so the max number of inodes is however many can fit in what */
/* remains. */
#define MAXFILES ((BSIZE / sizeof(struct inode)))
/*TODO:
 * Howwwww do I manage all this memory. I'm trying to picture it on draw.io
 */
/* Directory index node. */
struct dinode {
	word size;
	int blocki; /* Block address index */
	char path[PATHSIZE];
	char name[NAMESIZE];
	struct inode files[MAXFILES];
};

/* Superblock contains general info about the entire file system. */
struct superblock {
	word *blockaddr[FSSIZE/BSIZE];
	unsigned long long int usebits; /* Each bit represents a block */
	struct dinode *root;
};

/* Function prototypes */
struct dinode * create(char *, struct dinode *);
int init_fs(void);
int closedir(struct dinode *);

/* Macro functions */
#define opendir(addr) (struct dinode *)(addr)
