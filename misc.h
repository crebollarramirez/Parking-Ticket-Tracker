#ifndef _MISC_H_
#define _MISC_H_
#define TABSZ 8171  /* default hash table size a prime number */
#define MINTABSZ 1  /* Smallest allowable hash table size */

/*
 * function prototypes
 */
void dropmsg(char *, unsigned long, char **);
int do_opts(int, char **, char **, char **, uint32_t *, int *);
#endif
