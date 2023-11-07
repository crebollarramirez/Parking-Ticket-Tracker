#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "hashdb.h"
#include "readsubs.h"
#include "misc.h"
#include "split_input.h"

/*
 * function for reading up the fines table. 
 * the fine table is a csv file with three fields per record 
 * code,description,fine
 * returns 0 on success, -1 on failure
 */
 int read_fines(struct fine *fineTab, char *finenm, char **argv){
    FILE *fp;
    char *buf = NULL;       /* input buffer, getline() will allocate it */
    size_t bufsz = 0;       /* size of buffer altered by getline()*/
    unsigned long linecnt;  /* count of lines in finetable */
    int code;               /* code entry in record */
    char *endptr;           /* for strtol(); ensure entire argument is parsed */
    char *coltab[FCOLS];    /* pointers to fine table fields */
    int retval = 0;         /* return value */

    if ((fp = fopen(finenm, "r")) == NULL) {
        fprintf(stderr, "%s: unable to open %s for read\n", *argv, finenm);
        return -1;
    }

    /*
     * code 0 is not used, but set it to 0 dollars anyway
     */
    fineTab->desc = strdup("INVALID");
    fineTab->fine = 0U;
    /*
     * skip over the description
     */
    if (getline(&buf, &bufsz, fp) <= 0) {
        fprintf(stderr, "%s: error reading header in fine table\n", *argv);
        free(buf);
        fclose(fp);
        return -1;
    }
    fineTab++;
    linecnt = 1UL;

    /*
     * loop through the rest of the codes
     */
    while (getline(&buf, &bufsz, fp) > 0) {
        if (++linecnt > CODES) {
            fprintf(stderr, "%s: fine table, too many entries\n", *argv);
            retval = -1;
            break;
        }
        /*
         * find the fields in the table
         */
        if (split_input(buf, FDELIM, FCOLS, coltab, linecnt, argv) != 0) {
            retval = -1;
            break;
        }

        /*
         * validate the code is proper
         * a proper fine table is in sorted order
         */
        errno = 0;
        code = (int)strtoul(coltab[CODE_IN], &endptr, 10);
        if ((errno != 0) || (*endptr != '\0') || (code != (int)(linecnt -1))) {
            fprintf(stderr, "%s: fine table record %lu bad code field\n",
                 *argv, linecnt);
            retval = -1;
            break;
        }

        /*
         * check the description
         */
        if ((coltab[DESC_IN] == NULL) || (*coltab[DESC_IN] == '\0')) {
            fprintf(stderr, "%s: fine table record %lu bad description\n",
                 *argv, linecnt);
            retval = -1;
            break;
        }
       
        /*
         * copy the description
         */
        fineTab->desc = strdup(coltab[DESC_IN]);
        
        /*
         * get the fine
         */
        errno = 0;
        fineTab->fine = (unsigned int)strtoul(coltab[FINE_IN], &endptr, 10);
        if ((errno != 0) || ((*endptr != '\n') && (*endptr != '\0'))) {
            fprintf(stderr, "%s: fine table record %lu bad fine field\n",
                 *argv, linecnt);
            retval = -1;
            break;
        }
        fineTab++;
    }

    free(buf);
    fclose(fp);
    return retval;
}

/*
 * function to free fine table memory
 */
void free_fines(struct fine *fineTab){
    int i;

    if (fineTab == NULL)
        return;
    for (i = 0; i < CODES; i++) {
        if (fineTab[i].desc != NULL)
            free(fineTab[i].desc);
    }
    free(fineTab);
    return;
}

/*
 * function for reading up the ticket table. 
 * the ticket fine table is a csv file with five fields per record
 * summons_id,plate,state,date,code
 * returns 0 on success, -1 on failure
 */
 int read_tickets(struct vehicle **hashtab, uint32_t tabsz, struct fine *fineTab, char *datanm, char **argv){
    FILE *fp;
    char *buf = NULL;       /* input buffer, getline() will allocate it */
    size_t bufsz = 0;       /* size of buffer altered by getline()*/
    unsigned long linecnt;  /* count of lines in finetable */
    int code;               /* code entry in record */
    char *endptr;           /* for strtol(); ensure entire argument is parsed */
    char *coltab[DCOLS];    /* pointers to ticket table fields */
    int retval = 0;         /* return value */

    if ((fp = fopen(datanm, "r")) == NULL) {
        fprintf(stderr, "%s: unable to open %s for reading ticket data\n", *argv, datanm);
        return -1;
    }

    /*
     * skip over the description
     */
    if (getline(&buf, &bufsz, fp) <= 0) {
        fprintf(stderr, "%s: error reading header in ticket file\n", *argv);
        fclose(fp);
        free(buf);
        return -1;
    }
    linecnt = 1UL;
    
    /*
     * loop through the rest of the codes
     */
    while (getline(&buf, &bufsz, fp) > 0) {
        linecnt++;
        /*
         * find the fields in the table
         */
        if (split_input(buf, DDELIM, DCOLS, coltab, linecnt, argv) != 0) {
            retval = -1;
            break;
        }

        /*
         * translate the code to an int
         */
        errno = 0;
        code = (int)strtoul(coltab[TCODE_IN], &endptr, 10);
        if ((errno != 0) || (*endptr != '\0') || (code >= CODES)) {
            fprintf(stderr, "%s: ticket file record %lu bad code field\n",
                 *argv, linecnt);
            retval = -1;
            break;
        }
        if (insert_ticket(hashtab, tabsz, fineTab, coltab[SUMM_IN], coltab[PLATE_IN],
            coltab[STATE_IN], coltab[DATE_IN], code, argv) != 0) {
            retval = -1;
            break;
        }
    }

    free(buf);
    fclose(fp);
    return retval;
}
