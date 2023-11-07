/* Usage ./parking -c cvs_file -f fine_file
 *
 * reads the fine table up to get the fines for
 * each type of parking ticket 1-99
 *
 * fine table (csv):
 * code,description,fine
 *
 * read the ticket data in the cvs file into a
 * into a in memory hashed database
 *
 * ticket file (csv):
 * summons_id,state,plate,date,code
 *
 * assumes the above file formats
 * 
 * error handling:
 *     print out error for bad arguments and malformed lines
 *     will drop bad ticket records and run
 *     will not run on a bad fine table
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "hashdb.h"
#include "misc.h"
#include "readsubs.h"
#include "commands.h"

/*
 * main
 *
 * Arguments: argc, argv
 * 
 * Operation:  driver for in memory ticket database
 *  parses options
 *  open files, then reads up fines, read up ticket data, closes files
 *
 *  Has a simple command line interface; runs until EOF on stdio or q command
 *
 * Commands
 *  f plate state - prints all the tickets for the specified vehicle
 *          prints all the fields for tickets on the vehicle
 *  p plate state summons_id - pays (removes) ticket update total due
 *          prints a receipt of summons (details of summons)
 *          removes vehicle from database if no more tickets left
 *  l - prints the largest number of tickets and total fines in the database
 *  v - verify ticket count and total fines are correct for each vehicle
 *  q - quit the program - free up memory and exit 
 */ 
int main(int argc, char **argv){
    struct vehicle **htable;/* pointer to hash table */
    struct fine *fineTab;   /* table of fines by code 1-99 */
    uint32_t tabsz;         /* hash table size */
    char *datanm;           /* name of ticket data file name */
    char *finenm;           /* name of fines data file name */
    int silent;             /* silent mode for script testing */

    /*
     * parse the command line args.
     * if if there was an error, return with EXIT_FAILURE
     */
    if (do_opts(argc, argv, &datanm, &finenm, &tabsz, &silent) != 0)
        return EXIT_FAILURE;

    /*
     * load the fine table. The fine codes run 1-99
     * so fineTab[1] is the fine for code 1, etc.
     * use calloc() so the table is easy to free if there
     * are errors building it (small table) and keep valgrind
     * happy.
     */
    if ((fineTab = malloc(CODES * sizeof(struct fine))) == NULL) {
        fprintf(stderr,"%s: calloc of fine table failed\n", *argv);
        return EXIT_FAILURE;
    }
    if (read_fines(fineTab, finenm, argv) != 0) {
        free_fines(fineTab);
        return EXIT_FAILURE;
    }

    /*
     * load the ticket table
     * allocate the hash table, then fill it
     */
    if ((htable = calloc(tabsz, sizeof(struct vehicle *))) == NULL) {
        fprintf(stderr,"%s: calloc of ticket table failed\n", *argv);
        free_fines(fineTab);
        return EXIT_FAILURE;
    }
    if (read_tickets(htable, tabsz, fineTab, datanm, argv) != 0) {
        free_tickets(htable, tabsz);
        free_fines(fineTab);
        return EXIT_FAILURE;
    }

    /*
     * interactive commands
     */
    commands(htable, tabsz, fineTab, argv, silent);

    free_tickets(htable, tabsz);
    free_fines(fineTab);
    return EXIT_SUCCESS;
}
