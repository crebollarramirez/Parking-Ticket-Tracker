#ifndef _HASHDB_H_
#define _HASHDB_H_

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))

/*
 * fine description by code (table index is code)
 * given a summons code this maps to a description and fine amount
 */
struct fine {
    char *desc;             /* text description of code */
    unsigned int fine;      /* value of the fine in $ */
};

/*
 * a specific summons
 * Note: for this PA summons as a long is ok, but it should be bigger
 *       also time_t on 32-bit OS may wrap in 2038....
 */
struct ticket {
    unsigned long long summons;/* summons or ticket id */
    time_t date;            /* date summons was issued */
    unsigned int code;      /* fine code 1-99 */
    struct ticket *next;    /* pointer to next ticket */
};

/*
 * a vehicle that has at least one summons
 */
struct vehicle {
    char *state;            /* state on license plate */
    char *plate;            /* id on license plate */
    unsigned int tot_fine;  /* summary field; all tickets */
    unsigned int cnt_ticket;/* number of tickets unpaid */
    struct vehicle *next;   /* pointer to next vehicle on hash chain */
    struct ticket *head;    /* pointer to first ticket; add tickets to end! */
};

#define TMCT 3      /* digits in a date mon/day/year */
/*
 * function prototypes
 */
int insert_ticket(struct vehicle **, uint32_t, struct fine *, char *,
        char *, char *, char *, int, char **);
struct vehicle *vehicle_lookup(struct vehicle **, uint32_t, char *, 
        char *, char **);
unsigned long dump_index(uint32_t, struct vehicle **, uint32_t, struct fine *,
        unsigned long *, char **);
void dump_db(struct vehicle **, uint32_t, struct fine *, char **);
void verify_db(struct vehicle **, uint32_t, struct fine *);
void free_tickets(struct vehicle **, uint32_t);
unsigned int print_vehicle(struct vehicle *, struct fine *, char **);
void largest(struct vehicle **, uint32_t);
int del_ticket(struct vehicle **, uint32_t, struct fine *, char *,
        char *state, char *summ, char **argv);
int strtoDate(char *, time_t *, char **);
int strtosumid(char *, unsigned long long *, char **);
uint32_t hash(char *, char **);
#endif
