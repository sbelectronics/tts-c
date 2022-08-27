/*
 * ttsmain.c
 * NRL-based text-to-speech for CP/M-8000, Scott Baker, https://www.smbaker.com/
 * 
 * Based on Retrospeak by Jason Lane
 *
 * zcc rules.c
 * zcc tts.c
 * zcc ttsmain.c
 * asz8k -o inout.o inout.8kn
 * ld8k -w -s -o say.z8k startup.o rules.o tts.o ttsmain.o inout.o -lcpm 
 * 
 * zcc compiler is kinda weird, and some weirdisms are in this C source to deal
 * with it.
 */

#include <stdio.h>
#include "tts.h"
#include "rules.h"

#ifdef LINUX
#include <string.h>
#endif

#ifndef LINUX
/* CP/M-8000 turn off stuff we don't need. It only saves about 4K. */
#include "option.h"
NOLONG NOFLOAT NOFILESZ NOASCII

/* Z8000 port input and output functions, linked from assembly */
VOIDRET outp(x,y);
char inp(x);
#endif

#define ISSPACE(c) (((c)==' ') || ((c)=='\t') || ((c)=='\r') || ((c)=='\n'))

int quiet=0;

VOIDRET outPhon(phon)
char phon;
{
#ifndef LINUX
    /* Z8000 phoneme output function.
     *
     * My SP0256 is on port 0x59, and my status readback is on the same
     * port. I don't use interrupts. I poll.
     *
     * For some future person who wants to reuse this with whatever your
     * personal text-to-speech project is, this is where you write your
     * routine to interact with your SP0256A-AL2 ic.
     */
    while ((inp(0x59)&4)==4) {
       /* busy wait while phoneme is speaking */
    }
    outp(0x59, phon);
#endif
}

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

            speakPhoneme(outPhon, !quiet);

            *s = tmp;
        }
    }
}

char line[1024];

VOIDRET banner()
{
    printf("say.z8k by Scott M Baker, www.smbaker.com\n");
    printf("use -h for command-line help\n\n");
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
    exit(0);
    VOIDRETURN;
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
            return 0;
        }
        speakLine(line);
        printf("\n");
    }
    VOIDRETURN;
}

VOIDRET speakFile(fn)
char *fn;
{
    FILE *f = fopen(fn, "rt");

    if (f==NULL) {
        printf("Failed to open %s\n", fn);
        exit(-1);
    }

    speakStream(f, 0);

    fclose(f);
    VOIDRETURN;
}

int main(argc, argv)
int argc;
char **argv;
{
    int fileMode = 0;
    int noninteractive = 0;
    int i;

    for (i=1; i<argc; i++) {
        if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"-H")==0)) {
            usage();
        } else if ((strcmp(argv[i],"-f")==0) || (strcmp(argv[i],"-F")==0)) {
            fileMode=1;
        } else if ((strcmp(argv[i],"-d")==0) || (strcmp(argv[i],"-D")==0)) {
            setDebug(1);
        } else if ((strcmp(argv[i],"-q")==0) || (strcmp(argv[i],"-Q")==0)) {
            quiet=1;
        } else {
            if (fileMode) {
                speakFile(argv[i]);
            } else {
                speakLine(argv[i]);
            }
            noninteractive=1;
        }
    }
    
    if (!noninteractive) {
        banner();
        speakStream(stdin, 1);
    }

    return 0;
}
