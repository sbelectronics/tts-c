#include <stdio.h>
#include "tts.h"
#include "rules.h"

#define ISSPACE(c) (((c)==' ') || ((c)=='\t') || ((c)=='\r') || ((c)=='\n'))

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

        tmp=*s;
        *s='\0';

        translateWord(w);
        printPhoneme();

        *s = tmp;
    }
}

int main(argc, argv)
int argc;
char **argv;
{
    FILE *fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;    

    fp = fopen("translations.in","rt");

    while ((read = getline(&line, &len, fp)) != -1) {
        while (*(line+strlen(line)-1) == '\n') {
            *(line+strlen(line)-1) ='\0';
        }
        printf("%s ", line);
        speakLine(line);
        printf("\n");
    }
}
