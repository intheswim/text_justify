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