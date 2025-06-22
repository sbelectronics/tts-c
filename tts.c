/*
 * tts.c
 * NRL-based text-to-speech in C, Scott Baker, https://www.smbaker.com/
 * 
 * Based on Retrospeak by Jason Lane
 */

#include <stdio.h>
#include "rules.h"
#include "tts.h"
#include "outphon.h"

#ifdef LINUX
#include <string.h>
#else
#ifdef ISIS
#include <string.h>
#else
#ifdef OLIVETTI
#include <string.h>
#else
#ifdef H8_80186
#include <string.h>
#else
/* CPM8K stuff here */
#endif
#endif
#endif
#endif

/* watch out for side effects in the argument when using these macros */

#define ISDIGIT(c) (((c)>='0') && ((c)<='9'))
#define ISLOWER(c) (((c)>='a') && ((c)<='z'))
#define ISUPPER(c) (((c)>='A') && ((c)<='Z'))
#define ISVOWEL(c) (((c)=='A') || ((c)=='E') || ((c)=='I') || ((c)=='O') || ((c)=='U'))
#define ISCONSONANT(c)  (ISUPPER(c) && (!ISVOWEL(c)))

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE (!TRUE)
#endif

#define MAX_WLEN  32

char wordBuf[MAX_WLEN+3];
char phonBuf[MAX_WLEN * 2 + 1]; /* if every letter was turned into two phonemes */
char leftBuf[MAX_WLEN+3];
char leftPat[MAX_WLEN+3];
int phonCount;
int debug = 0;

VOIDRET setDebug(d)
int d;
{
    debug = d;
}

/* add a phoneme to the phoneme buffer */
VOIDRET addPhoneme(phon)
PHONEME phon;
{
    /* TODO: needs bounds check */
    phonBuf[phonCount] = phon;
    phonCount++;

    VOIDRETURN;
}

/* reset the phoneme buffer for a new word */
VOIDRET resetPhoneme()
{
    phonCount = 0;

    VOIDRETURN;
}

/* speak the phoneme buffer. Calls the callback to talk to the IC. */
VOIDRET speakPhoneme(speak,prt)
int speak;
int prt;
{
    int i;
    
    for(i=0; i<phonCount; i++) {
        if (speak) {
            outPhon(phonBuf[i]);
        }
        if (prt) {
            printf("%s ", phones[phonBuf[i]]);
        }
    }
}

/* print the phoneme buffer */
VOIDRET printPhoneme()
{
    speakPhoneme(0, 1);
}

int lrMatch(pattern, context, right)
char *pattern;
char *context;
int right;
{
    char *ctx = context;

    if (debug) {
        printf("LRMatch: %s %s %d\n", pattern, context, right);
    }

    /* invariant: we already know pattern!=ANYTHING and for left rules we're already reversed */ 

    while (*pattern) {
        char pc = *pattern;

        switch (pc) {
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '\'':
            case ' ':
                /* simple text of space */
                if (pc != *ctx) {
                    return FALSE;
                }
                ctx++;
                break;

            case '#':
                /* one or more vowels */
                if (!ISVOWEL(*ctx)) {
                    return FALSE;
                }
                ctx++;
                while (ISVOWEL(*ctx)) {
                    ctx++;
                }
                break;

            case ':':
                /* zero or more consonant */
                while (ISCONSONANT(*ctx)) {
                    ctx++;
                }
                break;

            case '^':
                /* one consonant */
                if (!ISCONSONANT(*ctx)) {
                    return FALSE;
                }
                ctx++;
                break;

            case '.':
                /*  B, D, V, G, J, L, M, N, R, W, Z */
                switch (*ctx) {
                    case 'B':
                    case 'D':
                    case 'V':
                    case 'G':
                    case 'J':
                    case 'L':
                    case 'M':
                    case 'N':
                    case 'R':
                    case 'W':
                    case 'Z':
                        break;
                    default:
                        return FALSE;
                }
                ctx++;
                break;

            case '+':
                /* E, I, Y */
                switch (*ctx) {
                    case 'E':
                    case 'I':
                    case 'Y':
                        break;
                    default:
                        return FALSE;
                }
                ctx++;
                break;

            case '%':
                if (((*ctx) == 'I') && (*(ctx+1) == 'N') && (*(ctx+2) == 'G')) {        /* ING */
                    ctx+=4; /* XXX suspicious */
                } else if (((*ctx) == 'E') && (*(ctx+1) == 'R') && (*(ctx+2) == 'Y')) { /* ERY */
                    ctx+=4; /* XXX suspicious */
                } else if (((*ctx) == 'E') && (*(ctx+1) == 'R')) {                      /* ER */
                    ctx+=3; /* XXX suspicious */
                } else if (((*ctx) == 'E') && (*(ctx+1) == 'S')) {                      /* ES */
                    ctx+=3; /* XXX suspicious */
                } else if (((*ctx) == 'E') && (*(ctx+1) == 'D')) {                      /* ED */
                    ctx+=3; /* XXX suspicious */
                } else if ((*ctx) == 'E') {                                             /* E */
                    ctx+=2; /* XXX suspicious */
                } else {
                    return FALSE;
                }
                break;

            default:
                /* this shouldn't happen */
                printf("UNKNOWN: %c\n", pc);
                return FALSE;
       }

nextpat:
       pattern++;
   }

   if (debug) {
       printf("  MATCH!\n");
   }

   return 1;
}


