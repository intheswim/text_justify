#pragma once

#ifdef _MSC_VER
#include <io.h>
#define PATH_MAX 260
#define F_OK  0
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define strdup _strdup
#define access _access
#else 
#include <unistd.h>  // access()
#include <linux/limits.h>  // PATH_MAX
#endif 

#include <stdio.h>

#ifndef _MSC_VER

// MS compatibility adapters for "secure" functions.

void strerror_s(char *buffer, size_t bufLen, int err);

int fopen_s (FILE **fp, const char *name, const char *mode);

int wcscat_s (wchar_t * dest, size_t dest_len, const wchar_t * src);

int wcscpy_s( wchar_t * dest, size_t destsz, const wchar_t * src);

int tmpnam_s(char* temp_name, size_t sizeInChars);

#endif // _MSC_VER
