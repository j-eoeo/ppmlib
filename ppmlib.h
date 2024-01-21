/* $Id: ppmlib.h 14 2023-12-31 11:29:03Z nishi $ */
#ifndef __PPMLIB_H__
#define __PPMLIB_H__

#include <stddef.h>

void ppmlib_parse(unsigned const char* _data, size_t size, int* width, int* height, int* type, unsigned char** buffer, void(*read_data)(int, int, int, int, int));

#endif