rule *findRule(word, index, curRule)
char *word;
int index;
rule *curRule;
{
    while (1) {
        char *w, *p;

        if (curRule->left == NOMORE) {
            /* no more rules for us to check... We have failed. */
            return NULL;
        }

        /* inline the string compare, for speed */
        w = &word[index];
        p = curRule->match;
        while (*p) {
            if (*p != *w) {
                goto nomatch;
            }
            p++;
            w++;
        }

        /* at this point, *w is conveniently pointing to rightWord */
        if (curRule->right != ANYTHING) {
            if (!lrMatch(curRule->right, w, TRUE)) {
                goto nomatch;
            }
        }

        if (curRule->left != ANYTHING) {
            int i;
            char *lp;

            /* referse leftBuf */
            lp = leftBuf;
            for (i=index-1; i>=0; i--) {
                *(lp++) = word[i];
            }
            *lp = '\0';

            /* reverse leftPat */
            lp = leftPat;
            for (i=strlen(curRule->left)-1; i>=0; i--) {
                *(lp++) = curRule->left[i];
            }
            *lp = '\0';

            if (!lrMatch(leftPat, leftBuf, FALSE)) {
                goto nomatch;
            }
        }
        
        if (debug) {
            printf("  MATCH RULE %d\n", (int) (curRule-rules));
        }

        return curRule;        

nomatch:
        curRule++;
    }
}

VOIDRET translateWord(s)
char *s;
{
    char *w;
    int wLen;
    int index;

    resetPhoneme();

    if (debug) {
        printf("TW: %s\n", s);
    }

    /* Copy word into wordbuf, with space at either end as a sentinel and
     * converting to upper case. */
    wLen = 0;
    w = wordBuf;
    *(w++) = ' ';
    while ((*s) && (wLen < MAX_WLEN)) {
        if (ISLOWER(*s)) {
            *(w++) = *s - 'a' + 'A';
        } else {
            *(w++) = *s;
        }
        s++;
        wLen++;
    }
    *(w++) = ' ';
    *(w++) = '\0';

    for (index=1; index<=wLen; ) {
        int isPunct;
        int ruleIndex;
        char c = wordBuf[index];
        rule *firstRule, *fndRule;

        ruleIndex = rulemap[c];
        if (ruleIndex == -1) {
            index++;
            continue;
        }

        firstRule = &rules[ruleIndex];

        fndRule = findRule(wordBuf, index, firstRule);
        if (fndRule == NULL) {
            /* this probably shouldn't happen */
            printf("NO RULE!\n");
            index++;
        } else {
            /* add all the phonemes to wordbuf */
            char *x = fndRule->output;
            while (*x != STOP) {
                addPhoneme(*x);
                x++;
            }
            index += strlen(fndRule->match);
        }
    }

    VOIDRETURN;
}
