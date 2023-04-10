/**************************************************************************//**
 * @author  Ben Haubrich                                                   
 * @file    cstring.c                                                      
 * @date    June 18th, 2019                                                 
 * @details \b Synopsis: \n String manipulation functions                                   
 *****************************************************************************/
#include <mem.h>
#include <types.h>
#include <stdarg.h> //stdargs can be used because all it does is substitute
                    //compiler built-in functions
#include <syscalls.h>

/**
 * @brief
 *   Copy the string from src into dst.
 * @param src:
 *   The source string to be copied
 * @param dst:
 *   The destination string to be copied to
 * @param len:
 *   The maximum number of bytes to copy
 * @return
 *   -1 on failure, 0 on success
 */
int strncpy(char *dst, const char *src, const unsigned int len) {
	int i = 0;
	while(i < len) {
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return 0;
}

/**
 * @brief
 *   Join two strings together
 *
 * Concatenate dest with with len bytes of str. dest will be null terminated
 * when the function returns. dest must be null terminated when the function
 * is called.
 *
 * @param dest
 *   The destination string
 * @param str
 *   The string to be appended onto dest
 * @param len
 *   The number of bytes from str to append.
 * @return
 *   A pointer to dest.
 */
char *strncat(char *dest, const char *str, const unsigned int len) {
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

/**
 * @brief
 *   Get the length of string.
 * @param string
 *   The string you want the length of
 * @return
 *   The number of bytes in the string or -1 on failure
 */
unsigned int strlen(const char *string) {
	int i = 0;
	while(0 != *(string + i)) {
		i++;
	}
return i;
}

/**
 * @brief
 *   Compare two strings and return the difference
 *
 * Compares up to len bytes of str1 to str2 by returning the difference of the
 * ASCII value of the two strings. A value return that is less than zero or
 * greater than zero means the strings are not equal, a value returned of zero
 * means they are equal.
 *
 * @param str1
 *   The first string to compare
 * @param str2
 *   The second string to compare
 * @param len
 *   Maximum number of bytes to compare
 * @return
 *   less than 0 if str1 is less than str2, 0 if str1 is equal to str2,
 *   greater than 0 if str1
 */
int strncmp(const char *str1, const char *str2, const unsigned int len) {
  int i,j,k;
  i = j = k = 0;
/* Add up str1 */
  while(str1[i] != 0 && i < len) {
    j = j + (int)str1[i];
    i++;
  }
  
  i = 0;

/* Add up str2 */
  while(str2[i] != 0 && i < len) {
    k = k + (int)str2[i];
    i++;
  }

  return (j - k);
}

/**
 * @brief
 *   Copy n bytes from memory area src to memory area dst.
 * @param dest
 *   The destination memory area to copy to
 * @param src
 *   The source of the memory to copy from
 * @param n
 *   Number of bytes to copy
 * @return
 *   NULL on failure, pointer to dest on success.
 */
void *memcpy(void *dest, const void *src, const unsigned int n) {
	int i = 0;
	while(i < n) {
		*((char *)dest + i) = *((char *)src + i);
			i++;
	}
	return dest;
}

/**
 * @brief
 *   Not implemented
 */
void *memmove(void *dest, const void *src, const unsigned int n) {
  return NULL;
}

/**
 * @brief
 *   Fills the first n bytes of dest with the constant value of src.
 * @param dest
 *   The destination memory area to set
 * @param src
 *   The source of the memory to that is used to set dest
 * @param n
 *   Number of bytes to copy
 * @return
 *   The destination pointer is returned
 */
void *memset(void *dest, const int src, const unsigned int n) {
	int i = 0;
	while(i < n) {
		*((char *)dest + i) = (char)src;
		i++;
	}
	return dest;
}

/**
 * @brief
 *   Not implemented
 */
int memcmp(const void *s1, const void *s2, unsigned int n) {
  return -1;
}

/**
 * @brief
 *   Reverse a string.
 * Reverse the characters of the string s in place. The behaviour of this
 * function is undefined if the string is not null terminated.
 * @param s
 *   The string to reverse
 */
void reverse(char *s) {
  int i, j, stringlen;
  char b, d;
  char *c;

  if(!*s) {
    return;
  }

  c = s;
/* Move the pointer position to the last character. */
  while(*c != 0) {
    c++;
  }
/* Subtract the relative addresses */
  stringlen = (int)(c - s);
  j = stringlen;
  i = 0;

  while(i < j) {
/* Copy the left most character */
    b = s[i];
/* Copy the right most character */
    d = s[j-1];
/* Put the left most in the right most spot. */
    s[j-1] = b;
/* Put the right most in the left most spot */
    s[i] = d;
    j--;
    i++;
  }
}

/**
 * @brief
 *   convert the integer n into the character string s.
 * @param n
 *   The integer to convert to a string
 * @param s
 *   The pointer to the string which will hold the value
 */
void itoa(int n, char *s) {
  int i = 0;

  /* Record sign */
  int sign = n;
  if(n < 0) {
    n = -1 * n;
  }

  /* Generate digits in reverse order */
  while(n > 0) {
    s[i] = n % 10 + 48;
    n /= 10;
    i++;
  }
  if(sign < 0) {
    s[i] = '-';
    i++;
  }
  s[i] = '\0';
  reverse(s);
}

/**
 * @brief
 *   Converts the integer h to character string s in hexidecimal format.
 * @param h
 *   The hexedecimal to convert to a string
 * @param s
 *   Pointer to the string that will hold the value
 */
void htoa(const int h, char *s) {
  int i = 0;
  unsigned long int tohex;
/* Remainder of tohex */
  unsigned long int tohex_r;

  tohex = h;

  while(tohex >= 16) {
    tohex_r = tohex % 16;
    tohex = tohex / 16;
/* Hex digits greater than 9 are reprsented by letters A-F. A-F starts 65, */
/* but since we are already at least above 10 in decimal, just add 55 to it. */
    if(tohex_r > 9) {
      s[i] = tohex_r + 55;
    }
    else {
      s[i] = tohex_r + 48;
    }
    i++;
  }
/* Print the last digit that was less than 16. */
  if(tohex > 9) {
    s[i] = tohex + 55;
  }
  else {
    s[i] = tohex + 48;
  }
/* First two values are symbolic for hex notation. */
  s[++i] = 'x';
  s[++i] = '0';
  s[++i] = '\0';
  reverse(s);
}

/**
 * @brief
 *   print a formatted string
 */
void printf(char *s, ...) {
  uint32_t hex; /* Holds values for hex numbers */
  int integer;
  int bytes_remaining = 64; /* Bytes left in the buffer */
  int i = 0;
  int len; /* for holding return value from strlen() */
/* Build strings 64 bytes at a time before sending to the uart to reduce the */
/* amount of system calls to write(). */
  char buf[64] = {'\0'};
/* For concatenating single chars of s */
  char s_char;
/* Strings for holding the string number. Sizes of the arrays are the max */
/* number of characters needed to represent the largest integer on this */
/* processor. hex has 2 extra for "0x" at the beginning. */
  char hex_string[sizeof(uint32_t)*2+2];
  char int_string[sizeof(uint32_t)*2+2];
  va_list format_strings;
  va_start(format_strings, s);
/* Print one char at a time, inserting the va_args whenever a specifier is */
/* encountered. */
  while(s[i] != '\0') {
    if(s[i] == '%') {
      switch(s[++i]) {
      case('x') :
        hex = va_arg(format_strings, uint32_t);
        memset(hex_string, 0, sizeof(uint32_t)*2+2);
        htoa(hex, hex_string);
        len = strlen(hex_string);
        if(bytes_remaining - len > 0) {
          strncat(buf, hex_string, len);
          bytes_remaining -= len;
        }
        else {
          write(buf);
          bytes_remaining = 64;
          buf[0] = '\0';
        }
      break;
      case('i') :
        integer = va_arg(format_strings, int);
        memset(int_string, 0, sizeof(uint32_t)*2+2);
        itoa(integer, int_string);
        len = strlen(int_string);
        if(bytes_remaining - len > 0) {
          strncat(buf, int_string, len);
          bytes_remaining -= len;
        }
        else {
          write(buf);
          bytes_remaining = 64;
          buf[0] = '\0';
        }
      break;
      case('d') : /* Same thing as %i */
        integer = va_arg(format_strings, int);
        memset(int_string, 0, sizeof(uint32_t)*2+2);
        itoa(integer, int_string);
        len = strlen(int_string);
        if(bytes_remaining - len > 0) {
          strncat(buf, int_string, len);
          bytes_remaining -= len;
        }
        else {
          write(buf);
          bytes_remaining = 64;
          buf[0] = '\0';
        }
      break;
      }
    }
    else if(bytes_remaining > 0) {
      s_char = s[i];
      strncat(buf, &s_char, 1);
      bytes_remaining--;
    }
    else {
      write(buf);
      bytes_remaining = 64;
      buf[0] = '\0';
    }
    i++;
  }/* Delimits while(s[i] != '\0')*/
  va_end(format_strings);
  write(buf);
  return;
}
