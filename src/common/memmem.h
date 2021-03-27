#pragma once 
#include <stdint.h>

//search in mem1 for mem2, return idx or -1 if not found
int MemMem(const char* mem1, const char* mem2, int len1, int len2); 
