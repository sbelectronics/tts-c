/* zcc doesn't understand "void" */
#define VOIDRET int
#define VOIDRETURN return 0

typedef VOIDRET (*OutPhonFn)(); /* char phoneme */

VOIDRET addPhoneme(phon);
VOIDRET speakPhoneme(cb,prt);
extern VOIDRET printPhoneme();
extern VOIDRET translateWord(s);
extern VOIDRET setDebug(d);
