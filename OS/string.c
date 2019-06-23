/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	string.h
 * Synopsis	:	String manipulation functions
 * Date			:	June 18th, 2019
 *****************************************************************************/
#include <mem.h>
/*
 * Copy the string from src into dst.
 * param src:
 * 	The source string to be copied
 * param dst:
 * 	The destination string to be copied to
 * param len:
 * 	The maximum number of bytes to copy
 * returns -1 on failure, 0 on success
 */
int strncpy(char *src, char *dst, unsigned int len) {
	int i = 0;
	if(NULL == src || NULL == dst) {
		return -1;
	}
	while(i < len) {
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return 0;
}

/*
 * Get the length of string.
 * param string:
 * 	The string you want the length of
 * returns the number of bytes in the string or -1 on failure
 */
unsigned int strlen(char *string) {
	int i;
	if(NULL == string) {
		return -1;
	}
	else {
		while(NULL != string) {
			string = string + 1;
			i++;
		}
	}
	return i;
}

