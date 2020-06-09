/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : cstring.h                                                       *
 * Synopsis : String manipulation functions                                   *
 * Date     : June 18th, 2019                                                 *
 *****************************************************************************/
#include <mem.h>
#include <types.h>
#include <hw.h> //For uart1_tchar
#include <stdarg.h> //stdargs can be used because all it does is substitute
                    //compiler built-in functions
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

/*
 * Reverse the characters of the string s in place. The behaviour of this
 * function is undefined if the string is not null terminated.
 */
void reverse(char *s) {
  int i, j, stringlen;
  char b, d;
  char *c;

  if(!s) {
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

/*
 * convert the integer n into the character string s.
 */
void itoa(int n, char *s) {
  int i = 0;

  /* Record sign */
  int sign = n;
  if(n < 0) {
    n = -1 * n;
  }

  /* Generate digits in reverse order */
  do {
    /* Get next digit. */
    s[++i] = n % 10 + 48;
  } while((n /= 10) > 0);

  if(sign < 0) {
    s[++i] = '-';
  }
  s[++i] = '\0';
  reverse(s);
}

/*
 * Converts the integer h to character string s in hexidecimal format.
 */
void htoa(int h, char *s) {
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

/*TODO:
 * Having issues grabbing the second argument from va_list and printing extra
 * characters on htoa.
 */    
void printf(const char *s, ...) {
  int i, j;
  word hex; /* Holds values for hex numbers */
  char hex_string[8]; /* Contains the string that we can print. */
  int integer;
  char integer_string[10];
  va_list format_strings;
  i = j = 0;
/* Count all the format specifiers. */
  while(s[i] != '\0') {
    if(s[i] == '%') {
      j++;
    }
    i++;
  }
  va_start(format_strings, j);
  i = j = 0;
/* Print one char at a time, inserting the va_args whenever a specifier is */
/* encountered. */
  while(s[i] != '\0') {
    if(s[i] == '%') {
      switch(s[++i]) {
        case('x') :
          hex = va_arg(format_strings, word);
          htoa(hex, hex_string);
          while(hex_string[j] != '\0') {
            uart1_tchar(hex_string[j]);
            j++;
          }
        case('i') :
          integer = va_arg(format_strings, int);
          itoa(integer, integer_string);
          while(integer_string[j] != '\0') {
            uart1_tchar(integer_string[j]);
            j++;
          }
      }
    }
    else {
      uart1_tchar(s[i]);
    }
    i++;
  }
  return;
}
