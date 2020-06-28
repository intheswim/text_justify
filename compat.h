#pragma once

#include <stdio.h>

#ifndef _MSC_VER

// MS compatibility adapters for "secure" functions.

void strerror_s(char *buffer, size_t bufLen, int err);

int fopen_s (FILE **fp, const char *name, const char *mode);

int wcscat_s (wchar_t * dest, size_t dest_len, const wchar_t * src);

int wcscpy_s( wchar_t * dest, size_t destsz, const wchar_t * src);

#endif // _MSC_VER
