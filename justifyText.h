#pragma once

#include <stdio.h>

struct headerAndBuffer
{
    wchar_t * buffer;
    int header_spaces, header_tabs;
    headerAndBuffer () : buffer(NULL), header_spaces(0), header_tabs(0) {}
    void reset() { delete[] buffer; buffer = NULL; header_spaces = header_tabs = 0; }
};

//bool ends_with_newline (const wchar_t *buffer);
void custom_getwline (wchar_t **buffer, size_t *bufLen, FILE *fp);

void gotNewLine (const wchar_t *buffer, FILE *out, const int LIMIT, struct headerAndBuffer & output_buffer);
