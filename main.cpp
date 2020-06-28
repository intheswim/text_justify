#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <wchar.h>
#include <locale.h>

#include "justifyText.h"
#include "compat.h"

#ifdef _MSC_VER
#define READ_MODE   "rt,ccs=UTF-8"
#define WRITE_MODE  "wt,ccs=UTF-8"
#else
#define READ_MODE   "rt"
#define WRITE_MODE  "wt"
#endif

struct AutoClose // RAII structure to close file automatically
{
    FILE * fp;
    FILE * out;
    AutoClose () : fp(0), out(0) {}
    ~AutoClose ()
    {
        if (fp) { fclose (fp); }
        if (out) { fclose (out); }
    }
    void flush ()
    {
        if (out) fflush (out);
    }
};

int main (int argc, char *argv[])
{
    if (argc < 3)
    {
        printf ("syntax: justify inputFile.txt outputFile.txt [number]\n");
        return EXIT_SUCCESS;
    }

    AutoClose ac;

    setlocale(LC_ALL, "");

    const char *inFile = argv[1];
    const char *outFile = argv[2]; 

    /* Opening file in write mode will delete all of its content. 
    We want to avoid having this when input and output are the same, 
    so this is for data safety (a better solution would involve file locking). 
    */

    if (strcmp (inFile, outFile) == 0)
    {
        fprintf (stderr, "Input and output files cannot be the same.\n");
        return EXIT_FAILURE;
    }

    int errNo = fopen_s (&ac.fp, inFile, READ_MODE);

    if (!ac.fp)
    {
        char err_buffer[256];
        strerror_s(err_buffer, 255, errNo);
        fprintf (stderr, "Could not open %s. %s\n", inFile, err_buffer);
        return EXIT_FAILURE;
    }

    errNo = fopen_s (&ac.out, outFile, WRITE_MODE);

    if (!ac.out)
    {
        char err_buffer[256];
        strerror_s(err_buffer, 255, errNo);
        fprintf (stderr, "Could not open for writing %s. %s\n", outFile, err_buffer);
        return EXIT_FAILURE;
    }

    int _limit = 80;

    if (argc == 4)
    {
        int width = atoi (argv[3]);
        if (width <= 0 || width < 40 || width > 1024)
        {
            fprintf (stderr, "Cannot set line width: %s\n", argv[3]);
            fprintf (stderr, "Using default width : %d\n", _limit);
        }
        else 
        {
            _limit = width;
            printf ("Using line width %d\n", _limit);
        }
    }

    srand(100);

    size_t bufLen = 256;
    wchar_t * buffer = new wchar_t [bufLen];

    struct headerAndBuffer hBuff;

    while (!feof (ac.fp))
    {
        custom_getwline (&buffer, &bufLen, ac.fp);

        if (wcslen (buffer) > 0)
        {
            gotNewLine (buffer, ac.out, _limit, hBuff);
        }
    }

    gotNewLine (NULL, ac.out, _limit, hBuff);

    delete[] buffer;

    ac.flush();

    printf ("Justified successfully\n");

    return EXIT_SUCCESS;
}