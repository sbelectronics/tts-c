/*
 * outphon.c
 * 
 * output the phonemes via the port.
 */

#include <stdio.h>
#include "tts.h"
#include "outphon.h"

#ifdef LINUX
#include <string.h>
#else
#ifdef ISIS
#define __STDC_ABI_ONLY
#include <string.h> /* strlen */
#include <stdlib.h> /* exit(), inp(), outp() */
#define MULTIMODULE
#else
#ifdef MULTIBUS
#define MULTIMODULE
int inp();
void outp();
#else
#ifdef OLIVETTI
#include <string.h>
#include "oliport.h"
#else
#ifdef H8_80186
#include <string.h>
#include <process.h>
#include <conio.h>
#else
/* CP/M-8000 turn off stuff here */
#endif
#endif
#endif
#endif
#endif

VOIDRET outPhon(phon)
char phon;
{
#ifdef LINUX
#else
#ifdef MULTIMODULE
    outp(MUTEPORT, 1);  /* unmute */
    while ((inp(SPEECHPORT)&1)==0) {
       /* busy wait while phoneme is speaking */
    }
    outp(SPEECHPORT, phon);
#else
#ifdef OLIVETTI
    while ((inp(0x41)&1)==0) {
       /* busy wait while phoneme is speaking */
    }
    outp(0x41, phon);
#else
#ifdef H8_80186
    while ((inp(0x43C)&1)==0) {
       /* busy wait while phoneme is speaking */
    }
    outp(0x43C, phon);
#else
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
#endif
#endif
#endif
}
