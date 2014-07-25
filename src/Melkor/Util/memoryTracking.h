#ifndef MEMORYTRACKING_H
#define MEMORYTRACKING_H

/** Include this header to use new and delete that record the file
and line number where allocations occur.
Use this to find memory leaks.
Also include the following lines after the #include statement:
#ifdef _DEBUG
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#endif // _DEBUG
**/

#ifdef _DEBUG

// This warning is: 
// "no matching operator delete found; memory will not be freed if initialization throws an exception"
// We can ignore it since we supply a delete function that overrides the global one
// it just doesn't handle exceptions
#pragma warning( disable : 4291 )

#include <stdlib.h>
#include <malloc.h>
#include "memoryHelper.h"

inline void * __cdecl operator new(unsigned int size, const char *file, int line)
{
	void *ptr = (void *)malloc(size);
	AddTrack((unsigned int)ptr, size, file, line);
	return(ptr);
};

inline void __cdecl operator delete(void *p)
{
	RemoveTrack((unsigned int)p);
	free(p);
};

/*inline void * __cdecl operator new[](unsigned int size, const char *file, int line)
{
	void *ptr = (void *)malloc(size);
	AddTrack((unsigned int)ptr, size, file, line);
	return(ptr);
};

inline void __cdecl operator delete[](void *p)
{
	RemoveTrack((unsigned int)p);
	free(p);
};
*/
#endif

#endif // MEMORYTRACKING_H