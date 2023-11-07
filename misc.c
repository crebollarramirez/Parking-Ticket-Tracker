#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "misc.h"

/*
 * option processing using standard getopt (not the gnu version)
 * There is one required option -c col_cnt
 * We check all the args to find as many errors in the command line 
 * note getopt() passes values using global varibles: optarg, optind
 * these globals are declared in unistd.h
 */
int do_opts(int argc, char **argv, char **ticknm, char **finenm,
        uint32_t *tabsz, int *silent){
    int opt;            /* option flag returned by getopt */
    int err_opt = 0;    /* set to non-zero if one of the args is bad */
    int saw_d = 0;      /* did we see the manditory -d option */
    int saw_f = 0;      /* did we see the manditory -f option */
    char *endptr;       /* for strtol(); ensure entire argument is parsed */
    
    /*
     * defaults 
     */
    *tabsz = TABSZ;
    *silent = 0;
    
    /*
     * call getopt for each arg
     */
    while ((opt = getopt(argc, argv, "+d:f:t:s")) != -1) {
        switch (opt) {
        case 'd':
            /*
             * name of ticket data to read up
             */
            *ticknm  = optarg;
            saw_d = 1;
            break;
        case 'f':
            /*
             * name of fine file to read up
             */
            *finenm  = optarg;
            saw_f = 1;
            break;
        case 's':
            *silent = 1;
            break;
        case 't':
            /*
             * over-ride of hash table size
             */
            errno = 0;
            *tabsz = (uint32_t)strtoul(optarg, &endptr, 10);
            if ((*endptr != '\0') || (errno != 0) || (*tabsz < MINTABSZ)) {
                fprintf(stderr, "%s: -t operand bad value\n", *argv);
                err_opt = 1;
            }
            break;
        case '?':
        default:
            /*
             * this is the case for error message handle directly by getopt()
             * we just mark we got an error
             */
            err_opt = 1;
            break;
        }
    }

    /*
     * make sure the user specified -c
     */
    if (!saw_d) {
        fprintf(stderr, "%s: -d operand is required\n", *argv);
        err_opt = 1;
    }
    /*
     * make sure the user specified -f
     */
    if (!saw_f) {
        fprintf(stderr, "%s: -f operand is required\n", *argv);
        err_opt = 1;
    }

    /*
     * make sure that there are no more args
     */
    if (optind != argc) {
        fprintf(stderr, "%s: Too many args\n", *argv);
        err_opt = 1;
    }

    /*
     * if bad or missing required option, print usage and exit
     */
    if (err_opt) {
        fprintf(stderr,
            "Usage: %s -d ticket_file -f fine_file [-t size] [-s]\n", *argv);
        return -1;
    }
    return 0;
}

/*
 * function for printing dropped error messages to stderr
 */
void dropmsg(char *mesg, unsigned long lineno, char **argv){
    fprintf(stderr, "%s: drop line %lu, %s\n", *argv, lineno, mesg);
}
