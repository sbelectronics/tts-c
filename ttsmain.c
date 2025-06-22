/*
 * ttsmain.c
 * NRL-based text-to-speech for CP/M-8000, Scott Baker, https://www.smbaker.com/
 * 
 * Based on Retrospeak by Jason Lane
 *
 * zcc rules.c
 * zcc tts.c
 * zcc ttsmain.c
 * zcc outphon.c
 * asz8k -o inout.o inout.8kn
 * ld8k -w -s -o say.z8k startup.o rules.o tts.o ttsmain.o inout.o outphon.o -lcpm 
 * 
 * zcc compiler is kinda weird, and some weirdisms are in this C source to deal
 * with it.
 */

#include <stdio.h>
#include "tts.h"
#include "outphon.h"
#include "rules.h"

#ifdef LINUX
#else
#ifdef ISIS
#define __STDC_ABI_ONLY
#include <stdlib.h> /* exit(), inp(), outp() */
#else
#ifdef MULTIBUS
/* no imports */
#else
#ifdef OLIVETTI
#include "oliport.h"
#else
#ifdef H8_80186
#include <process.h>
#include <conio.h>
#else
/* CP/M-8000 turn off stuff we don't need. It only saves about 4K. */
#include "option.h"
NOLONG NOFLOAT NOFILESZ NOASCII
#endif
#endif
#endif
#endif
#endif

#define ISSPACE(c) (((c)==' ') || ((c)=='\t') || ((c)=='\r') || ((c)=='\n'))

int quiet=0;
int noDevice=0; /* if set, don't output to the device */

int speakLine(s)
char *s;
{
    char *w = s;
    char tmp;

    while (*s) {
        while (ISSPACE(*s)) {
            s++;
        }
        w = s; /* start of word */
        while ((*s) && (!ISSPACE(*s))) {
            s++;
        }

        if (w!=s) {
            tmp = (*s);
            (*s) = '\0';

            translateWord(w);

            addPhoneme(3); /* add PA4 */

            speakPhoneme(!noDevice, !quiet);

            *s = tmp;
        }
    }
    return 0;
}

char line[1024];

VOIDRET banner()
{
    printf("say.z8k by Scott M Baker, www.smbaker.com\n");
    printf("use -h for command-line help\n");
    printf("type 'bye' to exit interactive mode\n\n");
    VOIDRETURN;
}

VOIDRET usage()
{
    banner();
    printf("say \"a bunch of words\" ... speak the words\n");
    printf("say a bunch of words ... speak the words\n");
    printf("say -f <filename> ... speak a file\n");
    printf("\noptions:\n");
    printf("  -d ... debug\n");
    printf("  -q ... quiet\n");
    printf("  -r ... repeat the last line\n");
    printf("  -n ... no device\n");
    printf("  -h ... this help\n");
    exit(0);
    VOIDRETURN;
}

int myToLower(c) 
    char c;
{
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    } else {
        return c;
    }
}

int isBye(s)
    char *s;
{
    if (myToLower(*s) != 'b') {
        return 0;
    }
    s++;
    if (myToLower(*s) != 'y') {
        return 0;
    }
    s++;
    if (myToLower(*s) != 'e') {
        return 0;
    }
    return 1;
}

int isFlag(s,f)
    char *s;
    char f;
{
    if ((*s!='-') && (*s!='/')) {
        return 0;
    }
    s++;
    return (myToLower(*s) == f);
}

VOIDRET speakStream(f, prompt)
FILE *f;
int prompt;
{
    while (1) {
        char *res;

        if (prompt) {
            printf(">");
        }
        res = fgets(line, sizeof(line)-1, f);
        if (res == NULL) {
            /* EOF */
            VOIDRETURN;
        }
        if (isBye(line)) {
            /* mostly for ISIS as I didn't have another good way to break out.
             * avoided using stricmp due to portability issues.
             */
            VOIDRETURN;            
        }
        speakLine(line);
        printf("\n");
    }
    VOIDRETURN;
}

VOIDRET speakFile(fn)
char *fn;
{
#ifdef ISIS
    printf("file mode unsupported\n");
#else
    FILE *f = fopen(fn, "rt");

    if (f==NULL) {
        printf("Failed to open %s\n", fn);
        exit(-1);
    }

    speakStream(f, 0);

    fclose(f);
    VOIDRETURN;
#endif
}

int main(argc, argv)
int argc;
char **argv;
{
    int fileMode = 0;
    int repeat = 0;
    int noninteractive = 0;
    int i;

again:
    for (i=1; i<argc; i++) {
        if (isFlag(argv[i], 'h')) {
            usage();
        } else if (isFlag(argv[i], 'f')) {
            fileMode=1;
        } else if (isFlag(argv[i], 'd')) {
            setDebug(1);
        } else if (isFlag(argv[i], 'q')) {
            quiet=1;
        } else if (isFlag(argv[i], 'r')) {
            repeat=1;
        } else if (isFlag(argv[i], 'n')) {
            noDevice=1;
        } else {
            if (fileMode) {
                speakFile(argv[i]);
            } else {
                speakLine(argv[i]);
            }
            noninteractive=1;
        }
    }
    if (repeat) {
        goto again;
    }
    
    if (!noninteractive) {
        banner();
        speakStream(stdin, 1);
    }

    if (!quiet) {
        printf("\n");
    }

    return 0;  
}
