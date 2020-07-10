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
    void closeFiles ()
    {
        if (out) fflush (out);

        if (fp) { fclose (fp); }
        if (out) { fclose (out); }

        fp = out = nullptr;
    }
};

struct ProgArgs
{
    char *inputFile;
    char *outputFile;
    bool update;
    bool overwrite;
    int width;
    ProgArgs ()
    {
        inputFile = outputFile = nullptr;
        update = overwrite = false;
        width = 80;
    }
    ~ProgArgs ()
    {
        free (inputFile);
        free (outputFile);
    }
    void printSyntax ()
    {
        printf ("syntax: justify [-f -u -wLINE_WIDTH] inputFile.txt [outputFile.txt] \n");
        printf ("\t -f - overwrite existing outputFile.txt \n");
        printf ("\t -u - update inputFile.txt \n");
        printf ("\t -wLINE_WIDTH - optional line width. Dafault is 80 \n");
    }

    bool parseArguments (int argc, char *argv[])
    {
        if (argc < 3)
        {
            return false;
        }

        int first_file_arg = -1;

        for (int i = 1; i < argc; i++)
        {
            char *arg = argv[i];

            if (*arg == '-')
            {
                if ((i > first_file_arg) && (first_file_arg > 0))
                {
                    // incorrect argument sequence.
                    return false;
                }

                if (strcasecmp (arg, "-u") == 0)
                {
                    update = true;
                }
                else if (strcasecmp (arg, "-f") == 0)
                {
                    overwrite = true;
                }
                else if (strncasecmp (arg, "-w", 2) == 0)
                {
                    width = atoi (arg + 2);

                    if (width <= 40 || width > 1024)
                    {
                        fprintf (stderr, "Cannot set line width to %d.\n", width);
                        if (width > 0)
                        {
                            fprintf (stderr, "Use values between 40 and 1024\n");
                        }

                        return false;
                    }
                }
                else 
                {
                    fprintf (stderr, "Unknown parameter: %s\n", arg);
                    return false;
                }
            }
            else // file names
            {
                if (first_file_arg < 0)
                    first_file_arg = i;

                if (!inputFile) 
                    inputFile = strdup (arg);
                else 
                    outputFile = strdup (arg);
            } 
        }
        if (inputFile == NULL)
        {
            fprintf (stderr, "No input file. \n");
            return false;
        }
        if (outputFile == NULL && update == false)
        {
            fprintf (stderr, "No output file name give. Use -u option to overwrite. \n");
            return false;
        }
        if (outputFile && update)
        {
            fprintf (stderr, "Extra parameter given with -u option. \n");
            return false;
        }
        if (overwrite && update)
        {
            fprintf (stderr, "Flags -f and -u cannot be used together. \n");
            return false;
        }

        return true;
    }
};

////////////////////////////////////////////////////////////////////////////
// justify -u Filename.txt
// justify -f -w70 Filename.old Filename.new (-f force overwrite)
////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
    ProgArgs prog_args;

    if (!prog_args.parseArguments (argc, argv))
    {
        prog_args.printSyntax();

        return EXIT_SUCCESS;
    }

    AutoClose ac;

    setlocale(LC_ALL, "");

    const char *inFile = prog_args.inputFile;
    const char *outFile = prog_args.outputFile;

    int errNo = fopen_s (&ac.fp, inFile, READ_MODE);

    if (!ac.fp)
    {
        char err_buffer[256];
        strerror_s(err_buffer, 255, errNo);
        fprintf (stderr, "Could not open %s. %s\n", inFile, err_buffer);
        return EXIT_FAILURE;
    }

    char temp_file_name[PATH_MAX];

    memset (temp_file_name, 0, PATH_MAX);

    if (0 != tmpnam_s (temp_file_name, PATH_MAX))
    {
        fprintf (stderr, "Unexpected error creating temp file name.\n");
        return EXIT_FAILURE;
    }

    errNo = fopen_s (&ac.out, temp_file_name, WRITE_MODE);

    if (!ac.out)
    {
        char err_buffer[256];
        strerror_s(err_buffer, 255, errNo);
        fprintf (stderr, "Could not open for writing %s. %s\n", outFile, err_buffer);
        return EXIT_FAILURE;
    }

    int _limit = prog_args.width;

    printf ("Using line width %d\n", _limit);

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

    ac.closeFiles ();

    int renameRet = 0;

    if (prog_args.update) 
    {
        remove (inFile);
        renameRet = rename (temp_file_name, inFile);
    }
    else if (prog_args.overwrite)
    {
        remove (outFile);
        renameRet = rename (temp_file_name, outFile);
    }
    else 
    {
        if (access (outFile, F_OK) != -1)
        {
            fprintf (stderr, "File %s already exists. Use -f flag to overwrite. \n", outFile);
            remove (temp_file_name);

            return EXIT_FAILURE;
        }

        renameRet = rename (temp_file_name, outFile);
    }

    if (renameRet != 0)
    {
        perror( "Error renaming temporary file: ");
        remove (temp_file_name); // cleanup

        return EXIT_FAILURE;
    }

    printf ("Justified successfully\n");

    return EXIT_SUCCESS;
}