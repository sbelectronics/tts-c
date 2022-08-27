typedef struct rule {
  char *left;
  char *match;
  char *right;
  char output[8];
} rule;

#define ANYTHING (char*)1
#define NOTHING " "
#define NOMORE (char*)3

#define STOP 127

typedef char PHONEME;

extern rule rules[];
extern int rulemap[128];
extern char *phones[];
