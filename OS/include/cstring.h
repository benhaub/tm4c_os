/******************************************************************************
 * Authour  :	Ben Haubrich                                                    *
 * File     :	cstring.h                                                       *
 * Synopsis :	String manipulation functions.                                  *
 * Date     :	June 21st, 2019                                                 *
 *****************************************************************************/
#ifndef __CSTRING_H__
#define __CSTRING_H__

int strncpy(char *, char *, unsigned int);
char *strncat(char *, char*, unsigned int);
unsigned int strlen(char *);
int strncmp(char *, char *, unsigned int);
void *memcpy(void *, const void *, unsigned int);
void *memmove(void *, const void *, unsigned int);
void *memset(void *, const int, unsigned int);
int memcmp(const void *, const void *, unsigned int);
void reverse(char *);
void htoa(int, char *s);
void itoa(int, char *s);
void printf(const char *, ...);
#endif /*__CSTRING_H__*/
