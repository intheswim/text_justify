#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h> // isalpha, isdigit.
#include <wchar.h>
#include <assert.h>
#include <vector>

#include "justifyText.h"
#include "compat.h"

#define TAB_SPACES   4

#define TAB         '\t'
#define SPACE       ' '
#define NEWLINE     '\n'
#define BACKTICK    '`'

#define SPACE_WSTR  L" "
#define NEWLN_WSTR  L"\n"

struct paragraph
{
    std::vector<wchar_t *> words;
    unsigned int header_tabs, header_spaces;

    paragraph ()
    {
        header_tabs = 0, header_spaces = 0;
    }

    ~paragraph ()
    {
        for (wchar_t *ptr : words)
        {
            delete[] ptr;
        }
    }
    int header_len()
    {
        return header_tabs * TAB_SPACES + header_spaces;
    }
};

static void writeWords (struct paragraph & par, int start_index, int word_count, FILE * out, const int LIMIT)
{
    int len = 0;

    if (start_index == 0)
    {
        len = par.header_len();
    }

    for (int i = start_index, j = 0; j < word_count; j++, i++)
    {
        wchar_t *w = par.words[i];
        len += wcslen (w);
    }

    int space_places = word_count - 1;

    assert (space_places > 0);

    int norm_spaces = (LIMIT - len) / space_places;

    int extra = LIMIT - len - space_places * norm_spaces;

    assert (extra >= 0);
    assert (extra < space_places);

    bool * extra_spaces = new bool[space_places]();

    int cnt = 0;

    while (cnt < extra)
    {
        int idx = rand() % space_places;
        if (extra_spaces[idx]) continue;
        extra_spaces[idx] = true;
        cnt++;
    }

    if (start_index == 0 && par.header_len() > 0)
    {
        for (int j = 0; j < par.header_len(); j++)
        {
            fputws (SPACE_WSTR, out);
        }
    }

    for (int i = start_index, j = 0; j < word_count; j++, i++)
    {
        wchar_t *w = par.words[i];

        fputws (w, out);

        if (j == word_count - 1) break;

        for (int j = 0; j < norm_spaces; j++)
        {
            fputws (SPACE_WSTR, out);
        }

        if (extra_spaces[j])
        {
            fputws (SPACE_WSTR, out);
        }
    }

    delete[] extra_spaces;
    fputws (NEWLN_WSTR, out);
    fflush (out);
}

static void writeWordsPlain (struct paragraph & par, int start_index, int word_count, FILE * out)
{
    if (start_index == 0 && par.header_len() > 0)
    {
        for (int j = 0; j < par.header_len(); j++)
        {
            fputws (SPACE_WSTR, out);
        }
    }

    for (int i = start_index, j = 0; j < word_count; j++, i++)
    {
        wchar_t *w = par.words[i];

        fputws (w, out);
        fputws (SPACE_WSTR, out);
    }

    fputws (NEWLN_WSTR, out);
    fflush (out);
}

static void writeParagraph (struct paragraph & par, FILE * out, const int LIMIT)
{
    int line_len = 0;
    int line_word_cnt = 0;
    unsigned int word_index = 0;
    int line_first_word_index = 0;

    if (par.header_spaces > 0 || par.header_tabs > 0)
    {
        line_len = par.header_len();
    }

    while (true)
    {
        if (word_index >= par.words.size()) break;

        int len = wcslen (par.words[word_index]);

        if (len + line_len + 1 > LIMIT && line_word_cnt > 0)
        {
            // we will include this word on next line.
            writeWords (par, line_first_word_index, line_word_cnt, out, LIMIT);

            line_word_cnt = 0;
            line_len = 0;
            continue;
        } 
        if (len + line_len >= LIMIT && line_word_cnt == 0)
        {
            fputws (par.words[word_index], out);
            fputws (NEWLN_WSTR, out);
            fflush (out);
            word_index++;
            line_len = 0;
            continue;
        }

        if (line_len == 0) line_first_word_index = word_index;
        if (line_word_cnt > 0) line_len++; // space
        word_index++;
        line_word_cnt++;
        line_len += len;
        
        if (word_index >= par.words.size())
        {
            writeWordsPlain (par, line_first_word_index, line_word_cnt, out);
            break;
        }

    }
}

