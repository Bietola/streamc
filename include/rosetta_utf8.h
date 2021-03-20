#ifndef _ROSETTA_UTF8
#define _ROSETTA_UTF8

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct {
	char mask;    /* char data will be bitwise AND with this */
	char lead;    /* start bytes of current char in utf-8 encoded character */
	uint32_t beg; /* beginning of codepoint range */
	uint32_t end; /* end of codepoint range */
	int bits_stored; /* the number of bits from the codepoint that fits in char */
}utf_t;
 
/* All lengths are in bytes */
int codepoint_len(const uint32_t cp); /* len of associated utf-8 char */
int utf8_len(const char ch);          /* len of utf-8 encoded char */
 
char *to_utf8(const uint32_t cp);
uint32_t to_cp(const char chr[4]);
 
#endif
