/******************************************************************************
 * Authour  :	Ben Haubrich                                                    *
 * File     :	cstring.h                                                       *
 * Synopsis :	String manipulation functions.                                  *
 * Date     :	June 21st, 2019                                                 *
 *****************************************************************************/
#ifndef __CSTRING_H__
#define __CSTRING_H__

int strncpy(char *, const char *, const unsigned int);
char *strncat(const char *, const char*, const unsigned int);
unsigned int strlen(char *);
int strncmp(char *, const char *, const unsigned int);
void *memcpy(void *, const void *, const unsigned int);
void *memmove(void *, const void *, const unsigned int);
void *memset(void *, const int, unsigned int);
int memcmp(const void *, const void *, unsigned int);
void reverse(char *);
void itoa(int, char *s);
void htoa(const int, char *s);
void printf(char *, ...);
#endif /*__CSTRING_H__*/
