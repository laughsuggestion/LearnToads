#include "PrecompiledHeader.h"

// these overrides for allocating memory are required by EASTL

void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return _aligned_offset_malloc(size, alignment, alignmentOffset);
}