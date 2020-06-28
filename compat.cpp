/* Text Justify utility, Copyright (c) 2020 Yuriy Yakimenko
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

#include "compat.h"
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <wchar.h>

#ifndef _MSC_VER // MS compatibility adapters for "secure" functions.

void strerror_s(char *buffer, size_t bufLen, int err)
{
    char *r = strerror_r (err, buffer, bufLen);
    assert (r);
}

int fopen_s(FILE **fp, const char *name, const char *mode) 
{
    int  ret = 0;
    assert (fp);

    *fp = fopen (name, mode);

    if (!*fp)
        ret = errno;
    return ret;
}

int wcscat_s (wchar_t * dest, size_t dest_len, const wchar_t * src)
{
    if (!src || !dest)
        return EINVAL;

    if (wcslen (src) >= dest_len)
        return ERANGE;

    wcscat (dest, src);

    return 0;
}

int wcscpy_s( wchar_t * dest, size_t dest_len, const wchar_t * src)
{
    if (!src || !dest)
        return EINVAL;

    if (wcslen (src) >= dest_len)
        return ERANGE;

    wcscpy (dest, src);

    return 0;
}

#endif // _MSC_VER