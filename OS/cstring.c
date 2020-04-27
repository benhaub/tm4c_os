/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : cstring.h                                                       *
 * Synopsis : String manipulation functions                                   *
 * Date     : June 18th, 2019                                                 *
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
int strncpy(char *dst, char *src, unsigned int len) {
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
char *strncat(char *dest, char *str, unsigned int len) {
  int i = 0;
	int j = 0;
	while(dest[i] != 0) {
    i++;
	}
	while(j < len) {
    dest[i] = str[j];
		j++;
    i++;
	}
	return dest;
}

/*
 * compares up to len bytes of str1 to str2 by returning the difference of the
 * ASCII value of the two strings. A value return that is less than zero or
 * greater than zero means the strings are not equal, a value returned of zero
 * means they are equal.
 * param str1
 *   The first string to compare
 * param str2
 *   The second string to compare
 * returns less than 0 if str1 is less than str2, 0 if str1 is equal to str2,
 * greater than 0 if str1
 */
int strncmp(char *str1, char *str2, unsigned int len) {
  int i,j,k;
  i = j = k = 0;
/* Add up str1 */
  while(str1[i] != 0) {
    j = j + (int)str1[i];
    i++;
  }
  
  i = 0;

/* Add up str2 */
  while(str2[i] != 0) {
    k = k + (int)str2[i];
    i++;
  }

  return (j - k);
}
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
 */
void *memset(void *dest, const int src, unsigned int n) {
	int i = 0;
	while(i < n) {
		*((char *)dest + i) = (char)src;
		i++;
	}
	return dest;
}	