static void splitAndWrite (struct headerAndBuffer & ob, FILE *out, const int LIMIT)
{
    wchar_t *buffer = ob.buffer;

    if (buffer == NULL)
    {
        return;
    }

    for (int i = 0; ; i++) // replace newlines with spaces.
    {
        if (buffer[i] == 0) break;
        if (buffer[i] == NEWLINE) buffer[i] = SPACE;
    }

    int len = wcslen (buffer);

    struct paragraph par;

    par.header_tabs = ob.header_tabs;
    par.header_spaces = ob.header_spaces;

    // get words separated by spaces until length (including spaces) is less than 80

    bool last_space = false;
    int last_space_index = -1;

    for (int i = 0; i <= len; i++)
    {
        if (buffer[i] == 0 && (i - last_space_index > 1))
        {
            wchar_t *word = new wchar_t [i - last_space_index]();
            memcpy (word, buffer + last_space_index + 1, (i - last_space_index - 1) * sizeof(wchar_t));
            word[i - last_space_index - 1] = 0;
            par.words.push_back (word);
            break;
        }
        if (buffer[i] != SPACE)
        {
            last_space = false;
        }
        else 
        {
            if (buffer[i] == SPACE && last_space == false)
            {
                if (i > 0)
                {
                    wchar_t *word = (wchar_t *)malloc (sizeof(wchar_t) * (i - last_space_index) );
                    memcpy (word, buffer + last_space_index + 1, (i - last_space_index - 1) * sizeof(wchar_t));
                    word[i - last_space_index -1] = 0;
                    par.words.push_back (word);
                }
            }
            last_space = true;
            last_space_index = i;
        }
    }

    writeParagraph (par, out, LIMIT);

}

void gotNewLine (const wchar_t *buffer, FILE *out, const int LIMIT, struct headerAndBuffer & ob)
{
    if (buffer == NULL) // must be called at the end to cleanup.
    {
        splitAndWrite (ob, out, LIMIT);
        ob.reset();
        return;
    }

    if (buffer[0] == NEWLINE) // newline (empty string) : flush contents, then write string as is.
    {
        splitAndWrite (ob, out, LIMIT);
        ob.reset();
        fputws (buffer, out);
    }
    else if (buffer[0] == SPACE || buffer[0] == TAB) // string starts with space or tab.
    {
        splitAndWrite (ob, out, LIMIT);
        ob.reset();

        // count number of leading spaces & tabs.
        int i = 0;
        int space_cnt = 0, tab_cnt = 0;

        while (buffer[i] == SPACE || buffer[i] == TAB)
        {
            if (buffer[i] == SPACE) space_cnt++;
            if (buffer[i] == TAB) tab_cnt++;
            i++;
        }

        // check first non-space and non-tab symbol

        if (buffer[i] == BACKTICK)  // if quote, write as is and return.
        {
            fputws (buffer, out);
            return;
        }

        size_t len = (wcslen(buffer + i) + 1);
        if (len > 0)
        {
            ob.header_spaces = space_cnt;
            ob.header_tabs = tab_cnt;

            ob.buffer = (wchar_t *)malloc ( sizeof(wchar_t) * len);
            memset (ob.buffer, 0, sizeof(wchar_t) * len);
            wcscpy_s (ob.buffer, len, buffer + i);
        }

        return;

    }
    else
    {
        // append to buffer. if buffer is null, create it.
        if (ob.buffer == NULL)
        {
            size_t len = (wcslen(buffer) + 1);
            ob.buffer = (wchar_t *)malloc ( sizeof(wchar_t) * len);
            memset (ob.buffer, 0, sizeof(wchar_t) * len);
            wcscpy_s (ob.buffer, len, buffer);
            return;
        }
        else 
        {
            size_t len = wcslen (ob.buffer) + wcslen (buffer) + 2;

            ob.buffer = (wchar_t *)realloc (ob.buffer, len * sizeof(wchar_t));

            wcscat_s (ob.buffer, len, SPACE_WSTR);
            wcscat_s (ob.buffer, len, buffer);
        }
    }
}

bool ends_with_newline (const wchar_t *buffer)
{
    int len = wcslen (buffer);

    if (len == 0) return false;

    return (buffer[len - 1] == NEWLINE);
}

void custom_getwline (wchar_t **buffer, size_t *bufLen, FILE *fp)
{
    bool error = false;

    if (buffer == NULL || *bufLen == 0)
    {
        if (buffer)
        {
            delete[] *buffer;
        }

        *bufLen = 256;
        *buffer = new wchar_t[*bufLen];
    }

    memset (*buffer, 0, (*bufLen) * sizeof(wchar_t));

    int pos = ftell (fp);

    error = (nullptr == fgetws (*buffer, *bufLen, fp));

    // this is custom getline() alternative for wchar_t
    while (!error && !ends_with_newline(*buffer) && !feof(fp))
    {
        delete[] *buffer;
        *bufLen = *bufLen * 2;
        *buffer = new wchar_t [*bufLen];

        fseek (fp, pos, SEEK_SET);

        memset (*buffer, 0, *bufLen * sizeof(wchar_t));

        error |= (nullptr == fgetws (*buffer, *bufLen, fp));
    }
}
