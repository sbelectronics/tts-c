#include "rules.h"

#ifdef H8_80186
#define FULLPROTO
#else
#ifdef OLIVETTI
#define FULLPROTO
#endif
#endif

#ifdef FULLPROTO
#define VOIDRET void
#define VOIDRETURN return

typedef VOIDRET (*OutPhonFn)(); /* char phoneme */
VOIDRET addPhoneme(PHONEME phon);
VOIDRET speakPhoneme(OutPhonFn cb, int prt);
extern VOIDRET printPhoneme();
extern VOIDRET translateWord(char *s);
extern VOIDRET setDebug(int d);

#else
/* zcc doesn't understand "void" */
#define VOIDRET int
#define VOIDRETURN return 0

typedef VOIDRET (*OutPhonFn)(); /* char phoneme */

/* zcc doesn't like prototypes with types in tem */

VOIDRET addPhoneme(phon);
VOIDRET speakPhoneme(cb,prt);
extern VOIDRET printPhoneme();
extern VOIDRET translateWord(s);
extern VOIDRET setDebug(d);
#endif
