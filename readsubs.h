#ifndef _READSUBS_H_
#define _READSUBS_H_

#define FDELIM ','  /* delimiter in fine table */
#define DDELIM ','  /* delimiter in data table */
#define FCOLS 3     /* fine table column count */
#define DCOLS 5     /* data table column count */
#define CODES 100   /* number of fine codes + 1 */
#define CODE_IN 0   /* code field in fine table */
#define DESC_IN 1   /* description field in fine table */
#define FINE_IN 2   /* fine field in fine table */
#define SUMM_IN 0   /* summons field in ticket table */
#define PLATE_IN 1  /* plate field in ticket table */
#define STATE_IN 2  /* plate field in ticket table */
#define DATE_IN 3   /* date field in ticket table */
#define TCODE_IN 4  /* code field in ticket table */

/*
 * function prototypes
 */
int read_fines(struct fine *, char *, char **);
int read_tickets(struct vehicle **, uint32_t, struct fine*, char *,
        char **);
void free_fines(struct fine *);
#endif
