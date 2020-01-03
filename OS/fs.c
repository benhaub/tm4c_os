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
 * Returns dn.blocki will contain a number between from 0 to 63 on success,
 * -1 on failure.
 */
struct dinode create(char *name, struct dinode *cwd) {
	struct dinode dn;
	if(cwd != NULL) {
		//strncat(name, "/", 1);
		//strncat(cwd->name, name, strlen(name));
	}
	else {
/* Directory is in root */
		strncpy("/", dn.path, strlen("/"));
		strncpy(name, dn.name, strlen(name));
		dn.size = sizeof(struct dinode);
/* Find a block to save to. Free blocks are 1 and non-free are 0 */
		int i = 0;
		while(i < NUMBLOCKS) {
			if((sb.usebits >> i) & 0x1) {
/* Check if there's room in the block for a new directory */
				if((sb.blockmem[i] - (int)sizeof(struct dinode)) < 0) {
					(sb.usebits) &= ~(1 << i);
					i++;
				}
				else {
					break;
				}
			}
			else {
				i++;
			}
		}
/* There wasn't enough space in the file system for the directory */
		if(i >= NUMBLOCKS) {
			dn.blocki = -1;
			return dn;
		}
		sb.blockmem[i] -= sizeof(struct dinode);
		dn.blocki = i;
	}
	return dn;
}

/*
 * Close an open directory.
 * param opendir
 * 	The directory to be closed.
 * 	Returns 0 on success, -1 on failure
 */
int closedir(struct dinode opendir) {
	if(!write_flash(&opendir, &opendir + 1, sb.blockaddr[opendir.blocki])) {
		return 0;
	}
	return -1;
}

int init_fs() {
/* All bits are 1 because all blocks are free */
	sb.usebits = 0xFFFFFFFFFFFFFFFF;
	int i = 0;
/* Fill in the free block list with the addresses of all the blocks */
/* Also fill in block mem with the size of each block */
	sb.blockaddr[i] = (word *)(KFLASHPGS*FLASH_PAGE_SIZE);
	sb.blockmem[i] = BSIZE;
 	for(i = 1; i < KFLASHPGS; i++) {
		sb.blockaddr[i] = (word *)((KFLASHPGS*FLASH_PAGE_SIZE) + i*BSIZE);
		sb.blockmem[i] = BSIZE;
	}
	sb.root = create("root", NULL);
	if(sb.root.blocki < 0) {
		return -1;
	}
	return 0;
}
