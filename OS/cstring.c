/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	string.h
 * Synopsis	:	String manipulation functions
 * Date			:	June 18th, 2019
 *****************************************************************************/
#include <mem.h>
#include <types.h>
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
	int i = 0;
	while(0 != *(string + i)) {
		i++;
	}
return i;
}

/*
 * concatenate dest with with len bytes of str. dest will be null terminated
 * when the function returns. dest must be null terminated when the function
 * is called.
 * param dest
 *   The destination string
 * param str
 *   The string to be appended onto dest
 * param n
 *   The number of bytes from str to append.
 * returns a pointer to dest.
 */
//char *strncat(char *dest, char *str, unsigned int len) {
//  int i = 0;
//	int j = 0;
//	while(*dest != 0) {
//    i++;
//	}
//  i++;
//	while(j < len) {
//    dest[i] = str[j];
//		j++;
//	}
//	return dest;
//}

/*
 * Copy n bytes from memory area src to memory area dst.
 * return NULL on failure, pointer to dest on success.
 */
void *memcpy(void *dest, const void *src, unsigned int n) {
	int i = 0;
	while(i < n) {
		*((char *)dest + i) = *((char *)src + i);
			i++;
	}
	return dest;
}

/*
 * Fills the first n bytes of dest with the constant value of src.
 * UNTESTED
 */
void *memset(void *dest, const int src, unsigned int n) {
	int i = 0;
	while(i < n) {
		*((char *)dest + 1) = (char)src;
		i++;
	}
	return dest;
}	
