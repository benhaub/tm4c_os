/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	fs.c
 * Synopsis	:	Implements the filesystem for tm4c_os
 * Date			:	September 11th, 2019
 *****************************************************************************/
#include <hw.h>
#include <fs.h>
#include <cstring.h>

/*
 * Create a directory.
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
	return NULL;
}

int init_fs() {
	struct superblock sb;
	sb.root = create("root", NULL);
	write_flash(&sb, &sb + sizeof(struct superblock));
	return 0;
}
