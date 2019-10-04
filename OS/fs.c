/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	fs.c
 * Synopsis	:	Implements the filesystem for tm4c_os
 * Date			:	September 11th, 2019
 *****************************************************************************/
#include <hw.h> /* For flash memory operations */
#include <fs.h>
#include <mem.h>
#include <cstring.h> /* For strncat and strncpy */
#include <tm4c123gh6pm.h>

struct superblock sb;

/*
 * Create an open directory. The directory should be closed after
 * it's done being read.
 * param name
 * 	The name of the directory
 * param cwd
 * 	The current working directory
 * Returns 0 on success, -1 on failure.
 */
struct dinode * create(char *name, struct dinode *cwd) {
	struct dinode dn;
/*TODO:
 * Need a strncat function. Then change path to append to cwd.
 */
	strncpy("/", dn.path, strlen("/"));
	strncpy(name, dn.name, strlen(name));
	dn.size = sizeof(struct dinode);
/* Find a block to save to. Free blocks are 1 and non-free are 0 */
	int i = 0;
	while(1) {
		if((sb.usebits >> i) & 0x1) {
			break;
		}
		i++;
	}
	dn.blocki = i;
	return NULL;
}

/*
 * Close an open directory.
 * param opendir
 * 	The directory to be closed.
 * 	Returns 0 on success, -1 on failure
 */
int closedir(struct dinode *opendir) {
	if(!write_flash(&sb.blockaddr[opendir->blocki], &opendir + 1)) {
		return 0;
	}
	return -1;
}

int init_fs() {
	sb.root = create("root", NULL);
	int i = 0;
/* Fill in the free block list with the addresses of all the blocks */
	sb.blockaddr[i] = (word *)(KFLASHPGS*FLASH_PAGE_SIZE);
 	for(i = 1; i < KFLASHPGS; i++) {
		sb.blockaddr[i] = (word *)((KFLASHPGS*FLASH_PAGE_SIZE) + i*BSIZE);
	}
/* All bits are 1 because all blocks are free */
	sb.usebits = 0xFFFFFFFFFFFFFFFF;
	return 0;
}
