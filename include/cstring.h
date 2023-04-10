/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file    cstring.h                                                       
 * @date   	June 21st, 2019                                                 
 * @details \b Synopsis:	\n String and memory manipulation functions.                                  
 *****************************************************************************/
#ifndef __CSTRING_H__
#define __CSTRING_H__

int strncpy(char *, const char *, const unsigned int);
char *strncat(char *, const char *, const unsigned int);
unsigned int strlen(const char *);
int strncmp(const char *, const char *, const unsigned int);
void *memcpy(void *, const void *, const unsigned int);
void *memmove(void *, const void *, const unsigned int);
void *memset(void *, const int, const unsigned int);
int memcmp(const void *, const void *, unsigned int);
void reverse(char *);
void itoa(int, char *s);
void htoa(const int, char *s);
void printf(char *, ...);
#endif /*__CSTRING_H__*/
