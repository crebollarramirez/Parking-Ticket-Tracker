#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "hashdb.h"

#define DATESZ 12   /* buffer space when converting a date */

/*
 * strtoDate
 *      converts a date in a string to an integer in Linux format
 *      saves space in the database
 *      used to convert a summons date string to the field in struct ticket
 * use example:
 *      time_t dateval;
 *      if (strtoDate(date, &dateval, argv) != 0)
 *           error handling
 *      new_ticket->date = dateval;
 *
 * input:
 *  date: string to convert in month/date/year 01/01/2022
 *  ltime: output variable pointer to time in linux format (time_t type)
 *  argv: argv for error message printing
 *
 * return: 0 is conversion is ok, -1 otherwise
 */

int
strtoDate(char *date, time_t *ltime, char **argv)
{
    struct tm tmd = {0}; /* set all members to zero */

    if ((date == NULL) || (ltime == NULL)) {
        fprintf(stderr, "%s: strtoDate() passed a NULL\n", *argv);
        return -1;
    }
    if (sscanf(date,
            "%d/%d/%d", &tmd.tm_mon, &tmd.tm_mday, &tmd.tm_year) != TMCT) {
        fprintf(stderr, "%s: add_ticket() date bad value %s\n", *argv, date);
        return -1;
    }
    tmd.tm_isdst = -1;      /* use local daylight setting */
    tmd.tm_mon--;           /* convert to months since jan */
    tmd.tm_year -= 1900;    /* Number of years from 1900 */
    *ltime =  mktime(&tmd);
    return 0;
}

/*
 * strtosumid
 *      converts a summons id string to long long integer
 *      saves space in the database
 * use example:
 *      unsigned long long summid;
 *      if (strtosumid(summ, &summid, argv) != 0)
 *          return -1;
 *      new_ticket->summons = summid;
 *
 * input:
 *  summ:summons string (ascii numbers)
 *  argv: for error message printing
 *
 * return: 0 is converted ok -1 otherwise
 */

int
strtosumid(char *summ, unsigned long long *summid, char **argv)
{
    char *endptr;

    if ((summ == NULL) || (summid == NULL)) {
        fprintf(stderr, "%s: strtoDate() passed a NULL\n", *argv);
        return -1;
    }
    errno = 0;
    *summid = strtoull(summ, &endptr, 10);
    if ((*endptr != '\0') || (errno != 0)) {
        fprintf(stderr, "%s: strtosumid() summons bad value %llu\n",
            *argv, *summid);
        return -1;
    }
    return 0;
}

/*
 *
 * hash
 *
 * Args:
 *  str     string to be hashed
 *  argv    for printing error message
 *
 * Operation: calculates a hash value for the string
 *        returns the hash value
 */
uint32_t
hash(char *str, char **argv)
{
    uint32_t hash = 0;
    uint32_t c;

    if (str == NULL) {
        fprintf(stderr, "%s: hash() passed NULL\n", *argv);
        return 0UL;
    }

    while ((c = (unsigned char)*str++) != '\0')
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}
